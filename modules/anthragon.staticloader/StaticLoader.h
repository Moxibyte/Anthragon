#pragma once

#include <anthragon/IModuleManager.h>
#include <anthragon.staticloader/ModuleInstances.h>

#include <string>
#include <memory>
#include <unordered_map>

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
             * @brief 
             * @param name 
             * @param ptr 
            */
            void RegisterStaticModuleInstance(std::string_view name, std::unique_ptr<IModule>&& ptr);

        private:
            std::unordered_map<std::string, std::unique_ptr<IModule>> m_modules;
            IInversionController* m_ioc = nullptr;
            bool m_loaded = false;
    };
}

namespace Anthragon
{
    std::shared_ptr<IModuleManager> CreateModuleManager();
}

