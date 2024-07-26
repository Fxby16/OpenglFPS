#!/bin/bash

cd vendor/raylib/src/
make PLATFORM=PLATFORM_DESKTOP -j6
cd ../..