include "../dependencies/conandeps.premake5.lua"
include "../config.lua"
include "anthragon.lua"

workspace "Anthragon"
    configurations { "Debug", "Release" }
    architecture "x64"
    location "../"

    -- Startup
    startproject "minimal"

    -- Global defines
    for i,mod in ipairs(ANTHRAGON_BUILD_MODULES)
    do
        defines { ANTHRAGON_PREFIX .. "HAS_" .. mod:gsub("%.", "_"):upper() }
    end

    -- Include all modules
    group "Modules"
    anthragon_X_deps = {}
    anthragon_X_mods = {}
    for i,mod in ipairs(ANTHRAGON_BUILD_MODULES)
    do
        local module_dir = "../modules/" .. mod .. "/"
        local module_file = module_dir .. "build.lua"
        if os.isfile(module_file) then
            include(module_file)
        end
    end

    -- Include example
    group "Examples"
    for example in anthragon_discover_subfolders("../examples")
    do
        local example_dir = "../examples/" .. example .. "/"
        local example_file = example_dir .. "build.lua"
        if os.isfile(example_file) then
            include(example_file)
        end
    end
