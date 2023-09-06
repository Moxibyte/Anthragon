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
    m_ioc = std::make_unique<IoCContainer>();
    loader.SetIoCContainer(m_ioc.get());
}

void Anthragon::Detail::Core::Module::OnModuleUnload()
{
    m_ioc = nullptr;
}

bool Anthragon::Detail::Core::Module::Instantiate(void)
{
    // throw std::logic_error("The method or operation is not implemented.");
    return false;
}
