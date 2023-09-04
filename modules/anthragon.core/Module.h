#pragma once

#include <anthragon/IModule.h>
#include <anthragon/IModuleManager.h>

#include <anthragon.core/Infrastructure/IoCContainer.h>

#include <memory>

namespace Anthragon::Detail::Core
{
    class Module : public IModule
    {
        public:
            const ModuleDescription& GetDescription() const override;
            void OnModuleLoad(IModuleManager& loader) override;
            void OnModuleUnload() override;
            bool Instantiate(void) override;

        private:
            std::unique_ptr<IoCContainer> m_ioc = nullptr;
    };
}
