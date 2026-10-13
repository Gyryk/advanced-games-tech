-- assimp 6.0.5, vendored and built from source.
--
-- Trimmed to the three formats the course assets actually use: 3DS, FBX
-- and OBJ (verified by scanning every lesson repo). Every other importer
-- is switched off below, which is what keeps the static lib - and the
-- shipped game.exe - small.
--
-- To re-enable a format, delete its ASSIMP_BUILD_NO_*_IMPORTER line and
-- add the matching code/AssetLib/<Name> folder to files{}. The full assimp
-- source is vendored, so nothing needs downloading.
--
-- NOTE: COLLADA (.dae) is OFF. Only the excluded lab-7-solution repo used it.

project "assimp"
    warnings "off"   -- third-party source; not ours to fix
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("inter/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/**.h",
        "include/**.hpp",
        "include/**.inl",
        "code/Common/**.cpp",
        "code/Common/**.h",
        "code/CApi/**.cpp",
        "code/CApi/**.h",
        "code/Geometry/**.cpp",
        "code/Geometry/**.h",
        "code/Material/**.cpp",
        "code/Material/**.h",
        "code/PostProcessing/**.cpp",
        "code/PostProcessing/**.h",
        "code/AssetLib/3DS/**.cpp",
        "code/AssetLib/3DS/**.h",
        "code/AssetLib/FBX/**.cpp",
        "code/AssetLib/FBX/**.h",
        "code/AssetLib/Obj/**.cpp",
        "code/AssetLib/Obj/**.h",
        "contrib/zlib/*.c",
        "contrib/zlib/*.h",
    }

    includedirs
    {
        ".",
        "include",
        "code",
        "contrib",
        "contrib/zlib",
        "contrib/utf8cpp/source",
        "contrib/rapidjson/include",
        "contrib/pugixml/src",
        "contrib/unzip",
    }

    defines
    {
        "ASSIMP_BUILD_NO_EXPORT",        -- we only ever load models
        "WIN32_LEAN_AND_MEAN",
        "_CRT_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_WARNINGS",

        -- importers switched off (49 of 52)
        "ASSIMP_BUILD_NO_3D_IMPORTER",
        "ASSIMP_BUILD_NO_3MF_IMPORTER",
        "ASSIMP_BUILD_NO_AC_IMPORTER",
        "ASSIMP_BUILD_NO_AMF_IMPORTER",
        "ASSIMP_BUILD_NO_ASE_IMPORTER",
        "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
        "ASSIMP_BUILD_NO_B3D_IMPORTER",
        "ASSIMP_BUILD_NO_BLEND_IMPORTER",
        "ASSIMP_BUILD_NO_BVH_IMPORTER",
        "ASSIMP_BUILD_NO_C4D_IMPORTER",
        "ASSIMP_BUILD_NO_COB_IMPORTER",
        "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
        "ASSIMP_BUILD_NO_CSM_IMPORTER",
        "ASSIMP_BUILD_NO_DXF_IMPORTER",
        "ASSIMP_BUILD_NO_GLTF_IMPORTER",
        "ASSIMP_BUILD_NO_GLTF1_IMPORTER",
        "ASSIMP_BUILD_NO_GLTF2_IMPORTER",
        "ASSIMP_BUILD_NO_HMP_IMPORTER",
        "ASSIMP_BUILD_NO_IFC_IMPORTER",
        "ASSIMP_BUILD_NO_IQM_IMPORTER",
        "ASSIMP_BUILD_NO_IRR_IMPORTER",
        "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
        "ASSIMP_BUILD_NO_LWO_IMPORTER",
        "ASSIMP_BUILD_NO_LWS_IMPORTER",
        "ASSIMP_BUILD_NO_M3D_IMPORTER",
        "ASSIMP_BUILD_NO_MD2_IMPORTER",
        "ASSIMP_BUILD_NO_MD3_IMPORTER",
        "ASSIMP_BUILD_NO_MD5_IMPORTER",
        "ASSIMP_BUILD_NO_MDC_IMPORTER",
        "ASSIMP_BUILD_NO_MDL_IMPORTER",
        "ASSIMP_BUILD_NO_MMD_IMPORTER",
        "ASSIMP_BUILD_NO_MS3D_IMPORTER",
        "ASSIMP_BUILD_NO_NDO_IMPORTER",
        "ASSIMP_BUILD_NO_NFF_IMPORTER",
        "ASSIMP_BUILD_NO_OFF_IMPORTER",
        "ASSIMP_BUILD_NO_OGRE_IMPORTER",
        "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
        "ASSIMP_BUILD_NO_PLY_IMPORTER",
        "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
        "ASSIMP_BUILD_NO_Q3D_IMPORTER",
        "ASSIMP_BUILD_NO_RAW_IMPORTER",
        "ASSIMP_BUILD_NO_SIB_IMPORTER",
        "ASSIMP_BUILD_NO_SMD_IMPORTER",
        "ASSIMP_BUILD_NO_STL_IMPORTER",
        "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
        "ASSIMP_BUILD_NO_USD_IMPORTER",
        "ASSIMP_BUILD_NO_X_IMPORTER",
        "ASSIMP_BUILD_NO_X3D_IMPORTER",
        "ASSIMP_BUILD_NO_XGL_IMPORTER",
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

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"    -- /MTd: must match the engine's CRT
        -- symbols deliberately OFF even in Debug. With them on this lib is
        -- 104MB, over GitHub's 100MB hard limit, so the repo cannot be
        -- pushed. Students debug their own game code, not assimp internals
        -- (the same reasoning that means we ship no third-party PDBs), so
        -- the symbols buy nothing. Turn this back on locally if you ever
        -- genuinely need to step into assimp.
        symbols "off"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"

