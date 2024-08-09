#!/bin/bash

cd vendor/glfw
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