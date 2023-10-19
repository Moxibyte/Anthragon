#pragma once

#include <anthragon/IModule.h>
#include <anthragon/ILibContext.h>
#include <anthragon/IInversionController.h>

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
            /*!
             * @brief Make of key to configure
            */
            enum class ConfigurationKey
            {
                /*!
                 * @brief Mode of dynamic module loading
                 * 
                 * Possible values:
                 * "dir"        : Loads modules from directory (DynamicModuleDir or current binary path)
                 * "manual"     : Loads modules manually (Set DynamicLoadNow multiple time. One time for each module)
                */
                DynamicMode = 1,

                /*!
                 * @brief Directory from which dynamic modules shall be loaded
                */
                DynamicModuleDir = 2,
            
                /*!
                 * @brief Module from dir loading behavior
                 * 
                 * Possible values:
                 * "strict"     : Only the specified directory will be used for module loading
                 * "relaxed"    : If loading from module dir failles the default system loading path will be traversed
                 * "reverse"    : Same as relaxed but system dirs will be checked first
                */
                DynamicDirMode = 3,

                /*!
                 * @brief Every set call on this configuration key will add the specified module to the loading list
                 * This will only work if "CDynamicMode" is on "manual". A full qualified path is required!
                 * Does not actually load the module!
                */
                DynamicLoadNow = 1001,
            };

            /*!
             * @brief Type of function supported by module
            */
            enum class ModuleFunctionType
            {
                /*!
                 * @brief Function for anthragon context creation
                 * 
                 * Type: std::function<ILibContext*(IModuleManager*)>
                 * 
                 * THIS IS NOT INITEDED TO BE IMPLEMENTED BY ANYBODY ELSE
                 * THAN THE CORE ITSELF
                */
                CreateContext = 0,

                /*!
                 * @brief Memory allocation callback
                 * 
                 * Type: std::function<void*(size_t)>
                */
                MemoryAllocate = 2,

                /*!
                 * @brief Memory re allocation callback
                 * 
                 * Type: std::function<void*(void*, size_t)>
                */
                MemoryReAllocate = 3,

                /*!
                 * @brief Memory freeing callback
                 * 
                 * Type: std::function<void(void*)>
                */
                MemoryFree = 4,
            };

            /*!
             * @brief Module functions are std::function<R(V..)> values encoded in std::any
            */
            using ModuleFunction = std::any;

            using FCreateContext = std::function<ILibContext*(IModuleManager*)>;
            using FMemoryAllocate = std::function<void*(size_t)>;
            using FMemoryReAllocate = std::function<void*(void*, size_t)>;
            using FMemoryFree = std::function<void(void*)>;

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
            virtual void Unload() = 0;
            
            /*!
             * @brief Sets a function
             * @param type 
             * @param function 
            */
            virtual void SetFunction(ModuleFunctionType type, const ModuleFunction& function) = 0;

            /*!
             * @brief 
             * @param type 
             * @return 
            */
            virtual ModuleFunction GetFunction(ModuleFunctionType type) = 0;

            /*!
             * @brief Creates a new library context
             * @return Pointer to context
            */
            virtual ILibContext* CreateContext() = 0;
    };

    /*!
     * @brief Retrieves an instance of the static module manager
     * @return Pointer to module manager
    */
    IModuleManager& GetModuleManager();
}
