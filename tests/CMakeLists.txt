cmake_minimum_required(VERSION 3.12)
project(InstantMeshesTests)

add_compile_options(-Wno-deprecated-literal-operator)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Unit Tests
add_executable(test_aabb unit/test_aabb.cpp)
target_link_libraries(test_aabb PRIVATE imesh_core)
add_test(NAME test_aabb COMMAND test_aabb)

add_executable(test_adjacency unit/test_adjacency.cpp)
target_link_libraries(test_adjacency PRIVATE imesh_core)
add_test(NAME test_adjacency COMMAND test_adjacency)

add_executable(test_meshio_formats unit/test_meshio_formats.cpp)
target_link_libraries(test_meshio_formats PRIVATE imesh_core)
add_test(NAME test_meshio_formats COMMAND test_meshio_formats)

add_executable(test_serializer unit/test_serializer.cpp)
target_link_libraries(test_serializer PRIVATE imesh_core)
add_test(NAME test_serializer COMMAND test_serializer)

# Functional Tests
add_executable(test_mesh_ops functional/test_mesh_ops.cpp)
target_link_libraries(test_mesh_ops PRIVATE imesh_core)
add_test(NAME test_mesh_ops COMMAND test_mesh_ops)

# Integration Tests  
add_executable(test_pipeline integration/test_pipeline.cpp)
target_link_libraries(test_pipeline PRIVATE imesh_core)
add_test(NAME test_pipeline COMMAND test_pipeline)
