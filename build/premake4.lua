--
-- premake4 file to build SteerSuite
-- http://steersuite.cse.yorku.ca
-- Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
-- See license.txt for complete license.
--

local action = _ACTION or ""
local todir = "./" .. action

function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end

solution "0_steersuite"
	configurations { 
		"Debug",
		"Release"
	}
	
	platforms {
		"x32", 
		"x64"
	}
	location (todir)

	-- extra warnings, no exceptions or rtti
	flags { 
		"ExtraWarnings",
--		"FloatFast",
--		"NoExceptions",
--		"NoRTTI",
		"Symbols"
	}
	defines { "ENABLE_GUI", "ENABLE_GLFW" }

	-- debug configs
	configuration "Debug*"
		defines { "DEBUG" }
		flags {
			"Symbols",
			Optimize = Off
		}
 
 	-- release configs
	configuration "Release*"
		defines { "NDEBUG" }
		flags { "Optimize" }

	-- windows specific
	configuration "windows"
		defines { "WIN32", "_WINDOWS" }
		libdirs { "lib" }
		targetdir ( "bin" )

	configuration { "linux" }
		linkoptions { 
			-- "-stdlib=libc++" ,
			"-Wl,-rpath," .. path.getabsolute("lib")
		}
		targetdir ( "lib" )
		
	configuration { "macosx" }
        buildoptions { "-stdlib=libc++" }
		linkoptions { 
			"-stdlib=libc++" ,
			"-Wl",
			"-Wl,-rpath," .. path.getabsolute("lib")
		}
		links {
	        "OpenGL.framework",
	        "Cocoa.framework",
        }
		libdirs {
			"/usr/local/lib",
			"/usr/lib",
		}
        targetdir ( "lib" )
      
	if os.get() == "macosx" then
		premake.gcc.cc = "clang"
		premake.gcc.cxx = "clang++"
		-- buildoptions("-std=c++0x -ggdb -stdlib=libc++" )
	end

	if  os.get() == "linux" then
		premake.dotnet = "mono"
	end

project "steersim"
	language "C++"
	kind "ConsoleApp"
	includedirs { 
		"/usr/local/include",
		"/usr/include",
		"../steerlib/include",
		"../steersim/include",
		"../steersimlib/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../steersim/include/*.h",
		"../steersim/src/*.cpp"
	}
	libdirs {
		"lib",
		"/usr/lib",
		"/usr/local/lib",
	}
	links { 		
		"glfw3",
		"steerlib",
		"steersimlib",
		"util",
	}
	defines { -- preprocessor defines for this project
		"_CRT_SECURE_NO_WARNINGS",
		"_SCL_SECURE_NO_WARNINGS",
	}

	targetdir "bin"
	buildoptions("-std=c++0x -ggdb" )	

	-- linux library cflags and libs
	configuration { "linux", "gmake" }
		linkoptions { 
			"-Wl,-rpath," .. path.getabsolute("lib") ,
		}
		libdirs {
			"lib",
			"/usr/lib",
			"/usr/local/lib",
		}
		links {
			"X11",
			"Xrandr",
			"Xi",
			"Xxf86vm",
			"Xinerama",
			"Xcursor",
			"GL",
			"GLU",
			"GLEW",
		}
		defines {
			"_LINUX_",
		}

	-- windows library cflags and libs
	configuration { "windows" }
		-- libdirs { "lib" }
		links { 
			"opengl32",
			"glew32",
			"glfw3",
		}
		
	configuration { "x32", "windows" }
		libdirs { 
			"../external/glew/lib/Release/Win32", 
			"../external/glfw3/lib-vc2013",
		}
		
	configuration { "x64", "windows" }
		libdirs { 
			"../external/glew/lib/Release/x64", 
			"../external/glfw3x64/lib-vc2013",
		}

	-- mac includes and libs
	configuration { "macosx" }
		kind "ConsoleApp" -- xcode4 failes to run the project if using WindowedApp
		includedirs { 
			"/usr/include",
			"/usr/local/include",
			"/Library/Frameworks/SDL.framework/Headers" }
		buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }
		linkoptions { 
			"-Wl,-rpath," .. path.getabsolute("lib") ,
		}
		links { 
			"OpenGL.framework", 
			"Cocoa.framework",
			"tinyxml",
			"dl",
			"pthread",
			"glfw3",
			"GLEW",
		}
		libdirs { 
			"lib", 
			"/usr/lib",
			"/usr/local/lib",
		}

project "steersimlib"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../steersimlib/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../steersimlib/include/*.h",
		"../steersimlib/include/**.h",
		"../steersimlib/src/*.cpp"
	}
	links { 		
		"steerlib",
		"util",
		"glfw3"
	}

	
	buildoptions("-std=c++0x -ggdb" )	

	-- linux library cflags and libs
	configuration { "linux" }
		buildoptions { 
			"-fPIC"
		}
		linkoptions { 
			"-Wl,-rpath," .. path.getabsolute("lib") ,
			"-fPIC"
		}
		libdirs { 
			"/usr/local/lib",
			"lib" 
		}
		links {
			"X11",
			"Xrandr",
			"Xi",
			"Xxf86vm",
			"Xinerama",
			"Xcursor",
			"GL",
			"GLU",
			"GLEW",
			"dl",
			"tinyxml",
		}

	-- library windows cflags and libs
	configuration { "windows" }
		-- libdirs { "../RecastDemo/Contrib/SDL/lib/x86" }
		links { 
			"opengl32",
			"glew32",
			"glfw3"
		}
		defines { "GLFW_BUILD_DLL" }
		
	configuration { "x32", "windows" }
		libdirs { 
			"../external/glew/lib/Release/Win32",
			"../external/glfw3/lib-vc2013",
		}
	configuration { "x64", "windows" }
		libdirs { 
			"../external/glew/lib/Release/x64", 
			"../external/glfw3x64/lib-vc2013",
		}
		
	configuration { "macosx" }
		links {
			"dl",
			"pthread",
			"tinyxml",
			"GLEW"
		}
		linkoptions { 
			"-install_name @rpath/libsteersimlib.dylib"
		}
		libdirs {
			"/usr/lib",
			"/usr/local/lib",
		}
		

--[====[
	-- mac includes and libs
	configuration { "macosx" }
		kind "ConsoleApp" -- xcode4 failes to run the project if using WindowedApp
		includedirs { "/Library/Frameworks/SDL.framework/Headers" }
		buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }
		links { 
			"OpenGL.framework", 
			"Cocoa.framework",
		}
--]====]
		
project "tinyxml"
        language "C++"
        kind "SharedLib"
        includedirs {
                "../external/tinyxml"
        }
        files {
                "../external/tinyxml/*.h",
                "../external/tinyxml/*.cpp"
        }
        
		
        configuration { "macosx" }
			linkoptions { 
				"-install_name @rpath/libtinyxml.dylib"
			}
			buildoptions("-std=c++0x -ggdb" )	
			
		 configuration { "linux" }
			buildoptions("-std=c++0x -ggdb" )	
		
		
			
project "util"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../util/include"
	}
	files { 
		"../util/include/*.h",
		"../util/include/**.h",
		"../util/src/*.cpp"
	}
	-- targetdir "bin"
	configuration { "macosx" }
		linkoptions { 
			"-install_name @rpath/libutil.dylib"
		}
		buildoptions("-std=c++0x -ggdb" )	
		
	 configuration { "linux" }
		buildoptions("-std=c++0x -ggdb" )	

project "steerlib"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include" ,
		"../external" ,
		"../steerlib/include/util", 
		"../util/include" 
		
	}
	files { 
		"../steerlib/include/*.h",
		"../steerlib/include/**.h",
		"../steerlib/src/*.cpp" 
	}
	links { 
		-- "tinyxml",
		"util",
		-- "glfw",
		-- "glew32",
	} 
	
	configuration { "windows" }
		files {
			"../external/tinyxml/*.h",
			"../external/tinyxml/*.cpp" 
		}
		links { 
			"opengl32",
			"glew32",
			"glu32",
			-- "glfw"
		}
		
	configuration { "x32", "windows" }
		libdirs { 
			"../external/glew/lib/Release/Win32", 
			"../external/glfw3/lib-vc2013",
		}
		
	configuration { "x64", "windows" }
		libdirs { 
			"../external/glew/lib/Release/x64", 
			"../external/glfw3x64/lib-vc2013",
		}
	
	configuration { "macosx" }
		links {
			"OpenGL.framework",
			"Cocoa.framework",
			"GLEW",
			"tinyxml",
			"pthread",
		}
		linkoptions { 
			"-install_name @rpath/libsteerlib.dylib"
		}
		buildoptions("-std=c++0x -ggdb" )	
			
	 configuration { "linux" }
		buildoptions("-std=c++0x -ggdb" )	
		links { 
			"tinyxml",
			"pthread",
			-- "glfw3",
			-- "glew32",
		}

local glfwLocation = "../external/glfw/"

project "glfw3"
	kind "SharedLib"
    	language "C++"
   	includedirs { 
		glfwLocation .. "include",
		glfwLocation .. "src",
		-- glfwLocation .. "lib/x11"
	}
	files { 
	--	glfwLocation .. "lib/x11/*.h",
	--	glfwLocation .. "lib/x11/*.c",
		glfwLocation .. "src/init.c",
		glfwLocation .. "src/input.c",
		glfwLocation .. "src/monitor.c",
		glfwLocation .. "src/context.c",
		glfwLocation .. "src/window.c",
		-- glfwLocation .. "src/glx_context.c",
		glfwLocation .. "src/xkb_unicode.c",
		glfwLocation .. "src/vulkan*.c",
	}
	defines { "GLFW_BUILD_DLL" }
	

    configuration {"linux"}
		-- targetdir ( "./lib" )
        files { 
			glfwLocation .. "src/x11*.c",
			glfwLocation .. "src/linux*.c",
			glfwLocation .. "src/posix*.c",
			glfwLocation .. "src/glx_context.c",
            glfwLocation .. "src/egl_context.c",
			-- glfwLocation .. "x11/*.h" 
		}
        defines { 
			"_GLFW_USE_LINUX_JOYSTICKS", 
			"_GLFW_HAS_XRANDR", 
			"_GLFW_HAS_PTHREAD", 
			"_GLFW_HAS_SCHED_YIELD", 
			"_GLFW_HAS_GLXGETPROCADDRESS", 
			"_GLFW_X11",
		 }
        links { 
			"pthread",
			"Xrandr",
			"X11",
			"Xi",
			-- "gl",
			"Xcursor",
			"GL",
			"GLU",
		}

        
        buildoptions { 
			"-pthread",
			"-fPIC",
		}
       
    configuration {"windows"}
        files { 
			glfwLocation .. "src/win32*.c",
			"include/glad/src/*.c",
			"include/glad/include/glad/*.h",
			glfwLocation .. "src/wgl_context.c",
			glfwLocation .. "src/*.h",
			glfwLocation .. "include/GLFW/*.h",
			glfwLocation .. "src/egl_context.c",
			-- glfwLocation .. "x11/*.h" 
		}
		includedirs { 
			"include/glad/include",
			-- glfwLocation .. "lib/x11"
		}
		links {
			"opengl32"
		}
        defines { 
        	"_GLFW_USE_LINUX_JOYSTICKS", 
        	"_GLFW_HAS_XRANDR", 
        	"_GLFW_HAS_PTHREAD",
        	"_GLFW_HAS_SCHED_YIELD", 
        	"_GLFW_HAS_GLXGETPROCADDRESS",
        	"_GLFW_WIN32",
        	"_WIN32",
        	"_GLFW_BUILD_DLL",
        	-- GLAD STUFF
        	"GLAD_GLAPI_EXPORT",
        	"WIN32",
        	"GLAD_GLAPI_EXPORT_BUILD",
        	
        }
       
    configuration {"macosx"}
		-- targetdir ( "./lib" )
        files { 
        	glfwLocation .. "src/cocoa*.c",
            glfwLocation .. "src/cocoa*.m",
			-- glfwLocation .. "src/linux*.c",
			-- glfwLocation .. "src/posix*.c",
			glfwLocation .. "src/nsgl*.m",
            glfwLocation .. "src/posix_tls.c",

		}
        includedirs { 
        	glfwLocation .. "lib/cocoa" 
        }
        defines {
        	"_GLFW_COCOA",
        }
        links { 
			"pthread",
		}

        
--	removebuildoptions "-std=c++0x"
        linkoptions { 
		"-framework OpenGL", 
		"-framework Cocoa", 
		"-framework IOKit", 
        "-framework CoreVideo",
        "-install_name @rpath/libglfw3.dylib"
	}
	buildoptions {
                        "-fPIC",
}

project "simpleAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../simpleAI/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../simpleAI/include/*.h",
		"../simpleAI/src/*.cpp"
	}
	links { 
		"util",
		"steerlib"
	}
	
		
	
	configuration { "macosx" }
		linkoptions { 
			"-install_name @rpath/libutil.dylib"
		}
		buildoptions("-std=c++0x -ggdb" )	
		
	 configuration { "linux" }
		buildoptions("-std=c++0x -ggdb" )	
	
project "sfAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../socialForcesAI/include",
		"../external",
		"../util/include", 
		"/usr/include",
		"/usr/local/include",
	}
	files { 
		"../socialForcesAI/include/*.h",
		"../socialForcesAI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util",
		-- "glew32"
	}
		
	buildoptions("-std=c++0x -ggdb" )	
	
project "pamAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../pamAI/include",
		"../external",
		"../util/include",
		"/usr/include",
		"/usr/local/include",
	}
	files { 
		"../pamAI/include/*.h",
		"../pamAI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util"
	}
		
	buildoptions("-std=c++0x -ggdb" )	

project "rvo2dAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../rvo2AI/include",
		"../external",
		"../util/include",
		"../kdtree/include", 
		-- "../meshdatabase/include",
		-- "../acclmesh/include"
	}
	files { 
		"../rvo2AI/include/*.h",
		"../rvo2AI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util",
		-- "meshdatabase"
	}
		
	buildoptions("-std=c++0x -ggdb" )	

project "pprAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../pprAI/include",
		"../external",
		"../util/include",
	}
	files { 
		"../pprAI/include/*.h",
		"../pprAI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util",
	}
		
	buildoptions("-std=c++0x -ggdb" )	
	
	
project "kdtree"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../kdtree/include",
		"../external",
		"../util/include",
	}
	files { 
		"../kdtree/include/*.h",
		"../kdtree/src/*.cpp"
	}
	links { 
		"steerlib",
		"util",
	}
	
	buildoptions("-std=c++0x -ggdb" )	
	configuration { "macosx" }
                linkoptions {
                        "-install_name @rpath/libkdtree.dylib"
                }

project "Recast"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../navmeshBuilder/include",
		"../external/recastnavigation/Recast/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/Recast/Include/*.h",
		"../external/recastnavigation/Recast/Source/*.cpp",
	}
	links { 
		"steerlib",
		"util",
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libRecast.dylib"
        }
		buildoptions { 
			"-fPIC"
		}

project "DebugUtils"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../navmeshBuilder/include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/DebugUtils/Include/*.h",
		"../external/recastnavigation/DebugUtils/Source/*.cpp",
	}
	links { 
		"Recast",
		"Detour"
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libDebugUtils.dylib"
        }

project "Detour"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../navmeshBuilder/include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/Detour/Include/*.h",
		"../external/recastnavigation/Detour/Source/*.cpp",
	}
	links { 
		"Recast",
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libDetour.dylib"
        }

project "DetourCrowd"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../navmeshBuilder/include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../external/recastnavigation/DetourCrowd/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/DetourCrowd/Include/*.h",
		"../external/recastnavigation/DetourCrowd/Source/*.cpp",
	}
	links { 
		"Recast",
		"Detour"
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libDetourCrowd.dylib"
        }	

project "navmesh"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../navmeshBuilder/include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../external/recastnavigation/DetourCrowd/Include",
		"../steersimlib/include",
		"../external",
		"../util/include",
	}
	files { 
		"../navmeshBuilder/include/*.h",
		"../navmeshBuilder/src/*.cpp"
	}
	links { 
		"steerlib",
		"steersimlib",
		"util",
		"Recast",
		"DebugUtils",
		"Detour",
		"DetourCrowd",
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
		linkoptions {
			"-install_name @rpath/libnavmesh.dylib"
        }

	
project "steerbench"
	language "C++"
	kind "ConsoleApp"
	includedirs { 
		"/usr/include",
		"/usr/local/include",
		"../steerlib/include",
		"../steerbench/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../steerbench/include/*.h",
		"../steerbench/src/*.cpp"
	}
	links { 		
		"steerlib",
		"util",
		"glfw3",
	}


	targetdir "bin"
	buildoptions("-std=c++0x -ggdb" )	

	-- linux library cflags and libs
	configuration { "linux", "gmake" }
		-- kind "ConsoleApp"
		buildoptions { 
			"`pkg-config --cflags gl`",
			"`pkg-config --cflags glu`" 
		}
		linkoptions { 
			-- "-Wl,-rpath,./lib",
			"-Wl,-rpath," .. path.getabsolute("lib") ,
			"`pkg-config --libs gl`",
			"`pkg-config --libs glu`" 
		}
		links { 		
			"GLU",
			"GL",
			"dl",
			"tinyxml",
			"GLEW",
		}
		libdirs { "lib" }

	-- windows library cflags and libs
	configuration { "windows" }
		libdirs { "../RecastDemo/Contrib/SDL/lib/x86","../external/glew/lib/Release/Win32","../external/glfw3/lib-vc2013" }
		links { 
			"opengl32",
			"glew32",
			"glfw3",
		}

	-- mac includes and libs
	configuration { "macosx" }
		kind "ConsoleApp" -- xcode4 failes to run the project if using WindowedApp
		buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }
		links { 
			"OpenGL.framework", 
			"Cocoa.framework",
			"dl",
			"tinyxml",
		}


if file_exists("premake4-dev.lua")
	then
	dofile("premake4-dev.lua")
end
