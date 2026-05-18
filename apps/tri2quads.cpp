#include "instant_mesh.h"
#include <argparse/argparse.hpp>
#include <iostream>
#include <filesystem>
#include <vector>

void show_progress(const std::string &status, Float progress) {
    if (progress < 0) return;
    int barWidth = 40;
    std::cout << "\r" << status << " [";
    int pos = (int)(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "%";
    std::cout.flush();
    if (progress >= 1.0f) std::cout << std::endl;
}

int main(int argc, char **argv) {
    argparse::ArgumentParser program("tri2quads");

    program.add_argument("input")
        .help("input triangle mesh file(s)")
        .nargs(argparse::nargs_pattern::at_least_one);

    program.add_argument("-o", "--out-dir")
        .help("output directory (default: same as input)");

    program.add_argument("-f", "--faces")
        .help("target face count")
        .scan<'d', int>();

    program.add_argument("-b", "--align-boundaries")
        .help("align to boundaries")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-c", "--crease")
        .help("crease angle")
        .scan<'f', float>();

    program.add_argument("-s", "--smooth")
        .help("smoothing iterations")
        .scan<'d', int>();

    program.add_argument("-e", "--extrinsic")
        .help("use extrinsic optimization")
        .default_value(false)
        .implicit_value(true);

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
            output_path = (std::filesystem::path(out_dir) / (p.stem().string() + "_quad.ply")).string();
        } else {
            output_path = (std::filesystem::current_path() / (p.stem().string() + "_quad.ply")).string();
        }

        std::cout << "Processing: " << input_path << " -> " << output_path << std::endl;

        try {
            InstantMesh im;
            im.setProgressCallback(show_progress);
            im.load(input_path);
            
            if (program.get<bool>("--align-boundaries")) im.setAlignToBoundaries(true);
            if (auto c = program.present<float>("--crease")) im.setCreaseAngle(*c);
            if (auto s = program.present<int>("--smooth")) im.setSmoothIter(*s);
            if (program.get<bool>("--extrinsic")) im.setExtrinsic(true);

            im.createQuadmesh(program.present<int>("-f") ? program.get<int>("-f") : -1);
            
            im.save(output_path);
            std::cout << "Successfully saved: " << output_path << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error processing " << input_path << ": " << e.what() << std::endl;
        }
    }

    return 0;
}
