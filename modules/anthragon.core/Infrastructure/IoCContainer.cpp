#include "IoCContainer.h"

void Anthragon::Detail::Core::IoCContainer::Register(const Locator& locator, const std::type_info* api, const std::any& factory)
{
    auto& registration = m_container[locator];
    registration.type = RegistrationType::FactoryConstruct;
    registration.api = api;
    registration.factory = factory;
}

void Anthragon::Detail::Core::IoCContainer::RegisterSingleton(const Locator& locator, const std::type_info* api, const std::any& factory)
{
    auto& registration = m_container[locator];
    registration.type = RegistrationType::FactorySingleton;
    registration.api = api;
    registration.factory = factory;
}

void Anthragon::Detail::Core::IoCContainer::RegisterSingleton(const Locator& locator, const std::type_info* api, VoidPointer externalObject)
{
    auto& registration = m_container[locator];
    registration.type = RegistrationType::ExternalSingleton;
    registration.api = api;
    registration.subject = externalObject;
}

bool Anthragon::Detail::Core::IoCContainer::RegisterRedirect(const Locator& source, const Locator& target)
{
    auto itTarget = Query(target);
    if (itTarget != m_container.end())
    {
        // Erase existing
        auto itSource = m_container.find(source); // Normal find because we are installing a "original" locator
        if (itSource != m_container.end())
        {
            m_container.erase(itSource);
        }

        // Install redirect
        auto& registration = m_container[source];
        registration.type = RegistrationType::Redirect;
        registration.redirect = target;
        registration.api = itTarget->second.api; // Serves same abi

        return true;
    }
    return false;
}

Anthragon::IInversionController::VoidPointer Anthragon::Detail::Core::IoCContainer::Get(const Locator& locator, const std::type_info* api, const Invoker& invoker)
{
    auto it = Query(locator);
    if (it != m_container.end())
    {
        auto& registration = it->second;
        if (!api || registration.api == api)
        {
            switch (registration.type)
            {
                // Simple type
                case RegistrationType::Redirect:
                    return Get(registration.redirect, api, invoker);

                case RegistrationType::ExternalSingleton:
                    return std::any_cast<VoidPointer>(registration.subject);

                // Factory based (invoker)
                case RegistrationType::FactorySingleton:
                    if (!registration.subject.has_value())
                    {
                        registration.subject = invoker(registration.factory);
                    }
                    return std::any_cast<VoidPointer>(registration.subject);

                case RegistrationType::FactoryConstruct:
                    return invoker(registration.factory);
            }
        }
    }
    return nullptr;
}

Anthragon::Detail::Core::IoCContainer::Container::iterator Anthragon::Detail::Core::IoCContainer::Query(const Locator& locator)
{
    // Step 1: Query 1:1
    Container::iterator it = m_container.find(locator);
    if (it != m_container.end())
    {
        return it;
    }

    // Step 2: Query nm/nm/* -> nm/*
    std::string l = locator;
    auto dpos = l.find_last_of('/');
    if (dpos != std::string::npos)
    {
        l = l.substr(0, dpos + 1);
        l += "*";

        while (true)
        {
            it = m_container.find(l);
            if (it != m_container.end())
            {
                return it;
            }

            l = l.substr(0, l.size() - 2);
            dpos = l.find_last_of('/');
            if (dpos == std::string::npos)
            {
                break;
            }
            l = l.substr(0, dpos + 1);
            l += "*";
        }
    }

    // Step 3: Return end
    return m_container.end();
}

Anthragon::IInversionController::VoidPointer Anthragon::Detail::Core::IoCContainer::DefaultInvoker(std::any& factory)
{
    return std::any_cast<std::function<VoidPointer()>>(factory)();
}
