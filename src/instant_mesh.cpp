/*
    instant_mesh.cpp -- High-level interface to the Instant Meshes library

    This file is part of the implementation of

        Instant Field-Aligned Meshes
        Wenzel Jakob, Daniele Panozzo, Marco Tarini, and Olga Sorkine-Hornung
        In ACM Transactions on Graphics (Proc. SIGGRAPH Asia 2015)

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include "instant_mesh.h"
#include "meshio.h"
#include "dedge.h"
#include "subdivide.h"
#include "normal.h"
#include "extract.h"

int nprocs = -1;

InstantMesh::InstantMesh()
    : mBVH(nullptr), mPointCloud(false) { }

InstantMesh::~InstantMesh() {
    if (mBVH)
        delete mBVH;
}

void InstantMesh::load(const std::string &filename) {
    mInputFilename = filename;
    load_mesh_or_pointcloud(mInputFilename, mF_input, mV_input, mN_input, mProgressCallback);
    mPointCloud = mF_input.size() == 0;
}

void InstantMesh::setMeshData(const MatrixXu &F, const MatrixXf &V, const MatrixXf &N) {
    mF_input = F;
    mV_input = V;
    mN_input = N;
    mPointCloud = mF_input.size() == 0;
    mInputFilename = "memory";
}

void InstantMesh::save(const std::string &filename) {
    if (mV_extracted.cols() == 0)
        throw std::runtime_error("InstantMesh::save: No mesh has been extracted yet!");
    write_mesh(filename, mF_extracted, mV_extracted, MatrixXf(), mNf_extracted);
}

void InstantMesh::preprocess() {
    if (mV_input.cols() == 0)
        throw std::runtime_error("InstantMesh::preprocess: No mesh has been loaded yet!");

    MatrixXu F = mF_input;
    MatrixXf V = mV_input;
    MatrixXf N = mN_input;
    VectorXf A;
    std::set<uint32_t> crease_in;
    AdjacencyMatrix adj = nullptr;

    mStats = compute_mesh_stats(F, V, mConfig.deterministic, mProgressCallback);

    if (mBVH) {
        delete mBVH;
        mBVH = nullptr;
    }

    if (mPointCloud) {
        mBVH = new BVH(&F, &V, &N, mStats.mAABB);
        mBVH->build(mProgressCallback);
        adj = generate_adjacency_matrix_pointcloud(V, N, mBVH, mStats, mConfig.knnPoints, mConfig.deterministic, mProgressCallback);
        A.resize(V.cols());
        A.setConstant(1.0f);
    }

    int vertexCount = mConfig.vertexCount;
    int faceCount = mConfig.faceCount;
    Float scale = mConfig.scale;

    if (scale < 0 && vertexCount < 0 && faceCount < 0) {
        vertexCount = (int) V.cols();
    }

    if (scale > 0) {
        Float face_area = mConfig.posy == 4 ? (scale*scale) : (std::sqrt(3.f)/4.f*scale*scale);
        faceCount = mStats.mSurfaceArea / face_area;
        vertexCount = mConfig.posy == 4 ? faceCount : (faceCount / 2);
    } else if (faceCount > 0) {
        Float face_area = mStats.mSurfaceArea / faceCount;
        vertexCount = mConfig.posy == 4 ? faceCount : (faceCount / 2);
        scale = mConfig.posy == 4 ? std::sqrt(face_area) : (2*std::sqrt(face_area * std::sqrt(1.f/3.f)));
    } else if (vertexCount > 0) {
        faceCount = mConfig.posy == 4 ? vertexCount : (vertexCount * 2);
        Float face_area = mStats.mSurfaceArea / faceCount;
        scale = mConfig.posy == 4 ? std::sqrt(face_area) : (2*std::sqrt(face_area * std::sqrt(1.f/3.f)));
    }

    if (!mPointCloud) {
        VectorXu V2E, E2E;
        VectorXb boundary, nonManifold;
        if (mStats.mMaximumEdgeLength*2 > scale || mStats.mMaximumEdgeLength > mStats.mAverageEdgeLength * 2) {
            build_dedge(F, V, V2E, E2E, boundary, nonManifold, mProgressCallback);
            subdivide(F, V, V2E, E2E, boundary, nonManifold, std::min(scale/2, (Float) mStats.mAverageEdgeLength*2), mConfig.deterministic, mProgressCallback);
        }
        build_dedge(F, V, V2E, E2E, boundary, nonManifold, mProgressCallback);
        adj = generate_adjacency_matrix_uniform(F, V2E, E2E, nonManifold, mProgressCallback);

        if (mConfig.creaseAngle >= 0)
            generate_crease_normals(F, V, V2E, E2E, boundary, nonManifold, mConfig.creaseAngle, N, crease_in, mProgressCallback);
        else
            generate_smooth_normals(F, V, V2E, E2E, nonManifold, N, mProgressCallback);

        compute_dual_vertex_areas(F, V, V2E, E2E, nonManifold, A, mProgressCallback);
        mRes.setE2E(std::move(E2E));
    }

    mRes.setAdj(std::move(adj));
    mRes.setF(std::move(F));
    mRes.setV(std::move(V));
    mRes.setA(std::move(A));
    mRes.setN(std::move(N));
    mRes.setScale(scale);
    mRes.build(mConfig.deterministic, mProgressCallback);
    mRes.resetSolution();

    if (mConfig.alignToBoundaries && !mPointCloud) {
        mRes.clearConstraints();
        for (uint32_t i=0; i<3*mRes.F().cols(); ++i) {
            if (mRes.E2E()[i] == INVALID) {
                uint32_t i0 = mRes.F()(i%3, i/3);
                uint32_t i1 = mRes.F()((i+1)%3, i/3);
                Vector3f p0 = mRes.V().col(i0), p1 = mRes.V().col(i1);
                Vector3f edge = p1-p0;
                if (edge.squaredNorm() > 0) {
                    edge.normalize();
                    mRes.CO().col(i0) = p0;
                    mRes.CO().col(i1) = p1;
                    mRes.CQ().col(i0) = mRes.CQ().col(i1) = edge;
                    mRes.CQw()[i0] = mRes.CQw()[i1] = mRes.COw()[i0] = mRes.COw()[i1] = 1.0f;
                }
            }
        }
        mRes.propagateConstraints(mConfig.rosy, mConfig.posy);
    }

    if (!mBVH && mConfig.smoothIter > 0) {
        mBVH = new BVH(&mRes.F(), &mRes.V(), &mRes.N(), mStats.mAABB);
        mBVH->build();
    }
}

void InstantMesh::optimizeOrientation() {
    Optimizer optimizer(mRes, false);
    optimizer.setRoSy(mConfig.rosy);
    optimizer.setPoSy(mConfig.posy);
    optimizer.setExtrinsic(mConfig.extrinsic);
    optimizer.optimizeOrientations(-1);
    optimizer.notify();
    optimizer.wait();
    optimizer.shutdown();
}

void InstantMesh::optimizePosition() {
    Optimizer optimizer(mRes, false);
    optimizer.setRoSy(mConfig.rosy);
    optimizer.setPoSy(mConfig.posy);
    optimizer.setExtrinsic(mConfig.extrinsic);
    optimizer.optimizePositions(-1);
    optimizer.notify();
    optimizer.wait();
    optimizer.shutdown();
}

void InstantMesh::extractMesh() {
    MatrixXf O_extr, N_extr;
    std::set<uint32_t> crease_in, crease_out;
    std::vector<std::vector<TaggedLink>> adj_extr;
    extract_graph(mRes, mConfig.extrinsic, mConfig.rosy, mConfig.posy, adj_extr, O_extr, N_extr,
                  crease_in, crease_out, mConfig.deterministic);

    extract_faces(adj_extr, O_extr, N_extr, mNf_extracted, mF_extracted, mConfig.posy,
            mRes.scale(), crease_out, true, mConfig.pureQuad, mBVH, mConfig.smoothIter);
    mV_extracted = O_extr;
}

void InstantMesh::process() {
    preprocess();
    optimizeOrientation();
    optimizePosition();
    extractMesh();
}

void InstantMesh::createQuadmesh(int targetFaces) {
    mConfig.rosy = 4;
    mConfig.posy = 4;
    mConfig.pureQuad = true;
    if (targetFaces > 0)
        mConfig.faceCount = targetFaces;
    process();
}

void InstantMesh::createTrimesh(int targetFaces) {
    mConfig.rosy = 6;
    mConfig.posy = 3;
    mConfig.pureQuad = false;
    if (targetFaces > 0)
        mConfig.faceCount = targetFaces;
    process();
}

void InstantMesh::remesh(int targetFaces) {
    if (targetFaces > 0)
        mConfig.faceCount = targetFaces;

    if (mF_input.rows() == 3) {
        mConfig.rosy = 6;
        mConfig.posy = 3;
        mConfig.pureQuad = false;
    } else {
        mConfig.rosy = 4;
        mConfig.posy = 4;
        mConfig.pureQuad = true;
    }
    process();
}
