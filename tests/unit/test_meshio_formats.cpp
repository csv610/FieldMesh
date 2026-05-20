#include "test_framework.h"
#include "meshio.h"
#include <filesystem>
#include <fstream>

bool test_off_format() {
    MatrixXu F(3, 1);
    F << 0, 1, 2;
    MatrixXf V(3, 3);
    V << 0, 1, 0,
         0, 0, 1,
         0, 0, 0;

    std::string filename = "test_mesh.off";
    write_off(filename, F, V);

    MatrixXu F_loaded;
    MatrixXf V_loaded, N_loaded;
    load_off(filename, F_loaded, V_loaded);

    ASSERT_EQ(V_loaded.cols(), V.cols());
    ASSERT_EQ(F_loaded.cols(), F.cols());
    ASSERT_TRUE((V_loaded - V).norm() < 1e-6);

    std::filesystem::remove(filename);
    return true;
}

bool test_off_quads() {
    // 0 1
    // 3 2
    std::string filename = "test_quad.off";
    std::ofstream os(filename);
    os << "OFF" << std::endl;
    os << "4 1 0" << std::endl;
    os << "0 0 0" << std::endl;
    os << "1 0 0" << std::endl;
    os << "1 1 0" << std::endl;
    os << "0 1 0" << std::endl;
    os << "4 0 1 2 3" << std::endl;
    os.close();

    MatrixXu F_loaded;
    MatrixXf V_loaded;
    load_off(filename, F_loaded, V_loaded);

    ASSERT_EQ(V_loaded.cols(), 4);
    ASSERT_EQ(F_loaded.cols(), 2); // 1 quad -> 2 triangles
    ASSERT_EQ(F_loaded.rows(), 3);

    std::filesystem::remove(filename);
    return true;
}

bool test_off_mixed() {
    std::string filename = "test_mixed.off";
    std::ofstream os(filename);
    os << "OFF" << std::endl;
    os << "4 2 0" << std::endl;
    os << "0 0 0" << std::endl;
    os << "1 0 0" << std::endl;
    os << "1 1 0" << std::endl;
    os << "0 1 0" << std::endl;
    os << "3 0 1 2" << std::endl;    // Triangle
    os << "4 0 1 2 3" << std::endl;  // Quad
    os.close();

    MatrixXu F_loaded;
    MatrixXf V_loaded;
    load_off(filename, F_loaded, V_loaded);

    ASSERT_EQ(V_loaded.cols(), 4);
    ASSERT_EQ(F_loaded.cols(), 3); // 1 triangle + 1 quad (2 triangles) = 3 triangles
    ASSERT_EQ(F_loaded.rows(), 3);

    std::filesystem::remove(filename);
    return true;
}

bool test_stl_format_welding() {
    // A single triangle with duplicate vertices in STL style
    MatrixXu F(3, 1);
    F << 0, 1, 2;
    MatrixXf V(3, 3);
    V << 0, 1, 0,
         0, 0, 1,
         0, 0, 0;

    std::string filename = "test_mesh.stl";
    write_stl(filename, F, V);

    MatrixXu F_loaded;
    MatrixXf V_loaded, N_loaded;
    load_stl(filename, F_loaded, V_loaded);

    // After welding, it should still have 3 vertices
    ASSERT_EQ(V_loaded.cols(), 3);
    ASSERT_EQ(F_loaded.cols(), 1);

    std::filesystem::remove(filename);
    return true;
}

bool test_generic_loader() {
    MatrixXu F(3, 1);
    F << 0, 1, 2;
    MatrixXf V(3, 3);
    V << 0, 1, 0,
         0, 0, 1,
         0, 0, 0;

    std::string filename = "test_mesh_generic.off";
    write_mesh(filename, F, V);

    MatrixXu F_loaded;
    MatrixXf V_loaded, N_loaded;
    load_mesh_or_pointcloud(filename, F_loaded, V_loaded, N_loaded);

    ASSERT_EQ(V_loaded.cols(), 3);
    ASSERT_EQ(F_loaded.cols(), 1);

    std::filesystem::remove(filename);
    return true;
}

bool test_ply_triangles() {
    MatrixXu F(3, 1);
    F << 0, 1, 2;
    MatrixXf V(3, 3);
    V << 0, 1, 0,
         0, 0, 1,
         0, 0, 0;

    std::string filename = "test_tri.ply";
    write_ply(filename, F, V);

    MatrixXu F_loaded;
    MatrixXf V_loaded, N_loaded;
    load_ply(filename, F_loaded, V_loaded, N_loaded);

    ASSERT_EQ(V_loaded.cols(), V.cols());
    ASSERT_EQ(F_loaded.cols(), F.cols());
    ASSERT_EQ(F_loaded.rows(), 3);
    ASSERT_TRUE((V_loaded - V).norm() < 1e-6);

    std::filesystem::remove(filename);
    return true;
}

bool test_ply_quads() {
    MatrixXu F(4, 1);
    F << 0, 1, 2, 3;
    MatrixXf V(3, 4);
    V << 0, 1, 1, 0,
         0, 0, 1, 1,
         0, 0, 0, 0;

    std::string filename = "test_quad.ply";
    write_ply(filename, F, V);

    MatrixXu F_loaded;
    MatrixXf V_loaded, N_loaded;
    load_ply(filename, F_loaded, V_loaded, N_loaded);

    ASSERT_EQ(V_loaded.cols(), V.cols());
    ASSERT_EQ(F_loaded.cols(), 2); // 1 quad -> 2 triangles
    ASSERT_EQ(F_loaded.rows(), 3);
    ASSERT_TRUE((V_loaded - V).norm() < 1e-6);

    std::filesystem::remove(filename);
    return true;
}

bool test_ply_mixed() {
    // Manually create a mixed PLY file
    std::string filename = "test_mixed.ply";
    std::ofstream os(filename);
    os << "ply" << std::endl;
    os << "format ascii 1.0" << std::endl;
    os << "element vertex 4" << std::endl;
    os << "property float x" << std::endl;
    os << "property float y" << std::endl;
    os << "property float z" << std::endl;
    os << "element face 2" << std::endl;
    os << "property list uchar int vertex_indices" << std::endl;
    os << "end_header" << std::endl;
    os << "0 0 0" << std::endl;
    os << "1 0 0" << std::endl;
    os << "1 1 0" << std::endl;
    os << "0 1 0" << std::endl;
    os << "3 0 1 2" << std::endl;    // Triangle
    os << "4 0 1 2 3" << std::endl;  // Quad
    os.close();

    MatrixXu F_loaded;
    MatrixXf V_loaded, N_loaded;
    
    load_ply(filename, F_loaded, V_loaded, N_loaded);

    ASSERT_EQ(V_loaded.cols(), 4);
    ASSERT_EQ(F_loaded.cols(), 3); // 1 triangle + 1 quad (2 triangles) = 3 triangles
    ASSERT_EQ(F_loaded.rows(), 3);

    std::filesystem::remove(filename);
    return true;
}

bool test_error_nonexistent_file() {
    MatrixXu F;
    MatrixXf V, N;
    try {
        load_mesh_or_pointcloud("nonexistent_file_12345.obj", F, V, N);
        return false; // Should have thrown
    } catch (const std::runtime_error &) {
        return true; // Expected
    }
}

bool test_error_malformed_off() {
    std::string filename = "test_malformed.off";
    std::ofstream os(filename);
    os << "OFF" << std::endl;
    os << "3 1 0" << std::endl;
    os << "0 0 0" << std::endl;
    os << "1 0" << std::endl;  // Malformed vertex
    os.close();

    MatrixXu F_loaded;
    MatrixXf V_loaded;
    bool caught = false;
    try {
        load_off(filename, F_loaded, V_loaded);
    } catch (const std::runtime_error &) {
        caught = true;
    }

    std::filesystem::remove(filename);
    return caught;
}

bool test_error_empty_off() {
    std::string filename = "test_empty.off";
    std::ofstream os(filename);
    os << "OFF" << std::endl;
    os << "0 0 0" << std::endl;
    os.close();

    MatrixXu F_loaded;
    MatrixXf V_loaded;
    load_off(filename, F_loaded, V_loaded);

    ASSERT_EQ(V_loaded.cols(), 0);
    ASSERT_EQ(F_loaded.cols(), 0);

    std::filesystem::remove(filename);
    return true;
}

int main() {
    std::cout << "=== MeshIO Format Tests ===" << std::endl;
    int passed = 0, failed = 0;

    RUN_TEST(off_format);
    RUN_TEST(off_quads);
    RUN_TEST(off_mixed);
    RUN_TEST(stl_format_welding);
    RUN_TEST(generic_loader);
    RUN_TEST(ply_triangles);
    RUN_TEST(ply_quads);
    RUN_TEST(ply_mixed);
    RUN_TEST(error_nonexistent_file);
    RUN_TEST(error_malformed_off);
    RUN_TEST(error_empty_off);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}
