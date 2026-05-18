cmake_minimum_required(VERSION 3.12)
project(InstantMeshesTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wno-deprecated-literal-operator)

find_package(Eigen3 REQUIRED)
find_package(TBB REQUIRED)

set(TEST_SOURCES_DIR ${CMAKE_SOURCE_DIR})

# CRITICAL: System Eigen must come first - do NOT include nanogui's old eigen
include_directories(
    ${EIGEN3_INCLUDE_DIR}
    ${TBB_INCLUDE_DIRS}
    ${TEST_SOURCES_DIR}/src
    ${TEST_SOURCES_DIR}/external/dset
    ${TEST_SOURCES_DIR}/external/pss
    ${TEST_SOURCES_DIR}/external/pcg32
    ${TEST_SOURCES_DIR}/external/rply
    ${TEST_SOURCES_DIR}/external/half
)

set(TEST_LIBS
    TBB::tbb
    Eigen3::Eigen
)

enable_testing()

# Unit Tests
add_executable(test_aabb ${TEST_SOURCES_DIR}/tests/unit/test_aabb.cpp)
target_link_libraries(test_aabb PRIVATE ${TEST_LIBS})
add_test(NAME test_aabb COMMAND test_aabb)

add_executable(test_adjacency ${TEST_SOURCES_DIR}/tests/unit/test_adjacency.cpp)
target_link_libraries(test_adjacency PRIVATE ${TEST_LIBS})
add_test(NAME test_adjacency COMMAND test_adjacency)

add_executable(test_meshio_formats 
    ${TEST_SOURCES_DIR}/tests/unit/test_meshio_formats.cpp
    ${TEST_SOURCES_DIR}/src/meshio.cpp
    ${TEST_SOURCES_DIR}/src/normal.cpp
    ${TEST_SOURCES_DIR}/external/rply/rply.c
)
target_link_libraries(test_meshio_formats PRIVATE ${TEST_LIBS})
target_include_directories(test_meshio_formats PRIVATE ${TEST_SOURCES_DIR}/tests)
add_test(NAME test_meshio_formats COMMAND test_meshio_formats)

add_executable(test_serializer 
    ${TEST_SOURCES_DIR}/tests/unit/test_serializer.cpp
    ${TEST_SOURCES_DIR}/src/serializer.cpp
    ${TEST_SOURCES_DIR}/external/rply/rply.c
)
target_link_libraries(test_serializer PRIVATE ${TEST_LIBS})
add_test(NAME test_serializer COMMAND test_serializer)

# Functional Tests
add_executable(test_mesh_ops 
    ${TEST_SOURCES_DIR}/tests/functional/test_mesh_ops.cpp
    ${TEST_SOURCES_DIR}/src/meshstats.cpp
    ${TEST_SOURCES_DIR}/src/meshio.cpp
    ${TEST_SOURCES_DIR}/src/adjacency.cpp
    ${TEST_SOURCES_DIR}/src/dedge.cpp
    ${TEST_SOURCES_DIR}/src/normal.cpp
    ${TEST_SOURCES_DIR}/src/bvh.cpp
    ${TEST_SOURCES_DIR}/external/rply/rply.c
)
target_link_libraries(test_mesh_ops PRIVATE ${TEST_LIBS})
add_test(NAME test_mesh_ops COMMAND test_mesh_ops)

# Integration Tests  
add_executable(test_pipeline
    ${TEST_SOURCES_DIR}/tests/integration/test_pipeline.cpp
    ${TEST_SOURCES_DIR}/src/meshstats.cpp
    ${TEST_SOURCES_DIR}/src/meshio.cpp
    ${TEST_SOURCES_DIR}/src/adjacency.cpp
    ${TEST_SOURCES_DIR}/src/dedge.cpp
    ${TEST_SOURCES_DIR}/src/normal.cpp
    ${TEST_SOURCES_DIR}/src/serializer.cpp
    ${TEST_SOURCES_DIR}/src/bvh.cpp
    ${TEST_SOURCES_DIR}/external/rply/rply.c
)
target_link_libraries(test_pipeline PRIVATE ${TEST_LIBS})
add_test(NAME test_pipeline COMMAND test_pipeline)