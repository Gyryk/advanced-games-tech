project "LinearMath"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    systemversion "latest"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("inter/" .. outputdir .. "/%{prj.name}")

	warnings "Off"
	includedirs {
		"..",
	}
	files {
		"*.cpp",
		"*.h",
		"TaskScheduler/*.cpp",
		"TaskScheduler/*.h"
	}

	filter "system:windows"
        systemversion "latest"


    filter {}   -- applies to every configuration

    -- Publish the built lib to lib/<Config>/ - the location LibDir points at.
    -- Debug libs are committed there so students never compile dependencies;
    -- building this project in any other configuration (after reloading it)
    -- populates that configuration too, which is what makes Release work.
    postbuildcommands
    {
        '{MKDIR} "%{prj.location}/lib/%{cfg.buildcfg}"',
        '{COPY} "%{cfg.buildtarget.abspath}" "%{prj.location}/lib/%{cfg.buildcfg}/"',
    }

    filter "configurations:Debug"
        runtime "Debug"
        -- No debug info in the shipped lib: with symbols on, the lib points at
        -- a .pdb that the student ZIPs do not contain, and every link of the
        -- game reports LNK4099 per object. Same choice as assimp.
        symbols "off"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
