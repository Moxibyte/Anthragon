#include "anthragon/core/IoCContainer.h"

std::shared_ptr<void> ANT_NAMESPACE::core::IoCContainer::Get(const IoCLocator& locator, RegistrationType* hint /*= nullptr*/)
{
    RegistrationType h;
    auto rv = Get(locator, nullptr, h, &IoCContainer::DefaultInvoker);
    if (hint)
    {
        *hint = h;
    }
    return rv;
}

std::shared_ptr<void> ANT_NAMESPACE::core::IoCContainer::Get(const IoCLocator& locator, const std::type_info* abi, RegistrationType& hint, const Invoker& invoker)
{
    hint = RegistrationType::Null;

    auto it = Query(locator);
    if (it != m_container.end())
    {
        auto& registration = it->second;
        if (!abi || registration.abi == abi)
        {
            switch (registration.type)
            {
                // Simple type
                case RegistrationType::Redirect:
                    return Get(registration.redirect, abi, hint, invoker);

                case RegistrationType::ExternalSingleton:
                    hint = RegistrationType::ExternalSingleton;
                    return std::any_cast<std::shared_ptr<void>>(registration.subject);

                // Factory based (invoker)
                case RegistrationType::FactorySingleton:
                    hint = RegistrationType::FactorySingleton;
                    if (!registration.subject.has_value())
                    {
                        registration.subject = invoker(registration.factory);
                    }
                    return std::any_cast<std::shared_ptr<void>>(registration.subject);

                case RegistrationType::FactoryConstruct:
                    hint = RegistrationType::FactoryConstruct;
                    return invoker(registration.factory);
            }
        }
    }
    return nullptr;
}

ANT_NAMESPACE::core::IoCContainer::Container::iterator ANT_NAMESPACE::core::IoCContainer::Query(const IoCLocator& locator)
{
    // Step 1: Query 1:1
    Container::iterator it = m_container.find(locator);
    if (it != m_container.end())
    {
        return it;
    }

    // Step 2: Query nm/nm/* -> nm/*
    std::string l = locator.string();
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

std::shared_ptr<void> ANT_NAMESPACE::core::IoCContainer::DefaultInvoker(std::any& factory)
{
    return std::any_cast<std::function<std::shared_ptr<void>()>>(factory)();
}

bool ANT_NAMESPACE::core::IoCContainer::RegisterRedirect(const IoCLocator& source, const IoCLocator& target)
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
        registration.abi = itTarget->second.abi; // Serves same abi

        return true;
    }
    return false;
}
