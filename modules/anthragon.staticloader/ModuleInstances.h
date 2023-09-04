#pragma once

namespace Anthragon::Detail
{
    class StaticLoader;

    /*!
     * @brief Registers all module instances within a static module loader
     * @param loader Reference to loader
    */
    void RegisterModuleInstances(StaticLoader& loader);
}
