# OpenglFPS

## Dependencies
- premake5
- zenity (only on Linux)

Everything else is cloned with the repo.

## Linux
### Build
`./dependencies.bash`  
`premake5 gmake2`  
`make`. Use `make config=release` to build in release mode.  

## Run
`Bin/Debug/RaylibFPS` or `Bin/Release/RaylibFPS`

## Windows
### Build
`.\dependencies.bat`  
`premake5.exe vs2022`  
Open the solution and compile it.  
Run the application (currently works only in release mode, i have to add libraries compilation in debug mode).

### Warnings
- VS2022 gives some errors about strings conversion, but the binaries are still generated. Will try to fix this in the future.  
- This project is developed on Linux and then ported to Windows, so I cannot guarantee it will build successfully on Windows. As I write this, it builds successfully.

## Images
![1](Demos/screenshot1.png)
![0](Demos/screenshot0.png)

## Clips
![1](Demos/animated_shadows.gif)