-- Configuration
ANTHRAGON_PREFIX = "ANT_"

-- OS discovery
function anthragon_is_windows()
    ds = package.config:sub(1,1)
    return ds == "\\"
end

-- Subfolder discovery
function anthragon_discover_subfolders(folder)
    if anthragon_is_windows() then
        return anthragon_discover_subfolders_win(folder)
    else
        return anthragon_discover_subfolders_linux(folder)
    end
end
function anthragon_discover_subfolders_win(folder)
    return io.popen("dir \"" .. folder .. "\" /b /ad"):lines()
end
function anthragon_discover_subfolders_linux(folder)
    return io.popen("find \"./" .. folder .. "\" -maxdepth 1 -type d -printf '%f\n'" ):lines()
end

-- Project functions
function anthragon_new(name)
    anthragon_X_name = name

    project(name)
        -- Project setup
        location "./"
        targetname(name)
        targetsuffix ""
        language "C++"
        cppdialect "C++20"

        -- Target directories
        targetdir "%{wks.location}/build/%{cfg.architecture}-%{cfg.buildcfg}/bin/"
        objdir    "%{wks.location}/build/%{cfg.architecture}-%{cfg.buildcfg}/obj/%{prj.name}/"
        debugdir  "%{wks.location}/examples"

        -- Files and include
        files {
            "%{prj.location}/**.lua",
            "%{prj.location}/**.txt", "%{prj.location}/**.md",
            "%{prj.location}/**.c", "%{prj.location}/**.cc", "%{prj.location}/**.cpp", "%{prj.location}/**.cxx",
            "%{prj.location}/**.h", "%{prj.location}/**.hh", "%{prj.location}/**.hpp", "%{prj.location}/**.hxx"
        }
        includedirs {
            "%{prj.location}",
            "%{wks.location}/modules",
        }

        -- Configuration based define
        filter { "configurations:Debug" }
            defines {
                "DEBUG",
                "_DEBUG",
                ANTHRAGON_PREFIX .. "DEBUG",
            } 
            symbols "On"
        filter {}
        filter { "configurations:Release" }
            defines {
                "NDEBUG",
                ANTHRAGON_PREFIX .. "NDEBUG",
            } 
            optimize "On"
        filter {}

        -- General defines
        defines {
            ANTHRAGON_PREFIX .. "BUILD",
            ANTHRAGON_PREFIX .. "BUILD_" .. name:upper():gsub("%.", "_"),
        }

        -- OS defines
        filter { "system:windows" }
            defines {
                ANTHRAGON_PREFIX .. "OS_WINDOWS",
                "WINVER=0x0A00",
                "_WIN32_WINNT=0x0A00",
            }
        filter {}
        filter { "system:unix" }
            defines {
                ANTHRAGON_PREFIX .. "OS_LINUX",
            }
        filter {}
            
        -- Conan includes
        conan_setup_build()
end

function anthragon_headers()
    kind "None"
end

function anthragon_static()
    kind "StaticLib"
end

function anthragon_module()
    -- Configuration type
    if ANTHRAGON_MODULE_TYPE == "static" then
        kind "StaticLib"
    else
        kind "SharedLib"
    end

    -- Add to modules
    table.insert(anthragon_X_mods, anthragon_X_name)
end

function anthragon_example()
    -- Configuration type
    kind "ConsoleApp"

    -- Link to anthragon libs
    if ANTHRAGON_MODULE_TYPE == "static" then
        anthragon_X_func = links
    else
        anthragon_X_func = dependson
    end
    for i,mod in ipairs(anthragon_X_mods)
    do
        anthragon_X_func({ mod })
    end

    -- Link to loader
    if ANTHRAGON_MODULE_TYPE == "static" then
        links { "anthragon.staticloader" }
    end

    -- Link to conan libs
    for i,dep in ipairs(anthragon_X_deps)
    do
        filter { "configurations:Debug" }
            conan_setup_link("debug_x86_64", dep)
        filter {}
        filter { "configurations:Release" }
            conan_setup_link("release_x86_64", dep)
        filter {}
    end
end

function anthragon_require(dependencie)
    requires = true
    for i,v in ipairs(anthragon_X_deps) do
        if v == dependencie then
            requires = false
        end
    end

    if requires then
        table.insert(anthragon_X_deps, dependencie)
    end
end
