#!/bin/bash

cd vendor/raylib/src/
make PLATFORM=PLATFORM_DESKTOP -j6
cd ../..

cd assimp
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake -S .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
make -j6
cd ../..