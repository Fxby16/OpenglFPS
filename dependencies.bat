@echo off
set GENERATOR="Visual Studio 17 2022"
set ARCH=x64

:: GLFW
cd Vendor\glfw
cmake -S . -B build -G %GENERATOR% -A %ARCH% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release -- /m:6
cd ..

:: Assimp
cd assimp
cmake -S . -B build -G %GENERATOR% -A %ARCH% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
cmake --build build --config Release -- /m:6
cd ..

:: FreeType (Using Makefile)
cd freetype
MSBuild.exe MSBuild.sln /p:Configuration="Release Static" /m:6
cd ..

:: ImGui (Using premake5.exe for Visual Studio 2022)
premake5.exe vs2022 --file=imgui.lua
MSBuild.exe imgui.sln /p:Configuration=Release /m:6

:: Native File Dialog (Using premake5.exe for Visual Studio 2022)
cd nativefiledialog/build
premake5.exe vs2022
MSBuild.exe NativeFileDialog.sln /p:Configuration=Release /p:Platform=x64 /m:6
cd ..\..

cd zlib
cmake -S . -B build -G %GENERATOR% -A %ARCH% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release --target zlibstatic -- /m:6
cd ..
