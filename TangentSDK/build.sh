#!/bin/bash

# Build the build tools
echo "Building build tools..."
rm -rf buildtools
mkdir -p buildtools
echo "Building Assembler"
dmd src/buildtools/tas_t16.d -of=buildtools/tas-t16.exe
echo "Building ML Compiler"
dmd src/buildtools/tmlc_t16.d -of=buildtools/tmlc-t16.exe
echo "Building Linker"
dmd src/buildtools/tl_t16.d -of=buildtools/tl-t16.exe

# Build the IDE
echo "Building TangentSDK IDE..."
rm -rf build
mkdir build
cd build
cmake -G Ninja ..
ninja
./tangentsdk
