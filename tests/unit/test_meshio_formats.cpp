#include "test_framework.h"
#include "meshio.h"
#include <filesystem>

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

int main() {
    std::cout << "=== MeshIO Format Tests ===" << std::endl;
    int passed = 0, failed = 0;

    RUN_TEST(off_format);
    RUN_TEST(stl_format_welding);
    RUN_TEST(generic_loader);

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed;
}
