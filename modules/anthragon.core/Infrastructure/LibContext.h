#pragma once

#include <anthragon/ILibContext.h>
#include <anthragon/IModuleManager.h>

#include <anthragon.core/Infrastructure/IoCContainer.h>

#include <functional>

namespace Anthragon::Detail::Core
{
    class LibContext : public ILibContext
    {
        public:
            LibContext() = delete;
            LibContext(const LibContext&) = delete;
            LibContext(IModuleManager& manager);
            
            void Destroy();

            LibContext& operator=(const LibContext&) = delete;

            IoCProxy GetIoCContainer() override;

            void* LibAllocate(size_t size) override;
            void* LibReAllocate(void* buffer, size_t size) override;
            void LibFree(void* buffer) override;

        private:
            IoCContainer m_ioc;

            std::function<void*(size_t)> m_libAllocate;
            std::function<void*(void*, size_t)> m_libReAllocate;
            std::function<void(void*)> m_libFree;
    };
}
