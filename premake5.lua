workspace "Break"
	configurations {"Debug", "Release"}
	location "build"
		
project "Break_Graphics"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	location "Graphics"
	
	files {"Graphics/inc/**.hpp", "Graphics/src/**.cpp"}
	
	links {"Break_Infrastructure"}
	
	includedirs{"Graphics/inc"}
	
	includedirs{
		"Infrastructure/inc",
		"Graphics/deps/glm/include",
	}
	
	configuration {"linux", "gmake"}
		buildoptions{"-std=c++11", "-pthread"}
	
	filter "configurations:Debug"
		defines {"DEBUG","COMPILE_DLL"}
		flags {"Symbols"}
	
	filter "configurations:Release"
		defines {"NDEBUG","COMPILE_DLL"}
		optimize "On"
		
project "Break_Infrastructure"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	location "Infrastructure"

	files {"Infrastructure/inc/**.hpp", "Infrastructure/src/**.cpp"}
	
	includedirs {"Infrastructure/inc"}
	links {"glfw3", "glew32", "OPENGL32", "FreeImage"}
	
	if os.get() == "windows" then
		
		includedirs{
			"Infrastructure/deps/glm/include",
			"Infrastructure/deps/glew-1.10.0/include",
			"Infrastructure/deps/glfw-3.1.bin.WIN32/include",
			"Infrastructure/deps/freeimage/",
			os.getenv("DXSDK_DIR") .. "/Include"
		}
		libdirs{
				"Infrastructure/deps/glew-1.10.0/lib/Release/Win32",
				"Infrastructure/deps/freeimage/",
				os.getenv("DXSDK_DIR") .. "/Lib/x86"
		}
		if _ACTION == "vs2010" then
			libdirs{
				"Infrastructure/deps/glfw-3.1.bin.WIN32/lib-vc2010"
			}
		elseif _ACTION == "vs2012" then
			libdirs{
				"Infrastructure/deps/glfw-3.1.bin.WIN32/lib-vc2012"
			}
		elseif _ACTION == "vs2013" then
			libdirs{
				"Infrastructure/deps/glfw-3.1.bin.WIN32/lib-vc2013"
			}
		else
			libdirs{
				"Infrastructure/deps/glfw-3.1.bin.WIN32/lib-mingw"
			} 
		end
		
		links {"d3d11", "dxgi", "d3dcompiler", "gdi32"}
	end
	
	configuration {"linux", "gmake"}
		buildoptions{"-std=c++11", "-pthread"}

	filter "configurations:Debug"
		defines {"DEBUG","COMPILE_DLL"}
		flags {"Symbols"}

	filter "configurations:Release"
		defines {"NDEBUG","COMPILE_DLL"}
		optimize "On"
		
project "Break"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"

	files {"main.cpp", "TestApplication.hpp"}
	
	links {
		"Break_Infrastructure", "Break_Graphics"
	}
	
	includedirs {
		"Infrastructure/inc",
		"Graphics/inc",
		"deps/glm/include/"
	}
	
	configuration {"linux", "gmake"}
		buildoptions{"-std=c++11", "-pthread"}

	filter "configurations:Debug"
		defines {"DEBUG"}
		flags {"Symbols"}

	filter "configurations:Release"
		defines {"NDEBUG"}
		optimize "On"