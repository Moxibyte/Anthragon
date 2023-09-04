#include "StaticLoader.h"

Anthragon::Detail::StaticLoader::StaticLoader()
{
    RegisterModuleInstances(*this);
}

const Anthragon::IModule* Anthragon::Detail::StaticLoader::GetModule(std::string_view name) const
{
    IModule* module = nullptr;
    auto itFind = m_modules.find(name.data());
    if (itFind != m_modules.end())
    {
        module = itFind->second.get();
    }

    return module;
}

bool Anthragon::Detail::StaticLoader::ConfigureManager(ConfigurationKey key, std::string_view value)
{
    // The static manager does not support any of the given configuration keys
    return false;
}

bool Anthragon::Detail::StaticLoader::Load()
{
    if (!m_loaded)
    {
        // Load all modules
        for (auto& module : m_modules)
        {
            module.second->OnModuleLoad(*this);
        }
        
        m_loaded = true;
        return true;
    }
    return false;
}

void Anthragon::Detail::StaticLoader::Shutdown()
{
    if (m_loaded)
    {
        // TODO: Deactivate all exiting states

        // Unload all modules
        for (auto& module : m_modules)
        {
            module.second->OnModuleUnload();
        }

        m_loaded = false;
    }
}

void Anthragon::Detail::StaticLoader::SetIoCContainer(IInversionController* controller)
{
    if (!m_ioc)
    {
        m_ioc = controller;
    }
}

Anthragon::IoCProxy Anthragon::Detail::StaticLoader::GetIoCContainer()
{
    return IoCProxy(*m_ioc);
}

void Anthragon::Detail::StaticLoader::RegisterStaticModuleInstance(std::string_view name, std::unique_ptr<IModule>&& ptr)
{
    if (!m_loaded)
    {
        auto* module = GetModule(name);
        if (!module)
        {
            m_modules[name.data()] = std::move(ptr);
        }
    }
}

std::shared_ptr<Anthragon::IModuleManager> Anthragon::CreateModuleManager()
{
    return std::make_shared<Detail::StaticLoader>();
}
