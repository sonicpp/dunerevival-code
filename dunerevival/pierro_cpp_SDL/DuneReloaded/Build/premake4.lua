
solution "DuneR"

	location (_ACTION) --creates a "./vs2008" subfolder, or "./vs2010", etc. and places the solution files there
	configurations { "Debug", "Release" }
	
	--includedirs { "../../../usr/include", "../../Common/Src/" }
	--libdirs { "../../../usr/lib" }
	
	
	--strictly SDL
	includedirs { "../../../../../external_libs/SDL/include/SDL"}
	libdirs { "../../../../../external_libs/SDL/lib" }

	--additional SDL extensions
	includedirs { "../../../../../external_libs/SDL_image/include"}
	libdirs { "../../../../../external_libs/SDL_image/lib" }
	includedirs { "../../../../../external_libs/SDL_mixer/include"}
	libdirs { "../../../../../external_libs/SDL_mixer/lib" }
	includedirs { "../../../../../external_libs/SDL_ttf/include"}
	libdirs { "../../../../../external_libs/SDL_ttf/lib" }
	
	
	

	configuration "vs2008"
		defines { "WIN32", "_WIN32", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }


	configuration "vs2010"
		defines { "WIN32", "_WIN32", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }

		
	configuration "codeblocks"
		--defines { "WIN32", "_WIN32", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }
		links { "mingw32" }
		
	configuration "linux"
		defines { "_LINUX" }
		
	--configuration "windows"

		
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	project "DuneR"

		kind "ConsoleApp"
		language "C++"
		
		--where the excutable will be created
		targetdir "../Data"
		
		--working directory
		-- "../Data" 	--This command DOES NOT EXIST YET in Premake but is under discussion at http://industriousone.com/post/set-working-directory
						--at the moment it has to be changed manually
		
		links { "SDLmain", "SDL"} --do not revert the order
		links {"SDL_image", "SDL_mixer", "SDL_ttf" } 

		--project itself
		includedirs { "../Src" , "../Src/Core" , "../Src/Game" , "../Src/Gfx"}

		files { "../Src/**.h", "../Src/**.cpp" }
