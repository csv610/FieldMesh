#include "../src/meshstats.h"
#include "../src/meshio.h"
#include "../src/adjacency.h"
#include "../src/dedge.h"
#include "../src/normal.h"
#include "../src/serializer.h"
#include "../tests/test_framework.h"
#include <fstream>

bool test_full_pipeline_single_triangle() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    MeshStats stats = compute_mesh_stats(F, V, false);
    ASSERT_TRUE(stats.mSurfaceArea > 0);

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);
    ASSERT_EQ(V2E.size(), 3);

    AdjacencyMatrix adj = generate_adjacency_matrix_uniform(F, V2E, E2E, nonManifold);
    ASSERT_TRUE(adj);

    MatrixXf N;
    generate_smooth_normals(F, V, V2E, E2E, nonManifold, N);
    ASSERT_EQ(N.cols(), 3);

    VectorXf A;
    compute_dual_vertex_areas(F, V, V2E, E2E, nonManifold, A);
    ASSERT_EQ(A.size(), 3);
    ASSERT_TRUE(A.sum() > 0.0f);

    return true;
}

bool test_full_pipeline_quad_mesh() {
    MatrixXu F(3, 2);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 2, 3;

    MatrixXf V(3, 4);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 1.0f, 1.0f, 0.0f;
    V.col(3) << 0.0f, 1.0f, 0.0f;

    MeshStats stats = compute_mesh_stats(F, V, false);
    ASSERT_FLOAT_EQ(stats.mSurfaceArea, 1.0, 1e-6);

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    AdjacencyMatrix adj = generate_adjacency_matrix_uniform(F, V2E, E2E, nonManifold);
    ASSERT_TRUE(adj);

    MatrixXf N;
    generate_smooth_normals(F, V, V2E, E2E, nonManifold, N);

    VectorXf A;
    compute_dual_vertex_areas(F, V, V2E, E2E, nonManifold, A);
    ASSERT_FLOAT_EQ(A.sum(), 1.0f, 1e-5f);

    return true;
}

bool test_full_pipeline_with_crease_normals() {
    MatrixXu F(3, 2);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 2, 3;

    MatrixXf V(3, 4);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 1.0f, 1.0f, 0.0f;
    V.col(3) << 0.0f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    std::set<uint32_t> crease_in, crease_out;
    MatrixXf N;
    float creaseAngle = 0.5f;
    generate_crease_normals(F, V, V2E, E2E, boundary, nonManifold, creaseAngle, N, crease_in);

    ASSERT_EQ(N.cols(), 4);
    return true;
}

bool test_serializer_integration() {
    Serializer s;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    s.set("mesh.vertices", V);
    s.set("mesh.faces", F);

    MatrixXf V2;
    MatrixXu F2;

    bool vFound = s.get("mesh.vertices", V2);
    bool fFound = s.get("mesh.faces", F2);

    ASSERT_TRUE(vFound);
    ASSERT_TRUE(fFound);
    ASSERT_EQ(V2.cols(), 3);
    ASSERT_EQ(F2.cols(), 1);

    ASSERT_FLOAT_EQ(V2(0, 0), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(V2(0, 1), 1.0f, 1e-6f);
    ASSERT_FLOAT_EQ(V2(0, 2), 0.0f, 1e-6f);

    return true;
}

bool test_serializer_hierarchy_state() {
    Serializer s;

    s.set("hierarchy.level_count", (uint32_t)2);
    s.set("hierarchy.scale", 1.0f);

    s.pushPrefix("hierarchy");
    s.set("level0.vertex_count", (uint32_t)100);
    s.set("level0.face_count", (uint32_t)50);
    s.popPrefix();

    uint32_t levelCount = 0;
    float scale = 0.0f;
    uint32_t vCount = 0;
    uint32_t fCount = 0;

    bool lc = s.get("hierarchy.level_count", levelCount);
    bool sc = s.get("hierarchy.scale", scale);
    bool vc = s.get("hierarchy.level0.vertex_count", vCount);
    bool fc = s.get("hierarchy.level0.face_count", fCount);

    ASSERT_TRUE(lc); ASSERT_EQ(levelCount, 2);
    ASSERT_TRUE(sc); ASSERT_FLOAT_EQ(scale, 1.0f, 1e-6f);
    ASSERT_TRUE(vc); ASSERT_EQ(vCount, 100);
    ASSERT_TRUE(fc); ASSERT_EQ(fCount, 50);

    return true;
}

bool test_mesh_stats_deterministic() {
    MatrixXu F(3, 10);
    for (int i = 0; i < 10; i++) {
        F.col(i) << i % 3, (i + 1) % 3, (i + 2) % 3;
    }

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.5f, 1.0f, 0.0f;

    MeshStats stats1 = compute_mesh_stats(F, V, true);
    MeshStats stats2 = compute_mesh_stats(F, V, true);

    ASSERT_FLOAT_EQ(stats1.mSurfaceArea, stats2.mSurfaceArea, 1e-10);
    ASSERT_EQ(stats1.mAABB.largestAxis(), stats2.mAABB.largestAxis());

    return true;
}

bool test_adjacency_cotan_weights() {
    MatrixXu F(3, 2);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 2, 3;

    MatrixXf V(3, 4);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 1.0f, 1.0f, 0.0f;
    V.col(3) << 0.0f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    AdjacencyMatrix adj = generate_adjacency_matrix_cotan(F, V, V2E, E2E, nonManifold);
    ASSERT_TRUE(adj);

    return true;
}

bool test_boundary_detection() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    ASSERT_TRUE(boundary.all());
    ASSERT_FALSE(nonManifold.any());

    return true;
}

bool test_non_manifold_detection() {
    MatrixXu F(3, 4);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 1, 2;
    F.col(2) << 0, 1, 3;
    F.col(3) << 0, 1, 4;

    MatrixXf V(3, 5);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.5f, 1.0f, 0.0f;
    V.col(3) << -0.5f, 1.0f, 0.0f;
    V.col(4) << 0.5f, 0.5f, 1.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    ASSERT_TRUE(nonManifold.any() || boundary.any());

    return true;
}

int main() {
    std::cout << "=== Integration Tests ===" << std::endl;

    int passed = 0, failed = 0;

    RUN_TEST(full_pipeline_single_triangle);
    RUN_TEST(full_pipeline_quad_mesh);
    RUN_TEST(full_pipeline_with_crease_normals);
    RUN_TEST(serializer_integration);
    RUN_TEST(serializer_hierarchy_state);
    RUN_TEST(mesh_stats_deterministic);
    RUN_TEST(adjacency_cotan_weights);
    RUN_TEST(boundary_detection);
    RUN_TEST(non_manifold_detection);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}