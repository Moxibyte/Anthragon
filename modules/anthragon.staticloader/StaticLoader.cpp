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
    if (!m_loaded && ResolveLoadOrder())
    {
        // Load all modules
        for (auto& module : m_modules)
        {
            module.second->OnModuleLoad(*this);
        }

        // TODO: Activate modules

        m_loaded = true;
        return true;
    }
    return false;
}

void Anthragon::Detail::StaticLoader::Shutdown()
{
    if (m_loaded)
    {
        // TODO: Deactivate modules

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

void Anthragon::Detail::StaticLoader::RegisterStaticModuleInstance(std::string_view name, std::shared_ptr<IModule>&& ptr)
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

bool Anthragon::Detail::StaticLoader::ResolveLoadOrder()
{
    // Define temporary container
    struct RevolveMeta
    {
        std::string moduleName;
        std::shared_ptr<IModule> instance;
        std::vector<std::string> dependencies;
    };

    // Move data to temp
    std::vector<RevolveMeta> meta;
    for (auto& module : m_modules)
    {
        RevolveMeta data;
        data.moduleName = module.first;
        data.instance = module.second;
        const char*const* deps = data.instance->GetDescription().dependencies;
        while (*deps)
        {
            data.dependencies.push_back(*deps);
            deps++;
        }

        meta.push_back(std::move(data));
    }
    m_modules.clear();

    // Resolve one at a time
    size_t lockCounter = meta.size();
    while (!meta.empty())
    {
        for (auto it = meta.begin(); it != meta.end(); ++it)
        {
            // Check for empty deps
            if (it->dependencies.empty())
            {
                // Resolve this
                m_modules.emplace(std::pair<std::string, std::shared_ptr<IModule>>(it->moduleName, std::move(it->instance)));

                // Clear dependencies
                for (auto& dep : meta)
                {
                    auto dit = std::find(dep.dependencies.begin(), dep.dependencies.end(), it->moduleName);
                    if (dit != dep.dependencies.end())
                    {
                        dep.dependencies.erase(dit);
                    }
                }

                meta.erase(it);
                break;
            }
        }
        
        if (lockCounter == meta.size())
        {
            return false;
        }
        lockCounter = meta.size();
    }

    return true;
}

std::shared_ptr<Anthragon::IModuleManager> Anthragon::CreateModuleManager()
{
    return std::make_shared<Detail::StaticLoader>();
}
