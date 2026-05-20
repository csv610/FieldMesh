/*
    imesh_cli.cpp -- Command line interface to Instant Meshes

    This file is part of the implementation of

        Instant Field-Aligned Meshes
        Wenzel Jakob, Daniele Panozzo, Marco Tarini, and Olga Sorkine-Hornung
        In ACM Transactions on Graphics (Proc. SIGGRAPH Asia 2015)

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include "instant_mesh.h"
#include <thread>
#include <cstdlib>
#include <tbb/global_control.h>

extern int nprocs;

int main(int argc, char **argv) {
    std::vector<std::string> args;
    bool extrinsic = true, dominant = false, align_to_boundaries = false;
    bool help = false, deterministic = false;
    int rosy = 4, posy = 4, face_count = -1, vertex_count = -1;
    uint32_t knn_points = 10, smooth_iter = 2;
    Float crease_angle = -1, scale = -1;
    std::string batchOutput;

    try {
        for (int i=1; i<argc; ++i) {
            if (strcmp("--help", argv[i]) == 0 || strcmp("-h", argv[i]) == 0) {
                help = true;
            } else if (strcmp("--deterministic", argv[i]) == 0 || strcmp("-d", argv[i]) == 0) {
                deterministic = true;
            } else if (strcmp("--intrinsic", argv[i]) == 0 || strcmp("-i", argv[i]) == 0) {
                extrinsic = false;
            } else if (strcmp("--boundaries", argv[i]) == 0 || strcmp("-b", argv[i]) == 0) {
                align_to_boundaries = true;
            } else if (strcmp("--threads", argv[i]) == 0 || strcmp("-t", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing thread count!" << std::endl;
                    return -1;
                }
                nprocs = str_to_uint32_t(argv[i]);
            } else if (strcmp("--smooth", argv[i]) == 0 || strcmp("-S", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing smoothing iteration count argument!" << std::endl;
                    return -1;
                }
                smooth_iter = str_to_uint32_t(argv[i]);
            } else if (strcmp("--knn", argv[i]) == 0 || strcmp("-k", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing knn point count argument!" << std::endl;
                    return -1;
                }
                knn_points = str_to_uint32_t(argv[i]);
            } else if (strcmp("--crease", argv[i]) == 0 || strcmp("-c", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing crease angle argument!" << std::endl;
                    return -1;
                }
                crease_angle = str_to_float(argv[i]);
            } else if (strcmp("--rosy", argv[i]) == 0 || strcmp("-r", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing rotation symmetry type!" << std::endl;
                    return -1;
                }
                rosy = str_to_int32_t(argv[i]);
            } else if (strcmp("--posy", argv[i]) == 0 || strcmp("-p", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing position symmetry type!" << std::endl;
                    return -1;
                }
                posy = str_to_int32_t(argv[i]);
                if (posy == 6)
                    posy = 3;
            } else if (strcmp("--scale", argv[i]) == 0 || strcmp("-s", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing scale argument!" << std::endl;
                    return -1;
                }
                scale = str_to_float(argv[i]);
            } else if (strcmp("--faces", argv[i]) == 0 || strcmp("-f", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing face count argument!" << std::endl;
                    return -1;
                }
                face_count = str_to_int32_t(argv[i]);
            } else if (strcmp("--vertices", argv[i]) == 0 || strcmp("-v", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing vertex count argument!" << std::endl;
                    return -1;
                }
                vertex_count = str_to_int32_t(argv[i]);
            } else if (strcmp("--output", argv[i]) == 0 || strcmp("-o", argv[i]) == 0) {
                if (++i >= argc) {
                    std::cerr << "Missing batch mode output file argument!" << std::endl;
                    return -1;
                }
                batchOutput = argv[i];
            } else if (strcmp("--dominant", argv[i]) == 0 || strcmp("-D", argv[i]) == 0) {
                dominant = true;
            } else {
                if (strncmp(argv[i], "-", 1) == 0) {
                    std::cerr << "Invalid argument: \"" << argv[i] << "\"!" << std::endl;
                    help = true;
                }
                args.push_back(argv[i]);
            }
        }
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        help = true;
    }

    if ((posy != 3 && posy != 4) || (rosy != 2 && rosy != 4 && rosy != 6)) {
        std::cerr << "Error: Invalid symmetry type!" << std::endl;
        help  = true;
    }

    int nConstraints = 0;
    nConstraints += scale > 0 ? 1 : 0;
    nConstraints += face_count > 0 ? 1 : 0;
    nConstraints += vertex_count > 0 ? 1 : 0;

    if (nConstraints > 1) {
        std::cerr << "Error: Only one of the --scale, --face and --vertices parameters can be used at once!" << std::endl;
        help = true;
    }

    if (args.size() != 1 || batchOutput.empty() || help) {
        std::cout << "Syntax: " << argv[0] << " [options] -o <output> <input mesh / point cloud>" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "   -o, --output <output>     Writes to the specified PLY/OBJ output file (required)" << std::endl;
        std::cout << "   -t, --threads <count>     Number of threads used for parallel computations" << std::endl;
        std::cout << "   -d, --deterministic       Prefer (slower) deterministic algorithms" << std::endl;
        std::cout << "   -c, --crease <degrees>    Dihedral angle threshold for creases" << std::endl;
        std::cout << "   -S, --smooth <iter>       Number of smoothing & ray tracing reprojection steps (default: 2)" << std::endl;
        std::cout << "   -D, --dominant            Generate a tri/quad dominant mesh instead of a pure tri/quad mesh" << std::endl;
        std::cout << "   -i, --intrinsic           Intrinsic mode (extrinsic is the default)" << std::endl;
        std::cout << "   -b, --boundaries          Align to boundaries (only applies when the mesh is not closed)" << std::endl;
        std::cout << "   -r, --rosy <number>       Specifies the orientation symmetry type (2, 4, or 6)" << std::endl;
        std::cout << "   -p, --posy <number>       Specifies the position symmetry type (4 or 6)" << std::endl;
        std::cout << "   -s, --scale <scale>       Desired world space length of edges in the output" << std::endl;
        std::cout << "   -f, --faces <count>       Desired face count of the output mesh" << std::endl;
        std::cout << "   -v, --vertices <count>    Desired vertex count of the output mesh" << std::endl;
        std::cout << "   -k, --knn <count>         Point cloud mode: number of adjacent points to consider" << std::endl;
        std::cout << "   -h, --help                Display this message" << std::endl;
        return -1;
    }

    tbb::global_control control(tbb::global_control::max_allowed_parallelism,
                                nprocs == -1 ? std::thread::hardware_concurrency() : nprocs);

    try {
        InstantMesh im;
        im.load(args[0]);
        im.setRoSy(rosy);
        im.setPoSy(posy);
        im.setScale(scale);
        im.setFaceCount(face_count);
        im.setVertexCount(vertex_count);
        im.setCreaseAngle(crease_angle);
        im.setExtrinsic(extrinsic);
        im.setAlignToBoundaries(align_to_boundaries);
        im.setSmoothIter(smooth_iter);
        im.setKnnPoints(knn_points);
        im.setPureQuad(!dominant);
        im.setDeterministic(deterministic);

        im.process();
        im.save(batchOutput);
    } catch (const std::exception &e) {
        std::cerr << "Caught runtime error : " << e.what() << std::endl;
        return -1;
    }

    return EXIT_SUCCESS;
}
