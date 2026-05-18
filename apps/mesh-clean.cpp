#include "instant_mesh.h"
#include "cleanup.h"
#include "meshio.h"
#include <argparse/argparse.hpp>
#include <iostream>
#include <filesystem>
#include <vector>

int main(int argc, char **argv) {
    argparse::ArgumentParser program("mesh-clean");

    program.add_argument("input")
        .help("input mesh file(s)")
        .nargs(argparse::nargs_pattern::at_least_one);

    program.add_argument("-o", "--out-dir")
        .help("output directory (default: same as input)");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto inputs = program.get<std::vector<std::string>>("input");
    std::string out_dir = program.present("-o") ? program.get<std::string>("-o") : "";

    for (const auto& input_path : inputs) {
        std::filesystem::path p(input_path);
        std::string output_path;
        if (!out_dir.empty()) {
            output_path = (std::filesystem::path(out_dir) / (p.stem().string() + "_clean" + p.extension().string())).string();
        } else {
            output_path = (p.parent_path() / (p.stem().string() + "_clean" + p.extension().string())).string();
        }

        std::cout << "Cleaning: " << input_path << " -> " << output_path << std::endl;

        try {
            MatrixXu F;
            MatrixXf V, N;
            load_mesh_or_pointcloud(input_path, F, V, N);

            MatrixXf Nf;
            remove_nonmanifold(F, V, Nf);

            write_mesh(output_path, F, V, MatrixXf(), Nf);
            std::cout << "Successfully saved cleaned mesh: " << output_path << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error cleaning " << input_path << ": " << e.what() << std::endl;
        }
    }

    return 0;
}
