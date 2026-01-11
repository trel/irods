#!/bin/bash
#
# run_static_analysis.sh
#
# Run comprehensive static analysis on iRODS build
# Supports ASAN, UBSAN, and clang-tidy
#
# Usage:
#   ./run_static_analysis.sh [asan|ubsan|tidy|all]
#
# Examples:
#   ./run_static_analysis.sh asan      # Build with Address Sanitizer
#   ./run_static_analysis.sh ubsan     # Build with UB Sanitizer
#   ./run_static_analysis.sh tidy      # Run clang-tidy
#   ./run_static_analysis.sh all       # All checks
#

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"
BUILD_DIR="${PROJECT_ROOT}/build"
CORES=${CORES:-30}

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

run_asan() {
    log_info "Building with Address Sanitizer..."
    
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DIRODS_ENABLE_ADDRESS_SANITIZER=ON
    
    log_info "Compiling with ASAN..."
    cmake --build . -j "$CORES" 2>&1 | tee asan_build.log
    
    if [ $? -eq 0 ]; then
        log_info "ASAN build successful!"
        log_info "To run tests: cd $BUILD_DIR && ctest --verbose"
        log_info "ASAN output will be in: /tmp/irods_asan_output"
    else
        log_error "ASAN build failed!"
        return 1
    fi
}

run_ubsan() {
    log_info "Building with Undefined Behavior Sanitizer..."
    
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON
    
    log_info "Compiling with UBSAN..."
    cmake --build . -j "$CORES" 2>&1 | tee ubsan_build.log
    
    if [ $? -eq 0 ]; then
        log_info "UBSAN build successful!"
        log_info "To run tests: cd $BUILD_DIR && ctest --verbose"
        log_info "UBSAN output will be in: /tmp/irods_ubsan_output"
    else
        log_error "UBSAN build failed!"
        return 1
    fi
}

run_tidy() {
    log_info "Running clang-tidy..."
    
    if [ ! -d "$BUILD_DIR" ]; then
        log_warn "Build directory not found, generating CMake files..."
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake ..
    fi
    
    cd "$BUILD_DIR"
    
    # Check if run-clang-tidy is available
    if ! command -v run-clang-tidy &> /dev/null; then
        log_error "run-clang-tidy not found. Install with:"
        log_error "  sudo apt-get install clang-tools"
        return 1
    fi
    
    log_info "Analyzing rule language plugin..."
    run-clang-tidy \
        -p . \
        ../plugins/rule_engines/irods_rule_language/ \
        -header-filter='irods/private/re/.*' \
        2>&1 | tee tidy_report.txt
    
    log_info "clang-tidy report saved to: $BUILD_DIR/tidy_report.txt"
}

run_all() {
    log_info "Running all static analysis checks..."
    
    run_asan || log_error "ASAN check failed"
    log_info ""
    
    run_ubsan || log_error "UBSAN check failed"
    log_info ""
    
    run_tidy || log_error "clang-tidy check failed"
    
    log_info "All static analysis complete!"
}

usage() {
    echo "Usage: $0 [asan|ubsan|tidy|all]"
    echo ""
    echo "Options:"
    echo "  asan   - Build with Address Sanitizer"
    echo "  ubsan  - Build with Undefined Behavior Sanitizer"
    echo "  tidy   - Run clang-tidy static analysis"
    echo "  all    - Run all checks (default)"
    exit 1
}

main() {
    if [ $# -eq 0 ]; then
        run_all
        return $?
    fi
    
    case "$1" in
        asan)
            run_asan
            ;;
        ubsan)
            run_ubsan
            ;;
        tidy)
            run_tidy
            ;;
        all)
            run_all
            ;;
        -h|--help)
            usage
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            ;;
    esac
}

main "$@"
