project "module"
	language "C++"
	cppdialect "C++17"
	kind "SharedLib"
	targetname "ml_redis"
	
	includedirs { "include" }
	libdirs { "lib" }

	dependson { "cpp_redis" }
    links { "cpp_redis" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h"
	}
	
	filter { "system:windows" }
		debugdir "../mta-server"
		
	filter { "system:windows", "platforms:x86" }
		debugcommand "../mta-server/MTA Server.exe"
		
	filter { "system:windows", "platforms:x64" }
		debugcommand "../mta-server/MTA Server64.exe"

	-- filter "system:linux"

	filter "system:not linux"
		excludes { "luaimports/luaimports.linux.h", "luaimports/luaimports.linux.cpp" }
