#pragma once

#include <anthragon/IModuleManager.h>
#include <anthragon.staticloader/ModuleInstances.h>

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace Anthragon::Detail
{
    /*!
     * @brief Loads module defined in static library
    */
    class StaticLoader : public IModuleManager
    {
        public:
            StaticLoader();

            const IModule* GetModule(std::string_view name) const override;
            bool ConfigureManager(ConfigurationKey key, std::string_view value) override;
            bool Load() override;
            void Shutdown() override;
            void SetIoCContainer(IInversionController* controller) override;
            IoCProxy GetIoCContainer() override;

            /*!
             * @brief Registers a shared pointer (as static instance) to this loader
             * @param name Name of thew module
             * @param ptr Pointer to module
            */
            void RegisterStaticModuleInstance(std::string_view name, std::shared_ptr<IModule>&& ptr);

        private:
            /*!
             * @brief Resolves dependencies (internally orders m_modules)
             * @return True if dependencies where resolvable 
            */
            bool ResolveLoadOrder();

        private:
            std::map<std::string, std::shared_ptr<IModule>> m_modules;
            IInversionController* m_ioc = nullptr;
            bool m_loaded = false;
    };
}

namespace Anthragon
{
    std::shared_ptr<IModuleManager> CreateModuleManager();
}

