#pragma once

#include <anthragon/ILibContext.h>

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

            /*!
             * @brief Retrieve a description from the module
             * @return Reference to module
            */
            virtual const ModuleDescription& GetDescription() const = 0;
            
            /*!
             * @brief Called when the module is initialized 
             * 
             * THIS IS CALLED ONCE PER APP
             * @param loader Module manager for callback setup
            */
            virtual void OnModuleLoad(IModuleManager& loader) = 0;
            
            /*!
             * @brief Called when the module is unloaded
             * 
             * THIS IS CALLED ONCE PER APP
            */
            virtual void OnModuleUnload() = 0;

            /*!
             * @brief Called when initialized
             * 
             * the module can't assume that systems are initialized now
             * @param ioc Inversion of control container
            */
            virtual void Init(ILibContext& ioc) = 0;
            
            /*!
             * @brief Called after init 
             * 
             * The module can assume that all subsystems are initialized
             * @param ioc Inversion of control container
            */
            virtual void PostInit(ILibContext& ioc) = 0;
            
            /*!
             * @brief Called on shutdown
             * 
             * This functions shall not destroy instance and only release references
             * @param ioc Inversion of control container
            */
            virtual void Shutdown(ILibContext& ioc) = 0;
            
            /*!
             * @brief Called directly after shutdown.
             * 
             * The module can expect that no external references to the internal objects are hold
             * @param ioc Inversion of control container
            */
            virtual void PostShutdown(ILibContext& ioc) = 0;
    };
}
