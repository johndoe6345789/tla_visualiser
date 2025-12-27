#!/bin/bash
# Build script for TLA+ Visualiser

set -e  # Exit on error

echo "==================================="
echo "TLA+ Visualiser Build Script"
echo "==================================="
echo

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check for required tools
echo "Checking prerequisites..."

command -v cmake >/dev/null 2>&1 || { echo -e "${RED}Error: cmake not found${NC}"; exit 1; }
command -v ninja >/dev/null 2>&1 || { echo -e "${RED}Error: ninja not found${NC}"; exit 1; }
command -v conan >/dev/null 2>&1 || { echo -e "${RED}Error: conan not found. Install with: pip install conan${NC}"; exit 1; }

echo -e "${GREEN}✓ All required tools found${NC}"
echo

# Parse arguments
BUILD_TYPE="Release"
CLEAN=false
RUN_TESTS=true

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --no-tests)
            RUN_TESTS=false
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo
            echo "Options:"
            echo "  --debug      Build in Debug mode (default: Release)"
            echo "  --clean      Clean build directory before building"
            echo "  --no-tests   Skip running tests"
            echo "  --help       Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf build
    echo -e "${GREEN}✓ Clean complete${NC}"
    echo
fi

# Detect Conan profile
if [ ! -d "$HOME/.conan2" ] && [ ! -d "$HOME/.conan" ]; then
    echo "Detecting Conan profile..."
    conan profile detect
    echo -e "${GREEN}✓ Conan profile detected${NC}"
    echo
fi

# Install dependencies
echo "Installing dependencies with Conan..."
conan install . --output-folder=build --build=missing
echo -e "${GREEN}✓ Dependencies installed${NC}"
echo

# Configure CMake
echo "Configuring CMake ($BUILD_TYPE)..."
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
echo -e "${GREEN}✓ Configuration complete${NC}"
echo

# Build
echo "Building project..."
cmake --build build --config $BUILD_TYPE
echo -e "${GREEN}✓ Build complete${NC}"
echo

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo "Running tests..."
    cd build
    if ctest --output-on-failure; then
        echo -e "${GREEN}✓ All tests passed${NC}"
    else
        echo -e "${RED}✗ Some tests failed${NC}"
        exit 1
    fi
    cd ..
    echo
fi

# Success message
echo "==================================="
echo -e "${GREEN}Build successful!${NC}"
echo "==================================="
echo
echo "To run the application:"
echo "  ./build/tla_visualiser"
echo
echo "Build artifacts are in: build/"
echo "Executable: build/tla_visualiser"
