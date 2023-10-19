#include "Module.h"

const Anthragon::ModuleDescription& Anthragon::Detail::Core::Module::GetDescription() const
{
    const char* dependencies[] = {
        nullptr,
    };

    static ModuleDescription description{
        .name = "anthragon.core",
        .author = "Moxibyte GmbH",
        .description = "Core logic of anthragon",
        .version{
            .major  = 1,
            .minor  = 0,
            .patch  = 0,
            .hotfix = 0,
        },
        .dependencies = dependencies
    };
    return description;
}

void Anthragon::Detail::Core::Module::OnModuleLoad(IModuleManager& loader)
{
    IModuleManager::FCreateContext fCreateContext =
        [](IModuleManager* manager)
        {
            ILibContext* ctx = nullptr;
            auto libAllocAx = manager->GetFunction(IModuleManager::ModuleFunctionType::MemoryAllocate);
            if (libAllocAx.has_value())
            {
                ctx = (ILibContext*)std::any_cast<std::function<void* (size_t)>>(libAllocAx)(sizeof(LibContext));
            }
            else
            {
                ctx = (ILibContext*)malloc(sizeof(LibContext));
            }
            new((LibContext*)ctx)LibContext(*manager);
            return ctx;
        }
    ;
    loader.SetFunction(IModuleManager::ModuleFunctionType::CreateContext, fCreateContext);
}

void Anthragon::Detail::Core::Module::OnModuleUnload()
{

}

void Anthragon::Detail::Core::Module::Init(ILibContext& ctx)
{
    
}

void Anthragon::Detail::Core::Module::PostInit(ILibContext& ctx)
{
    
}

void Anthragon::Detail::Core::Module::Shutdown(ILibContext& ctx)
{
    
}

void Anthragon::Detail::Core::Module::PostShutdown(ILibContext& ctx)
{
    
}
