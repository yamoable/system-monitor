#!/bin/bash

VERSION="1.0.0"

# build.sh
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Help function
show_help() {
    echo "Usage: ./build.sh [option]"
    echo "Options:"
    echo "  build    - Build the project (default)"
    echo "  run      - Build and run the project"
    echo "  clean    - Clean build directory"
    echo "  help     - Show this help message"
}

# Build function
build_project() {
    echo "Starting build process..."

    # Create build directory if it doesn't exist
    if [ ! -d "build" ]; then
        echo "Creating build directory..."
        mkdir build
    fi

    # Clean build directory
    echo "Cleaning build directory..."
    rm -rf build/*

    # Enter build directory
    cd build

    # Run cmake
    echo "Running cmake..."
    if cmake ..; then
        echo -e "${GREEN}CMake configuration successful${NC}"
    else
        echo -e "${RED}CMake configuration failed${NC}"
        exit 1
    fi

    # Run make
    echo "Running make..."
    if make -j$(nproc); then
        echo -e "${GREEN}Build successful!${NC}"
    else
        echo -e "${RED}Build failed${NC}"
        exit 1
    fi
    echo "Building version ${VERSION}..."
    cmake -DAPP_VERSION="${VERSION}" ..
}

# Run function
run_project() {
    build_project
    echo -e "${YELLOW}Running system-monitor...${NC}"

    # environment variables
    export LIBGL_ALWAYS_SOFTWARE=1

    ./system-monitor
}

# Clean function
clean_project() {
    echo "Cleaning project..."
    rm -rf build
    echo -e "${GREEN}Clean successful!${NC}"
}

# Main script
case "$1" in
    "build")
        build_project
        ;;
    "run")
        run_project
        ;;
    "clean")
        clean_project
        ;;
    "help")
        show_help
        ;;
    "")
        build_project
        ;;
    *)
        echo -e "${RED}Unknown option: $1${NC}"
        show_help
        exit 1
        ;;
esac