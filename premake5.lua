workspace "OpenglFPS"
    configurations { "Debug", "Release" }

project "OpenglFPS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Bin/%{cfg.buildcfg}"
    objdir "Obj/%{cfg.buildcfg}"

    files { "Source/**", "Vendor/glad/src/glad.c" }

    includedirs { "Source", "Vendor/assimp/include", "Vendor/glfw/include", "Vendor/stb",
                  "Vendor/glad/include", "Vendor/glm/glm", "Vendor/freetype/include",
                  "Vendor/json/single_include/nlohmann", "Vendor/imgui", "Vendor/imgui/backends",
                  "Vendor/imguizmo", "Vendor/nativefiledialog/src", "Vendor/nativefiledialog/src/include" }

    libdirs { "Vendor/assimp/build/lib", "Vendor/glfw/build/src", "Vendor/freetype/objs",
              "Vendor/imgui/build/Release", "Vendor/nativefiledialog/build/lib/Release/x64" }

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
