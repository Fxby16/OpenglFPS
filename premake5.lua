workspace "OpenglFPS"
    configurations { "Debug", "Release" }

project "OpenglFPS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**", "vendor/glad/src/glad.c" }

    includedirs { "src", "vendor/assimp/include", "vendor/glfw/include", "vendor/stb",
                  "vendor/glad/include", "vendor/glm/glm", "vendor/freetype/include",
                  "vendor/json/single_include/nlohmann", "vendor/imgui", "vendor/imgui/backends",
                  "vendor/imguizmo", "vendor/nativefiledialog/src", "vendor/nativefiledialog/src/include" }

    libdirs { "vendor/assimp/build/lib", "vendor/glfw/build/src", "vendor/freetype/objs",
              "vendor/imgui/build/Release", "vendor/nativefiledialog/build/lib/Release/x64" }

    links { "glfw3", "assimp", "z", "minizip", "freetype", "ImGui", "nfd", "gtk-3", "glib-2.0" }

filter "configurations:Debug"
    optimize "Debug"
    symbols "On"
    buildoptions { "-Wall" }
    defines { "DEBUG", "GLFW_INCLUDE_NONE" }

filter "configurations:Release"
    optimize "Full"
    symbols "Off"
    buildoptions { "-Wall" }
    defines { "NDEBUG", "GLFW_INCLUDE_NONE" }
