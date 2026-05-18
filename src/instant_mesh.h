/*
    instant_mesh.h -- High-level interface to the Instant Meshes library

    This file is part of the implementation of

        Instant Field-Aligned Meshes
        Wenzel Jakob, Daniele Panozzo, Marco Tarini, and Olga Sorkine-Hornung
        In ACM Transactions on Graphics (Proc. SIGGRAPH Asia 2015)

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include "hierarchy.h"
#include "field.h"
#include "bvh.h"
#include "meshstats.h"
#include "instant_mesh_config.h"

class InstantMesh {
public:
    InstantMesh();
    ~InstantMesh();

    /* Mesh Loading and Saving */
    void load(const std::string &filename);
    void setMeshData(const MatrixXu &F, const MatrixXf &V, const MatrixXf &N = MatrixXf());
    void save(const std::string &filename);

    /* Configuration */
    void setConfig(const InstantMeshConfig &config) { mConfig = config; }
    const InstantMeshConfig &config() const { return mConfig; }

    void setTargetVertices(int count) { mConfig.vertexCount = count; }
    void setTargetFaces(int count) { mConfig.faceCount = count; }
    void setRoSy(int rosy) { mConfig.rosy = rosy; }
    void setPoSy(int posy) { mConfig.posy = posy; }
    void setScale(Float scale) { mConfig.scale = scale; }
    void setFaceCount(int count) { mConfig.faceCount = count; }
    void setVertexCount(int count) { mConfig.vertexCount = count; }
    void setCreaseAngle(Float angle) { mConfig.creaseAngle = angle; }
    void setExtrinsic(bool extrinsic) { mConfig.extrinsic = extrinsic; }
    void setAlignToBoundaries(bool align) { mConfig.alignToBoundaries = align; }
    void setSmoothIter(int iter) { mConfig.smoothIter = iter; }
    void setKnnPoints(int knn) { mConfig.knnPoints = knn; }
    void setPureQuad(bool pure) { mConfig.pureQuad = pure; }
    void setDeterministic(bool deterministic) { mConfig.deterministic = deterministic; }
    void setProgressCallback(const ProgressCallback &cb) { mProgressCallback = cb; }

    /* Processing Steps */
    void preprocess();
    void optimizeOrientation();
    void optimizePosition();
    void extractMesh();

    /* Combined Execution */
    void process();

    /* High-level Remeshing Functions */
    void createQuadmesh(int targetFaces = -1);
    void createTrimesh(int targetFaces = -1);
    void remesh(int targetFaces = -1);

    int inputFaceCount() const { return (int) mF_input.cols(); }
    int inputVertexCount() const { return (int) mV_input.cols(); }
    int inputVerticesPerFace() const { return (int) mF_input.rows(); }

    const MatrixXf& vertices() const { return mV_extracted; }
    const MatrixXu& faces() const { return mF_extracted; }
    const MatrixXf& normals() const { return mNf_extracted; }

private:
    /* Processing Parameters */
    InstantMeshConfig mConfig;
    ProgressCallback mProgressCallback;

    /* Internal Data */
    MultiResolutionHierarchy mRes;
    BVH *mBVH;
    MeshStats mStats;
    std::string mInputFilename;
    bool mPointCloud;

    /* Input Data */
    MatrixXu mF_input;
    MatrixXf mV_input, mN_input;

    /* Extraction Result */
    MatrixXu mF_extracted;
    MatrixXf mV_extracted, mNf_extracted;
};
