project "test"
	language "C++"
	cppdialect "C++17"
	kind "ConsoleApp"

	libdirs { "lib" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Test files"] = "**.json",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h",
	}

	filter { "system:windows", "platforms:x86" }
		links { "tacopie.lib" }
		links { "cpp_redis.lib" }
	
	filter { "system:windows", "platforms:x64" }
		links { "tacopie_64.lib" }
		links { "cpp_redis_64.lib" }

	filter "system:linux"
