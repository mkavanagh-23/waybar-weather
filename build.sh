#!/bin/bash

# Build script for multi-configuration C++ project

# Function to print usage
usage() {
    echo "Usage: $0 [debug|release|all]"
    echo "  debug   - Build debug configuration"
    echo "  release - Build release configuration"
    echo "  all     - Build both debug and release configurations"
    exit 1
}

# Check for valid arguments
if [ $# -eq 0 ]; then
  usage
fi

# Determine build type
BUILD_TYPE="${1:-all}"

# Base CMake configuration
CMAKE_BASE_ARGS="-DCMAKE_CXX_STANDARD=20"

# Debug build function
build_debug() {
    echo "Building Debug Configuration..."
    cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug ${CMAKE_BASE_ARGS}
    cmake --build build/debug
}

# Release build function
build_release() {
    echo "Building Release Configuration..."
    cmake -B build/release -DCMAKE_BUILD_TYPE=Release ${CMAKE_BASE_ARGS}
    cmake --build build/release
}

# Handle build type
case "$BUILD_TYPE" in
    debug)
        build_debug
        cp build/debug/compile_commands.json build/
        ;;
    release)
        build_release
        cp build/release/compile_commands.json build/
        ;;
    all)
        build_debug
        build_release
        cp build/debug/compile_commands.json build/
        ;;
    *)
        usage
        ;;
esac

echo "Build complete."
