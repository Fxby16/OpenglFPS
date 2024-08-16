workspace "ImGui"
    configurations { "Debug", "Release" }

project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "imgui/build/%{cfg.buildcfg}"

    files{ "imgui/*.cpp", "imgui/backends/imgui_impl_glfw.cpp", "imgui/backends/imgui_impl_opengl3.cpp",
           "imguizmo/*.cpp" }

    includedirs{ "imgui", "imgui/backends", "imguizmo", "glfw/include" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"
        symbols "Off"