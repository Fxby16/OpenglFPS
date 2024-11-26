#!/bin/bash

cd Vendor/glfw
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cd build
make -j6
cd ../../

cd assimp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
cd build
make -j6
cd ../..

cd freetype
make setup ansi
make -j6
cd ..

premake5 gmake2 --file=imgui.lua
make -j6 config=release

cd nativefiledialog/build/gmake_linux
make config=release_x64 -j6
cd ../../../

cd joltphysics/Build
./cmake_linux_clang_gcc.sh Release g++ -D DEBUG_RENDERER_IN_DEBUG_AND_RELEASE=OFF -D PROFILER_IN_DEBUG_AND_RELEASE=OFF -D ENABLE_OBJECT_STREAM=OFF
cd Linux_Release
make -j6

cd ..
./cmake_linux_clang_gcc.sh Debug g++ -D DEBUG_RENDERER_IN_DEBUG_AND_RELEASE=OFF -D PROFILER_IN_DEBUG_AND_RELEASE=OFF -D ENABLE_OBJECT_STREAM=OFF -D ENABLE_ASSERTS=OFF
cd Linux_Debug
make -j6 -B