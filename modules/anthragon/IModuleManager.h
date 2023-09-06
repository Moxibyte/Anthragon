#pragma once

#include <anthragon/IModule.h>
#include <anthragon/IInversionController.h>
#include <anthragon/Helpers/IoCProxy.h>

#include <string_view>

namespace Anthragon
{
    /*!
     * @brief API For module manager / loader
     * 
     * Managers are responsible from module loading, activation and shutdown
    */
    class IModuleManager
    {
        public:
            enum class ConfigurationKey
            {
                /*!
                 * @brief Mode of dynamic module loading
                 * 
                 * Possible values:
                 * "dir"        : Loads modules from directory (CFKey_DynamicModuleDir or current binary path)
                 * "manual"     : Loads modules manually (Set CFKey_DynamicLoadNow multiple time. One time for each module)
                */
                CFKey_DynamicMode = 1,

                /*!
                 * @brief Directory from which dynamic modules shall be loaded
                */
                CFKey_DynamicModuleDir = 2,
            
                /*!
                 * @brief Module from dir loading behavior
                 * 
                 * Possible values:
                 * "strict"     : Only the specified directory will be used for module loading
                 * "relaxed"    : If loading from module dir failles the default system loading path will be traversed
                 * "reverse"    : Same as relaxed but system dirs will be checked first
                */
                CFKey_DynamicDirMode = 3,

                /*!
                 * @brief Every set call on this configuration key will add the specified module to the loading list
                 * This will only work if "CFKey_DynamicMode" is on "manual". A full qualified path is required!
                 * Does not actually load the module!
                */
                CFKey_DynamicLoadNow = 1001,
            };

        public:
            ~IModuleManager() = default;

            /*!
             * @brief Retrieves a loaded module from the module managers registry
             * @param name Name of the module to lookup
             * @return Pointer to module implementation
            */
            virtual const IModule* GetModule(std::string_view name) const = 0;
            
            /*!
             * @brief Configures the manager
             * @param key Property to set (One of the specified above)
             * @param value Depends on key
             * @return True if configuration value could be changed
            */
            virtual bool ConfigureManager(ConfigurationKey key, std::string_view value) = 0;

            /*!
             * @brief Loads modules 
             * @return True on success
            */
            virtual bool Load() = 0;
            
            /*!
             * @brief Initializes module shutdown and unloads modules
            */
            virtual void Shutdown() = 0;

            /*!
             * @brief Sets the inversion of control container (can only be called once)
             * @param controller IoC Container
            */
            virtual void SetIoCContainer(IInversionController* controller) = 0;
            
            /*!
             * @brief Retrieves the inversion of control container
             * @return IoC Container
            */
            virtual IoCProxy GetIoCContainer() = 0;
    };

    /*!
     * @brief Create an instance of the module manager
     * @return Pointer to module manager
    */
    std::shared_ptr<IModuleManager> CreateModuleManager();
}
