# Instant Meshes Toolkit

[![License](https://img.shields.io/badge/license-BSD-blue.svg)](LICENSE.txt)

Instant Meshes Toolkit is a professional-grade geometry processing suite based on the algorithm "Instant Field-Aligned Meshes" (SIGGRAPH Asia 2015). It provides a robust, high-level API and a set of command-line utilities for high-quality field-aligned remeshing.

## Features

- **Field-Aligned Remeshing**: Generate high-quality quad or triangle meshes from any input.
- **Smart Defaults**: Automatically detects input topology and density to preserve detail without manual tuning.
- **Robust Format Support**: Native support for **PLY, OBJ, STL (Binary), and OFF**.
- **Vertex Welding**: High-performance welding logic to fix "triangle soup" artifacts in STL files.
- **Professional CLI**: Modern command-line interface with flags, help messages, and batch processing.
- **Python Bindings**: Extremely fast and lightweight bindings using **nanobind**.
- **Progress Reporting**: Real-time visual feedback for all long-running operations.

## Command Line Utilities

The toolkit provides three primary applications in the `build/` directory:

### `tri2quads`
Convert any triangle mesh into a quad-dominant mesh.
```bash
./tri2quads input.obj -o output.ply --faces 5000 --align-boundaries
```

### `remesh`
Remesh an input while preserving its original topology (Tri/Quad) and density.
```bash
./remesh input.stl --crease 45.0 --smooth 3
```

### `quad2tris`
Convert a quad mesh into a high-quality triangle mesh.
```bash
./quad2tris input.obj
```

### `mesh-clean`
Sanitize dirty meshes by removing non-manifold elements.
```bash
./mesh-clean complex_scan.stl
```

**Batch Processing**: All utilities support multiple input files and output directories.
```bash
./tri2quads assets/*.obj --out-dir processed/
```

## Python API

The `instant_meshes_python` module allows you to integrate the toolkit into your Python pipelines with just a few lines of code.

```python
import instant_meshes_python as im

# Initialize and load
mesh = im.InstantMesh()
mesh.load("bunny.obj")

# Optional: Set a progress callback
mesh.set_progress_callback(lambda msg, p: print(f"{msg}: {p*100:.1f}%"))

# High-level remeshing
mesh.create_quadmesh(target_faces=10000)

# Save the result
mesh.save("bunny_quads.ply")
```

## Installation

### Prerequisites
- CMake 3.12 or newer
- A C++20 compliant compiler
- Python 3.8+ (for bindings)

### Build Instructions
```bash
mkdir build && cd build
cmake ..
make -j
```
Dependencies like `nanobind` and `argparse` are automatically fetched by CMake.

## Technical Details

The toolkit is built on a high-resolution field optimization core. It optimizes an orientation field followed by a position field to guide the extraction of elements. By consolidating the paper's complex configuration into high-level C++ methods, the toolkit makes state-of-the-art remeshing accessible to production environments.

## License
The core implementation is based on the original work by Wenzel Jakob et al. and is licensed under the BSD license.
