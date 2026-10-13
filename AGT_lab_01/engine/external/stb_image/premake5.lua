project "stb_image"
  kind "StaticLib"
  language "C++"

  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("inter/" .. outputdir .. "/%{prj.name}")

  files {
    "stb_image.h",
    "stb_image.cpp"
  }

  filter "system:windows"
    systemversion "latest"
    cppdialect "C++17"
    staticruntime "On"

  filter "configurations:Debug"
      defines "PYRO_DEBUG"
      runtime "Debug"
      -- No debug info in the shipped lib: with symbols on, the lib points at
      -- a .pdb that the student ZIPs do not contain, and every link of the
      -- game reports LNK4099 per object. Same choice as assimp.
      symbols "off"

  filter "configurations:Release"
      defines "PYRO_RELEASE"
      runtime "Release"
      optimize "on"

  filter "configurations:Dist"
      defines "PYRO_DIST"
      runtime "Release"
      optimize "on"
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
