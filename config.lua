--  ==========================================================================
--  Anthragon project configuration file
--  This file gives you access to all relevant configuration values
--  ==========================================================================

-- Defines how modules shall be produced and consumed. Valid options are:
-- 1) "static"  - Modules are compiled into a static lib
-- 2) "shared"  - Modules are compiled into a dynamic lib (DLL)
ANTHRAGON_MODULE_TYPE = "static"

-- This array defines which modules are build
-- You can disable (Comment out with "--") modules you don't want or need
ANTHRAGON_BUILD_MODULES = {
    -- Anthragon core services
    "anthragon",                -- API
    "anthragon.core",           -- Logic
    "anthragon.staticloader",   -- Static lib module loader
    
    -- All platformes format support modules
    -- "anthragon.freetype",       -- Fonts
    -- "anthragon.image.jpg",      -- Image/jpg
    -- "anthragon.image.png",      -- Image/png

    -- Windows platform modules
    -- "anthragon.windows",        -- Windows OS abstraction
    -- "anthragon.dllloader"       -- Windows DLL Module loader
    -- "anthragon.d3d12",          -- DirectX 12 renderer
    -- "anthragon.image.wic",      -- Windows image component picture loader
}

