workspace "OpenglFPS"
    configurations { "Debug", "Release" }
	platforms { "x64" }

project "OpenglFPS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Bin/%{cfg.buildcfg}"
    objdir "Obj/%{cfg.buildcfg}"

    files { "Source/**", "Vendor/glad/src/glad.c" }

    includedirs { "Source", "Vendor", "Vendor/assimp/include", "Vendor/assimp/build/include", "Vendor/glfw/include", "Vendor/stb",
                  "Vendor/glad/include", "Vendor/glm/glm", "Vendor/freetype/include",
                  "Vendor/json/single_include/nlohmann", "Vendor/imgui", "Vendor/imgui/backends",
                  "Vendor/imguizmo", "Vendor/nativefiledialog/src", "Vendor/nativefiledialog/src/include" }

	filter "system:linux"
		libdirs { "Vendor/assimp/build/lib", "Vendor/glfw/build/src", "Vendor/freetype/objs/x64/Release Static",
                  "Vendor/imgui/build/Release", "Vendor/nativefiledialog/build/lib/Release/x64" }

		links { "glfw3", "assimp", "z", "minizip", "freetype", "ImGui", "nfd", "gtk-3", "glib-2.0" }

	filter "system:windows"
		libdirs { "Vendor/assimp/build/lib/Release", "Vendor/glfw/build/src/Release", "Vendor/freetype/objs",
                  "Vendor/imgui/build/Release", "Vendor/nativefiledialog/build/lib/Release/x64", "Vendor/zlib/build/Release" }

		links { "glfw3", "assimp-vc143-mt", "freetype", "ImGui", "nfd", "zlibstatic" }


filter "configurations:Debug"
    optimize "Debug"
    symbols "On"
    buildoptions { "-Wall", "-fsanitize=address" }
    linkoptions { "-fsanitize=address" }
    defines { "DEBUG", "GLFW_INCLUDE_NONE" }

filter "configurations:Release"
    optimize "Full"
    symbols "Off"
    buildoptions { "-Wall" }
    defines { "NDEBUG", "GLFW_INCLUDE_NONE" }