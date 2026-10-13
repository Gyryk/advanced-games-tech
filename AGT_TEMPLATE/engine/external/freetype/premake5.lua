project "freetype"
  warnings "off"   -- third-party source; not ours to fix
  kind "StaticLib"
  language "C++"

  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("inter/" .. outputdir .. "/%{prj.name}")

  files {
    "src/autofit/autofit.c",
    "src/bdf/bdf.c",
    "src/cff/cff.c",
    "src/base/ftbase.c",
    "src/base/ftbitmap.c",
    "src/base/ftdebug.c",
    "src/base/ftfstype.c",
    "src/base/ftgasp.c",
    "src/base/ftglyph.c",
    "src/gzip/ftgzip.c",
    "src/base/ftinit.c",
    "src/lzw/ftlzw.c",
    "src/base/ftstroke.c",
    "src/base/ftsystem.c",
    "src/smooth/smooth.c",
    "src/base/ftbbox.c",
    "src/base/ftmm.c",
    "src/base/ftpfr.c",
    "src/base/ftsynth.c",
    "src/base/fttype1.c",
    "src/base/ftwinfnt.c",
    "src/base/ftgxval.c",
    "src/base/ftotval.c",
    "src/base/ftpatent.c",
    "src/pcf/pcf.c",
    "src/pfr/pfr.c",
    "src/psaux/psaux.c",
    "src/pshinter/pshinter.c",
    "src/psnames/psnames.c",
    "src/raster/raster.c",
    "src/sdf/sdf.c",
    "src/svg/ftsvg.c",
    "src/sfnt/sfnt.c",
    "src/truetype/truetype.c",
    "src/type1/type1.c",
    "src/cid/type1cid.c",
    "src/type42/type42.c",
    "src/winfonts/winfnt.c",
    "include/ft2build.h",
    "include/ft2build.h",
    "include/freetype/config/ftconfig.h",
    "include/freetype/config/ftheader.h",
    "include/freetype/config/ftmodule.h",
    "include/freetype/config/ftoption.h",
    "include/freetype/config/ftstdlib.h",
  }
  
  includedirs{
      "include/",
  }
  defines{
    "FT2_BUILD_LIBRARY"
    -- no DLL_EXPORT: static lib, not a DLL
  }

  filter "system:windows"
    systemversion "latest"
    cppdialect "C++17"
    staticruntime "On"
    
    defines{
      "WIN32",
      "_LIB",
    }

  filter "configurations:Debug"
      defines "PYRO_DEBUG"
      runtime "Debug"
      -- No debug info in the shipped lib: with symbols on, the lib points at
      -- a .pdb that the student ZIPs do not contain, and every link of the
      -- game reports LNK4099 per object. Same choice as assimp.
      symbols "off"
    
      defines{
        "_DEBUG",
        "_CRT_SECURE_NO_WARNINGS",
        "FT_DEBUG_LEVEL_ERROR",
        "FT_DEBUG_LEVEL_TRACE",
        "_CRT_SECURE_NO_DEPRECATE",
      }

  filter "configurations:Release"
      defines "PYRO_RELEASE"
      runtime "Release"
      optimize "on"
    
      defines{
        "NDEBUG",
        "_CRT_SECURE_NO_WARNINGS",
      }

  filter "configurations:Dist"
      defines "PYRO_DIST"
      runtime "Release"
      optimize "on"
    
      defines{
        "NDEBUG",
        "_CRT_SECURE_NO_WARNINGS",
      }

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
