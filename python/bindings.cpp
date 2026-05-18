#include <nanobind/nanobind.h>
#include <nanobind/eigen/dense.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/function.h>
#include "instant_mesh.h"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(instant_meshes_python, m) {
    nb::class_<InstantMesh>(m, "InstantMesh")
        .def(nb::init<>())
        .def("load", &InstantMesh::load, "filename"_a)
        .def("save", &InstantMesh::save, "filename"_a)
        .def("process", &InstantMesh::process)
        .def("create_quadmesh", &InstantMesh::createQuadmesh, "target_faces"_a = -1)
        .def("create_trimesh", &InstantMesh::createTrimesh, "target_faces"_a = -1)
        .def("remesh", &InstantMesh::remesh, "target_faces"_a = -1)
        .def("set_target_vertices", &InstantMesh::setTargetVertices)
        .def("set_target_faces", &InstantMesh::setTargetFaces)
        .def("set_rosy", &InstantMesh::setRoSy)
        .def("set_posy", &InstantMesh::setPoSy)
        .def("set_scale", &InstantMesh::setScale)
        .def("set_crease_angle", &InstantMesh::setCreaseAngle)
        .def("set_extrinsic", &InstantMesh::setExtrinsic)
        .def("set_align_to_boundaries", &InstantMesh::setAlignToBoundaries)
        .def("set_smooth_iter", &InstantMesh::setSmoothIter)
        .def("set_knn_points", &InstantMesh::setKnnPoints)
        .def("set_pure_quad", &InstantMesh::setPureQuad)
        .def("set_deterministic", &InstantMesh::setDeterministic)
        .def("set_progress_callback", &InstantMesh::setProgressCallback, "callback"_a)
        .def("input_face_count", &InstantMesh::inputFaceCount)
        .def("input_vertex_count", &InstantMesh::inputVertexCount)
        .def("input_vertices_per_face", &InstantMesh::inputVerticesPerFace)
        .def("vertices", &InstantMesh::vertices)
        .def("faces", &InstantMesh::faces)
        .def("normals", &InstantMesh::normals);
}
