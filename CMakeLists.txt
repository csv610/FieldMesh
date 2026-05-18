cmake_minimum_required(VERSION 3.12)

# Silence CMake warnings
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

# Fix compatibility with older dependencies (like nanogui) on modern CMake
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Silence CMake policy warnings from dependencies
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()
if (POLICY CMP0175)
  cmake_policy(SET CMP0175 NEW)
endif()
if (POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD) # Allow FetchContent_Populate
endif()

project(InstantMeshes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Options
option(INSTANT_MESHES_DEV_MODE "Instant meshes developer mode" OFF)

# Silence OpenGL deprecation warnings on macOS
if(APPLE)
  add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# Find dependencies
find_package(Eigen3 REQUIRED)
find_package(TBB REQUIRED)

# Build dependencies
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
if (EIGEN3_INCLUDE_DIR)
    set(NANOGUI_EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR} CACHE STRING " " FORCE)
else()
    get_target_property(EIGEN3_INC Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    set(NANOGUI_EIGEN_INCLUDE_DIR ${EIGEN3_INC} CACHE STRING " " FORCE)
endif()

include(FetchContent)

# Function to patch nanogui for modern CMake
function(patch_nanogui)
  if(nanogui_SOURCE_DIR)
    # Patch bin2c.cmake
    if(EXISTS "${nanogui_SOURCE_DIR}/resources/bin2c.cmake")
      file(READ "${nanogui_SOURCE_DIR}/resources/bin2c.cmake" BIN2C_CONTENT)
      string(REPLACE "cmake_minimum_required (VERSION 2.8.12)" "cmake_minimum_required(VERSION 3.10)" BIN2C_CONTENT "${BIN2C_CONTENT}")
      file(WRITE "${nanogui_SOURCE_DIR}/resources/bin2c.cmake" "${BIN2C_CONTENT}")
    endif()
    # Patch CMakeLists.txt (PRE_BUILD issue)
    if(EXISTS "${nanogui_SOURCE_DIR}/CMakeLists.txt")
      file(READ "${nanogui_SOURCE_DIR}/CMakeLists.txt" NANOGUI_CML)
      string(REPLACE "PRE_BUILD VERBATIM)" "VERBATIM)" NANOGUI_CML "${NANOGUI_CML}")
      file(WRITE "${nanogui_SOURCE_DIR}/CMakeLists.txt" "${NANOGUI_CML}")
    endif()
    # Patch for missing <cassert> in C++20
    file(GLOB NANOGUI_CPP_FILES "${nanogui_SOURCE_DIR}/src/*.cpp")
    foreach(CPP_FILE ${NANOGUI_CPP_FILES})
      file(READ ${CPP_FILE} CPP_CONTENT)
      if (NOT CPP_CONTENT MATCHES "#include <cassert>")
        file(WRITE ${CPP_FILE} "#include <cassert>\n${CPP_CONTENT}")
      endif()
    endforeach()
  endif()
endfunction()

FetchContent_Declare(
  nanogui
  GIT_REPOSITORY https://github.com/wjakob/nanogui.git
  GIT_TAG        master
  GIT_SUBMODULES_RECURSE YES
)

FetchContent_Declare(
  pss
  GIT_REPOSITORY https://github.com/wjakob/pss.git
  GIT_TAG        master
)

FetchContent_Declare(
  pcg32
  GIT_REPOSITORY https://github.com/wjakob/pcg32.git
  GIT_TAG        master
)

FetchContent_Declare(
  dset
  GIT_REPOSITORY https://github.com/wjakob/dset.git
  GIT_TAG        master
)

# Populate and patch nanogui before making it available
FetchContent_GetProperties(nanogui)
if(NOT nanogui_POPULATED)
  FetchContent_Populate(nanogui)
  patch_nanogui()
  add_subdirectory(${nanogui_SOURCE_DIR} ${nanogui_BINARY_DIR})
endif()

FetchContent_MakeAvailable(pss pcg32 dset)

# IDE Folders
if(CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Platform-specific compiler settings
if(MSVC)
  add_compile_definitions("_CRT_SECURE_NO_WARNINGS" "__TBB_NO_IMPLICIT_LINKAGE")
  add_compile_options("/MP")
  if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_options("/arch:SSE2")
    add_compile_definitions("EIGEN_DONT_ALIGN")
  endif()
  # Static runtime for MSVC
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (TARGET nanogui)
  set_property(TARGET nanogui PROPERTY FOLDER "dependencies")
endif()
if (TARGET glfw)
  set_property(TARGET glfw PROPERTY FOLDER "dependencies")
endif()

# Resource generation
set(PNG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/comb.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/delete_stroke.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/loadmesh.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/scare.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/sing_dir.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/sing_pos.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/instantmeshes.png)
file(GLOB FRAG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.frag")
file(GLOB VERT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.vert")
file(GLOB GEO_FILES  "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.geo")
set(IM_RESOURCES ${PNG_FILES} ${FRAG_FILES} ${VERT_FILES} ${GEO_FILES})

string(REPLACE ";" "," IM_RESOURCES_COMMAS "${IM_RESOURCES}")

add_custom_command(
  OUTPUT resources.cpp resources.h
  COMMAND ${CMAKE_COMMAND}
          -DOUTPUT_C=resources.cpp
          -DOUTPUT_H=resources.h
          "-DINPUT_FILES=${IM_RESOURCES_COMMAS}"
          -P "${nanogui_SOURCE_DIR}/resources/bin2c.cmake"
  DEPENDS ${IM_RESOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running bin2c"
  VERBATIM)

# Python Bindings
option(INSTANT_MESHES_BUILD_PYTHON "Build Python bindings" ON)
if (INSTANT_MESHES_BUILD_PYTHON)
  find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)
  include(FetchContent)
  FetchContent_Declare(
    nanobind
    GIT_REPOSITORY https://github.com/wjakob/nanobind.git
    GIT_TAG        v1.9.2
  )
  FetchContent_MakeAvailable(nanobind)

  nanobind_add_module(instant_meshes_python python/bindings.cpp)
  target_link_libraries(instant_meshes_python PRIVATE imesh_core)
  target_include_directories(instant_meshes_python SYSTEM PRIVATE src external)
  target_include_directories(instant_meshes_python SYSTEM PRIVATE
    ${nanobind_SOURCE_DIR}/include
    ${Python_INCLUDE_DIRS}
  )
endif()

# Executable sources
set(CORE_SOURCES
  src/common.h
  src/aabb.h
  src/meshio.h src/meshio.cpp
  src/normal.h src/normal.cpp
  src/adjacency.h src/adjacency.cpp
  src/meshstats.h src/meshstats.cpp
  src/hierarchy.h src/hierarchy.cpp
  src/extract.h src/extract.cpp
  src/field.h src/field.cpp
  src/bvh.h src/bvh.cpp
  src/subdivide.h src/subdivide.cpp
  src/reorder.h src/reorder.cpp
  src/serializer.h src/serializer.cpp
  src/instant_mesh.h src/instant_mesh.cpp
  src/smoothcurve.h src/smoothcurve.cpp
  src/cleanup.h src/cleanup.cpp
  src/dedge.h src/dedge.cpp
  external/rply/rply.c
)

add_library(imesh_core STATIC ${CORE_SOURCES})
target_include_directories(imesh_core SYSTEM PUBLIC
  ${nanogui_SOURCE_DIR}/include
  ${NANOGUI_EXTRA_INCS}
  ${EIGEN3_INCLUDE_DIR}
  ${TBB_INCLUDE_DIRS}
)
target_include_directories(imesh_core PUBLIC
  ${dset_SOURCE_DIR}
  ${pss_SOURCE_DIR}
  ${pcg32_SOURCE_DIR}
  external/rply
  external/half
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(imesh_core PUBLIC TBB::tbb Eigen3::Eigen)

set(EXTRA_SOURCE "")
if(APPLE)
  list(APPEND EXTRA_SOURCE "resources/im.icns")
elseif(WIN32)
  list(APPEND EXTRA_SOURCE "resources/im.rc")
endif()

# GUI executable
add_executable(imesh_vis MACOSX_BUNDLE
  src/imesh_vis.cpp
  resources.cpp
  src/glutil.h src/glutil.cpp
  src/widgets.h src/widgets.cpp
  src/viewer.h src/viewer.cpp
  ${EXTRA_SOURCE}
)
target_link_libraries(imesh_vis PRIVATE imesh_core nanogui ${NANOGUI_EXTRA_LIBS})
target_compile_definitions(imesh_vis PRIVATE ${NANOGUI_EXTRA_DEFS})

# CLI executable
add_executable(imesh_cli
  src/imesh_cli.cpp
)
target_link_libraries(imesh_cli PRIVATE imesh_core)

# New Utilities
add_executable(tri2quads apps/tri2quads.cpp)
target_link_libraries(tri2quads PRIVATE imesh_core)
target_include_directories(tri2quads PRIVATE src external)

add_executable(remesh apps/remesh.cpp)
target_link_libraries(remesh PRIVATE imesh_core)
target_include_directories(remesh PRIVATE src external)

add_executable(quad2tris apps/quad2tris.cpp)
target_link_libraries(quad2tris PRIVATE imesh_core)
target_include_directories(quad2tris PRIVATE src external)

add_executable(mesh_clean apps/mesh-clean.cpp)
target_link_libraries(mesh_clean PRIVATE imesh_core)
target_include_directories(mesh_clean PRIVATE src external)

# Shared configurations
foreach(target imesh_core imesh_vis imesh_cli tri2quads remesh quad2tris mesh_clean)
  if(INSTANT_MESHES_DEV_MODE)
    target_compile_definitions(${target} PRIVATE DEV_MODE)
  endif()

  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-literal-operator)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_compile_options(${target} PRIVATE -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types)
    endif()
  endif()
endforeach()

if(MSVC)
  set_source_files_properties(external/rply/rply.c PROPERTIES COMPILE_FLAGS "/wd4127")
endif()

# Bundle properties
set_target_properties(imesh_vis PROPERTIES
  OUTPUT_NAME "imesh_vis"
  MACOSX_BUNDLE_BUNDLE_NAME "imesh_vis"
  MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.im.imesh_vis"
  MACOSX_BUNDLE_ICON_FILE im.icns
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/MacOSXBundleInfo.plist.in
)

if(APPLE)
  set_source_files_properties(resources/im.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
elseif(NOT WIN32)
  # Insulate from ABI changes on Linux
  target_link_options(imesh_vis PRIVATE "-static-libgcc")
endif()

# Post-build strip
if(UNIX AND NOT CMAKE_BUILD_TYPE MATCHES Debug)
  add_custom_command(TARGET imesh_vis POST_BUILD COMMAND strip $<TARGET_FILE:imesh_vis>)
endif()

# Tests
option(BUILD_TESTS "Build test suite" ON)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

# Final pass to suppress warnings in all third-party targets
if (NOT MSVC)
  set(THIRD_PARTY_WARNING_FLAGS 
    -Wno-deprecated-declarations 
    -Wno-deprecated-literal-operator 
    -Wno-objc-multiple-method-names
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-but-set-variable
  )
  foreach(target glfw glfw_objects nanogui nanogui-obj nanobind nanobind-static nanobind-obj)
    if (TARGET ${target})
      target_compile_options(${target} PRIVATE ${THIRD_PARTY_WARNING_FLAGS})
    endif()
  endforeach()
endif()
