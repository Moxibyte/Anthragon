#pragma once

#include <anthragon/IModule.h>
#include <anthragon/IModuleManager.h>

#include <anthragon.core/Infrastructure/LibContext.h>
#include <anthragon.core/Infrastructure/IoCContainer.h>

#include <memory>

namespace Anthragon::Detail::Core
{
    /*!
     * @brief Core module
     * 
     * Core implements IoC, Flow and Logic
    */
    class Module : public IModule
    {
        public:
            const ModuleDescription& GetDescription() const override;
            void OnModuleLoad(IModuleManager& loader) override;
            void OnModuleUnload() override;

            void Init(ILibContext& ioctxc) override;
            void PostInit(ILibContext& ctx) override;
            void Shutdown(ILibContext& ctx) override;
            void PostShutdown(ILibContext& ctx) override;

    };
}
