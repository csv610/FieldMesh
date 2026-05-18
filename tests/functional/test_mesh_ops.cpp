#include "../src/meshstats.h"
#include "../src/meshio.h"
#include "../src/adjacency.h"
#include "../src/dedge.h"
#include "../tests/test_framework.h"
#include <fstream>

// Forward declarations
bool test_mesh_stats_empty_mesh();
bool test_mesh_stats_single_triangle();
bool test_mesh_stats_two_triangles();
bool test_compute_dual_vertex_areas();
bool test_dedge_basic();
bool test_dedge_two_triangles();
bool test_adjacency_uniform_basic();
bool test_adjacency_uniform_two_triangles();
bool test_meshio_write_and_read_triangle();
bool test_meshio_write_ply();

int main() {
    std::cout << "=== Functional Tests (Mesh Operations) ===" << std::endl;

    int passed = 0, failed = 0;

    RUN_TEST(mesh_stats_empty_mesh);
    RUN_TEST(mesh_stats_single_triangle);
    RUN_TEST(mesh_stats_two_triangles);
    RUN_TEST(compute_dual_vertex_areas);
    RUN_TEST(dedge_basic);
    RUN_TEST(dedge_two_triangles);
    RUN_TEST(adjacency_uniform_basic);
    RUN_TEST(adjacency_uniform_two_triangles);
    RUN_TEST(meshio_write_and_read_triangle);
    RUN_TEST(meshio_write_ply);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}

bool test_mesh_stats_empty_mesh() {
    MatrixXu F(3, 0);
    MatrixXf V(3, 0);

    MeshStats stats = compute_mesh_stats(F, V, false);

    ASSERT_FLOAT_EQ(stats.mAABB.min.x(), std::numeric_limits<Float>::infinity(), 1e-6f);
    ASSERT_FLOAT_EQ(stats.mSurfaceArea, 0.0, 1e-6);
    return true;
}

bool test_mesh_stats_single_triangle() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    MeshStats stats = compute_mesh_stats(F, V, false);

    ASSERT_FLOAT_EQ(stats.mSurfaceArea, 0.5, 1e-6);
    ASSERT_FLOAT_EQ(stats.mAABB.min.x(), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(stats.mAABB.max.x(), 1.0f, 1e-6f);
    return true;
}

bool test_mesh_stats_two_triangles() {
    MatrixXu F(3, 2);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 2, 3;

    MatrixXf V(3, 4);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.5f, 1.0f, 0.0f;
    V.col(3) << -0.5f, 1.0f, 0.0f;

    MeshStats stats = compute_mesh_stats(F, V, false);

    ASSERT_FLOAT_EQ(stats.mSurfaceArea, 1.0, 1e-5);
    return true;
}

bool test_compute_dual_vertex_areas() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    VectorXf A;
    compute_dual_vertex_areas(F, V, V2E, E2E, nonManifold, A);

    ASSERT_EQ(A.size(), 3);
    ASSERT_FLOAT_EQ(A.sum(), 0.5f, 1e-6f);
    return true;
}

bool test_dedge_basic() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.5f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;

    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    ASSERT_EQ(V2E.size(), 3);
    ASSERT_EQ(E2E.size(), 3);

    ASSERT_FALSE(nonManifold.any());
    return true;
}

bool test_dedge_two_triangles() {
    MatrixXu F(3, 2);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 2, 3;

    MatrixXf V(3, 4);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.5f, 1.0f, 0.0f;
    V.col(3) << -0.5f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;

    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    ASSERT_EQ(V2E.size(), 4);
    ASSERT_EQ(E2E.size(), 6);

    ASSERT_FALSE(nonManifold.any());
    return true;
}

bool test_adjacency_uniform_basic() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    AdjacencyMatrix adj = generate_adjacency_matrix_uniform(F, V2E, E2E, nonManifold);

    ASSERT_TRUE(adj);

    ASSERT_TRUE(adj[0] != adj[1]);
    ASSERT_TRUE(adj[1] != adj[2]);
    ASSERT_TRUE(adj[2] != adj[0]);

    return true;
}

bool test_adjacency_uniform_two_triangles() {
    MatrixXu F(3, 2);
    F.col(0) << 0, 1, 2;
    F.col(1) << 0, 2, 3;

    MatrixXf V(3, 4);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.5f, 1.0f, 0.0f;
    V.col(3) << -0.5f, 1.0f, 0.0f;

    VectorXu V2E, E2E;
    VectorXb boundary, nonManifold;
    build_dedge(F, V, V2E, E2E, boundary, nonManifold);

    AdjacencyMatrix adj = generate_adjacency_matrix_uniform(F, V2E, E2E, nonManifold);

    ASSERT_TRUE(adj);

    bool foundSharedEdge = false;
    for (const Link* l = adj[0]; l != adj[1]; ++l) {
        if (l->id == 2) foundSharedEdge = true;
    }
    ASSERT_TRUE(foundSharedEdge);

    return true;
}

bool test_meshio_write_and_read_triangle() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    MatrixXf N(3, 3);
    N.col(0) << 0.0f, 0.0f, 1.0f;
    N.col(1) << 0.0f, 0.0f, 1.0f;
    N.col(2) << 0.0f, 0.0f, 1.0f;

    std::string filename = "/tmp/test_mesh_output.obj";
    write_obj(filename, F, V, N);

    MatrixXu F2;
    MatrixXf V2;
    load_obj(filename, F2, V2);

    ASSERT_EQ(F2.cols(), 1);
    ASSERT_EQ(V2.cols(), 3);

    ASSERT_EQ(F2(0, 0), 0);
    ASSERT_EQ(F2(1, 0), 1);
    ASSERT_EQ(F2(2, 0), 2);

    std::remove(filename.c_str());
    return true;
}

bool test_meshio_write_ply() {
    MatrixXu F(3, 1);
    F.col(0) << 0, 1, 2;

    MatrixXf V(3, 3);
    V.col(0) << 0.0f, 0.0f, 0.0f;
    V.col(1) << 1.0f, 0.0f, 0.0f;
    V.col(2) << 0.0f, 1.0f, 0.0f;

    MatrixXf N(3, 3);
    N.col(0) << 0.0f, 0.0f, 1.0f;
    N.col(1) << 0.0f, 0.0f, 1.0f;
    N.col(2) << 0.0f, 0.0f, 1.0f;

    std::string filename = "/tmp/test_mesh_output.ply";
    write_ply(filename, F, V, N);

    MatrixXu F2;
    MatrixXf V2, N2;
    load_ply(filename, F2, V2, N2);

    ASSERT_EQ(F2.cols(), 1);
    ASSERT_EQ(V2.cols(), 3);
    ASSERT_EQ(N2.cols(), 3);

    std::remove(filename.c_str());
    return true;
}