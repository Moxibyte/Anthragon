#include "LibContext.h"

Anthragon::Detail::Core::LibContext::LibContext(IModuleManager& manager)
{
    auto libAllocateAx = manager.GetFunction(IModuleManager::ModuleFunctionType::MemoryAllocate);
    if (libAllocateAx.has_value())
    {
        m_libAllocate = std::any_cast<IModuleManager::FMemoryAllocate>(libAllocateAx);
    }
    else
    {
        m_libAllocate = malloc;
    }

    auto libReAllocateAx = manager.GetFunction(IModuleManager::ModuleFunctionType::MemoryReAllocate);
    if (libReAllocateAx.has_value())
    {
        m_libReAllocate = std::any_cast<IModuleManager::FMemoryReAllocate>(libReAllocateAx);
    }
    else
    {
        m_libFree = free;
    }

    auto libFreeAx = manager.GetFunction(IModuleManager::ModuleFunctionType::MemoryFree);
    if (libFreeAx.has_value())
    {
        m_libFree = std::any_cast<IModuleManager::FMemoryFree>(libFreeAx);
    }
    else
    {
        m_libReAllocate = realloc;
    }
}

void Anthragon::Detail::Core::LibContext::Destroy()
{
    this->~LibContext();
    m_libFree(this);
}

Anthragon::IoCProxy Anthragon::Detail::Core::LibContext::GetIoCContainer()
{
    return IoCProxy(m_ioc);
}

void* Anthragon::Detail::Core::LibContext::LibAllocate(size_t size)
{
    return m_libAllocate(size);
}

void* Anthragon::Detail::Core::LibContext::LibReAllocate(void* buffer, size_t size)
{
    return m_libReAllocate(buffer, size);
}

void Anthragon::Detail::Core::LibContext::LibFree(void* buffer)
{
    m_libFree(buffer);
}
