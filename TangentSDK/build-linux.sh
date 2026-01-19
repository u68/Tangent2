#!/usr/bin/env bash
set -euo pipefail

# This script targets Linux

check_command() {
  command -v "$1" >/dev/null 2>&1 || { echo >&2 "ERROR: '$1' is required but not found. Please install it and re-run."; exit 1; }
}

check_any() {
  for c in "$@"; do
    command -v "$c" >/dev/null 2>&1 && return 0
  done
  return 1
}

echo "Checking prerequisites..."
check_command dmd
if ! check_any cmake cmake3; then echo >&2 "ERROR: 'cmake' is not installed. Install CMake (https://cmake.org) and re-run."; exit 1; fi
if ! check_any ninja ninja-build; then echo >&2 "ERROR: 'ninja' is not installed. Install Ninja (https://ninja-build.org) and re-run."; exit 1; fi

# Build the build tools
echo "Building build tools..."
rm -rf buildtools
mkdir -p buildtools

echo "Building Assembler"
dmd src/buildtools/tas_t16.d -of=buildtools/tas-t16

echo "Building ML Compiler"
dmd src/buildtools/tmlc_t16.d -of=buildtools/tmlc-t16

echo "Building Linker"
dmd src/buildtools/tl_t16.d -of=buildtools/tl-t16

# Build the IDE
echo "Building TangentSDK IDE..."
rm -rf build
mkdir build
cd build
cmake -G Ninja ..
ninja
./tangentsdk
