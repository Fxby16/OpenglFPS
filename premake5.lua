workspace "RaylibFPS"
    configurations { "Debug", "Release" }


project "RaylibFPS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**" }

    includedirs { "src", "vendor/raylib/src", "vendor/raylib/src/external/glfw/include", 
                  "vendor/raylib/src/external", "vendor/assimp/include" }

    libdirs { "vendor/raylib/src", "vendor/assimp/build/lib" }

    links { "raylib", "assimp", "z", "minizip" }

filter "configurations:Debug"
    optimize "Debug"
    symbols "On"
    defines { "DEBUG" }

filter "configurations:Release"
    optimize "Full"
    symbols "Off"
    defines { "NDEBUG" }
