#!/bin/bash
# Workflow Simulation Script for TLA+ Visualiser
# This script simulates the gated tree workflow locally for testing

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
RUN_LINT=${RUN_LINT:-true}
RUN_BUILD=${RUN_BUILD:-true}
RUN_TESTS=${RUN_TESTS:-true}
BUILD_TYPE=${BUILD_TYPE:-Release}
PLATFORM=$(uname -s)

# File patterns for checking
FILE_PATTERNS="*.cpp *.h *.qml"

# Job results tracking
LINT_RESULT="pending"
BUILD_RESULT="pending"
TEST_RESULT="pending"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}TLA+ Visualiser Workflow Simulation${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Platform: $PLATFORM"
echo "Build Type: $BUILD_TYPE"
echo ""

# Function to print job header
print_job_header() {
    echo -e "\n${BLUE}>>> Job: $1${NC}"
    echo "----------------------------------------"
}

# Function to print success
print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

# Function to print error
print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Function to print warning
print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

# Job 1: Lint and Code Quality
run_lint_job() {
    print_job_header "Lint and Code Quality"
    
    if [ "$RUN_LINT" != "true" ]; then
        print_warning "Lint job skipped"
        LINT_RESULT="skipped"
        return 0
    fi
    
    # Check for trailing whitespace
    echo "Checking for trailing whitespace..."
    if git grep -I --line-number --perl-regexp '\s+$' -- $FILE_PATTERNS 2>/dev/null; then
        print_warning "Found trailing whitespace (not failing - should be fixed eventually)"
    else
        print_success "No trailing whitespace found"
    fi
    
    # Check for TODO/FIXME without issue reference
    echo "Checking for untracked TODOs..."
    if git grep -n "TODO\|FIXME" -- $FILE_PATTERNS 2>/dev/null | grep -v "#[0-9]"; then
        print_warning "Found TODOs without issue references (not failing)"
    fi
    
    # Verify CMake exists
    echo "Checking CMake..."
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found"
        LINT_RESULT="failed"
        return 1
    fi
    print_success "CMake found: $(cmake --version | head -n1)"
    
    LINT_RESULT="success"
    print_success "Lint job completed"
    return 0
}

# Job 2: Build
run_build_job() {
    print_job_header "Build"
    
    if [ "$RUN_BUILD" != "true" ]; then
        print_warning "Build job skipped"
        BUILD_RESULT="skipped"
        return 0
    fi
    
    # Check if lint passed
    if [ "$LINT_RESULT" != "success" ]; then
        print_error "Cannot build: Lint job did not pass"
        BUILD_RESULT="failed"
        return 1
    fi
    
    # Install dependencies (basic check)
    echo "Checking build dependencies..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found"
        BUILD_RESULT="failed"
        return 1
    fi
    
    if ! command -v conan &> /dev/null; then
        print_warning "Conan not found - attempting to continue"
    fi
    
    # Configure and build
    echo "Configuring build..."
    if [ ! -d "build" ]; then
        mkdir -p build
    fi
    
    # Install Conan dependencies if conan is available
    if command -v conan &> /dev/null; then
        echo "Installing Conan dependencies..."
        if ! conan install . --output-folder=build --build=missing; then
            print_warning "Conan install failed - build may fail due to missing dependencies"
            print_warning "Consider installing Conan: pip install conan"
        fi
    else
        print_warning "Conan not found - build may fail due to missing dependencies"
        print_warning "Install with: pip install conan"
    fi
    
    # Configure CMake
    echo "Running CMake configure..."
    CMAKE_ARGS="-B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    
    if [ -f "build/conan_toolchain.cmake" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake"
    fi
    
    if command -v ninja &> /dev/null; then
        CMAKE_ARGS="$CMAKE_ARGS -G Ninja"
    fi
    
    if ! cmake $CMAKE_ARGS; then
        print_error "CMake configuration failed"
        BUILD_RESULT="failed"
        return 1
    fi
    print_success "CMake configuration succeeded"
    
    # Build
    echo "Building project..."
    if ! cmake --build build --config ${BUILD_TYPE}; then
        print_error "Build failed"
        BUILD_RESULT="failed"
        return 1
    fi
    
    BUILD_RESULT="success"
    print_success "Build completed successfully"
    return 0
}

# Job 3: Test
run_test_job() {
    print_job_header "Test"
    
    if [ "$RUN_TESTS" != "true" ]; then
        print_warning "Test job skipped"
        TEST_RESULT="skipped"
        return 0
    fi
    
    # Check if build passed
    if [ "$BUILD_RESULT" != "success" ]; then
        print_error "Cannot test: Build job did not pass"
        TEST_RESULT="failed"
        return 1
    fi
    
    # Run tests
    echo "Running tests..."
    if [ -d "build" ]; then
        cd build
        if ! ctest --output-on-failure; then
            cd ..
            print_error "Tests failed"
            TEST_RESULT="failed"
            return 1
        fi
        cd ..
    else
        print_error "Build directory not found"
        TEST_RESULT="failed"
        return 1
    fi
    
    TEST_RESULT="success"
    print_success "All tests passed"
    return 0
}

# Gate: Check all results
run_gate_check() {
    print_job_header "Gated Tree Check"
    
    echo "Job Results:"
    echo "  Lint:  $LINT_RESULT"
    echo "  Build: $BUILD_RESULT"
    echo "  Test:  $TEST_RESULT"
    echo ""
    
    local gate_passed=true
    
    # Check lint
    if [ "$LINT_RESULT" != "success" ] && [ "$LINT_RESULT" != "skipped" ]; then
        print_error "Lint check failed"
        gate_passed=false
    fi
    
    # Check build
    if [ "$BUILD_RESULT" != "success" ] && [ "$BUILD_RESULT" != "skipped" ]; then
        print_error "Build check failed"
        gate_passed=false
    fi
    
    # Check test
    if [ "$TEST_RESULT" != "success" ] && [ "$TEST_RESULT" != "skipped" ]; then
        print_error "Test check failed"
        gate_passed=false
    fi
    
    # Ensure at least something ran
    if [ "$LINT_RESULT" == "skipped" ] && [ "$BUILD_RESULT" == "skipped" ] && [ "$TEST_RESULT" == "skipped" ]; then
        print_error "No jobs were executed"
        gate_passed=false
    fi
    
    echo ""
    if [ "$gate_passed" == true ]; then
        print_success "All required checks passed - gate is OPEN ✅"
        echo ""
        echo -e "${GREEN}========================================${NC}"
        echo -e "${GREEN}   WORKFLOW SIMULATION SUCCESSFUL   ${NC}"
        echo -e "${GREEN}========================================${NC}"
        return 0
    else
        print_error "One or more checks failed - gate is CLOSED ❌"
        echo ""
        echo -e "${RED}========================================${NC}"
        echo -e "${RED}     WORKFLOW SIMULATION FAILED     ${NC}"
        echo -e "${RED}========================================${NC}"
        return 1
    fi
}

# Main execution
main() {
    # Parse command line arguments
    for arg in "$@"; do
        case $arg in
            --no-lint)
                RUN_LINT=false
                ;;
            --no-build)
                RUN_BUILD=false
                ;;
            --no-tests)
                RUN_TESTS=false
                ;;
            --debug)
                BUILD_TYPE=Debug
                ;;
            --help)
                echo "Usage: $0 [options]"
                echo ""
                echo "Options:"
                echo "  --no-lint     Skip lint checks"
                echo "  --no-build    Skip build"
                echo "  --no-tests    Skip tests"
                echo "  --debug       Build in Debug mode (default: Release)"
                echo "  --help        Show this help message"
                echo ""
                echo "Examples:"
                echo "  $0                    # Run full workflow"
                echo "  $0 --no-tests         # Build without tests"
                echo "  $0 --debug            # Debug build with tests"
                exit 0
                ;;
        esac
    done
    
    # Run workflow jobs
    run_lint_job || true
    run_build_job || true
    run_test_job || true
    
    # Gate check
    run_gate_check
    exit $?
}

# Run main
main "$@"
