
solution "DuneR"

	configurations { "Debug", "Release" }
	includedirs { "../../../usr/include", "../../Common/Src/" }
	libdirs { "../../../usr/lib" }

	configuration "vs*"
		defines { "WIN32", "_WIN32", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }

	configuration "linux"
		defines { "_LINUX" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	project "Core"

		kind "StaticLib"
		language "C++"

		files { "../../Common/Src/Core/**.h", "../../Common/Src/Core/**.cpp" }

	project "DuneR"

		kind "ConsoleApp"
		language "C++"
		targetdir "../Data"
		links { "SDL", "SDLmain", "SDL_mixer", "Core" }

		includedirs { "../Src/", "../Src/Game", "../Src/Gfx" }

		files { "../Src/**.h", "../Src/**.cpp" }
