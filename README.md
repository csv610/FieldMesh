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

## Technical Details: How it Works

The algorithm behind this toolkit, **Instant Field-Aligned Meshes**, reimagines remeshing as a global optimization problem over a point-sampled surface. The process consists of three major stages:

1.  **Orientation Field Optimization**: The tool first computes a sparse "cross-field" (for quads) or "6-way field" (for triangles) across the mesh. This field defines the local directions in which the new edges should align. It uses a **multi-resolution hierarchy** to solve this globally, ensuring the field remains smooth and coherent even on complex, noisy geometries.

2.  **Position Field Optimization**: Once the orientations are fixed, the tool optimizes for vertex placement. It computes a smooth parameterization that maps the 3D surface into a grid-like coordinate system. This step ensures that the resulting elements have uniform size and optimal aspect ratios.

3.  **Mesh Extraction**: Finally, the tool extracts the new topology by tracing the streamlines of the optimized fields. For quads, it identifies singularities (where the field "turns") and builds a clean graph of quadrilaterals. For triangles, it performs a high-quality Delaunay-style triangulation guided by the local field.

By decoupling the problem into field optimization and extraction, the algorithm can produce extremely high-quality results "instantly" compared to traditional remeshing techniques, while maintaining strict adherence to the underlying surface features and boundaries.

## License
The core implementation is based on the original work by Wenzel Jakob et al. and is licensed under the BSD license.
