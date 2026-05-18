#!/bin/bash
# Test runner script for Instant Meshes

set -e

cd "$(dirname "$0")/build"

echo "========================================"
echo "Running Instant Meshes Test Suite"
echo "========================================"

# Create tests directory in build if it doesn't exist
mkdir -p tests

# Build tests
echo ""
echo "Building tests..."
cmake .. -DBUILD_TESTS=ON
make -j$(nproc 2>/dev/null || echo 4)

# Run each test
echo ""
echo "========================================"
echo "Unit Tests: AABB and Ray"
echo "========================================"
./tests/test_aabb

echo ""
echo "========================================"
echo "Unit Tests: Adjacency Matrix"
echo "========================================"
./tests/test_adjacency

echo ""
echo "========================================"
echo "Unit Tests: Serializer"
echo "========================================"
./tests/test_serializer

echo ""
echo "========================================"
echo "Functional Tests: Mesh Operations"
echo "========================================"
./tests/test_mesh_ops

echo ""
echo "========================================"
echo "Integration Tests: Full Pipeline"
echo "========================================"
./tests/test_pipeline

echo ""
echo "========================================"
echo "All tests completed!"
echo "========================================"