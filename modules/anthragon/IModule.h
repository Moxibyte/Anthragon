#pragma once

#include <cstdint>

namespace Anthragon
{
    class IModuleManager;

    /*!
     * @brief Module version encoded as a 32-Bit value or as 4 8-Bit octets
    */
    struct ModuleVersion
    {
        union
        {
            uint32_t quad;
            struct
            {
                uint8_t major;
                uint8_t minor;
                uint8_t patch;
                uint8_t hotfix;
            };
        };
    };

    /*!
     * @brief Metadata about a module
    */
    struct ModuleDescription
    {
        const char* name;
        const char* author;
        const char* description;
        ModuleVersion version;
        const char* const* dependencies;
    };

    /*!
     * @brief Module interface
    */
    class IModule
    {
        public:
            ~IModule() = default;

            virtual const ModuleDescription& GetDescription() const = 0;
            virtual void OnModuleLoad(IModuleManager& loader) = 0;
            virtual void OnModuleUnload() = 0;
            virtual bool Instantiate(void) = 0;
    };
}
