solution "Redis"
	configurations { "Debug", "Release" }
	location ( "Build" )
	startproject "module"
	targetdir "Bin/%{cfg.buildcfg}/%{cfg.platform}"

	platforms { "x86", "x64" }
	pic "On"
	symbols "On"
	
	includedirs { "vendor", "." }

	filter "system:windows"
		defines { "WINDOWS", "WIN32" }

	filter "configurations:Debug"
		defines { "DEBUG" }

	filter "configurations:Release"
		optimize "On"

	include "cpp_redis/tacopie/tacopie"
	include "cpp_redis/cpp_redis"
	include "module"
	--include "test"
