#pragma once

#include <anthragon/Helpers/IoCProxy.h>

namespace Anthragon
{
    /*!
     * @brief State full control flow container per lib instance
    */
    class ILibContext
    {
        public:
            virtual ~ILibContext() = default;

            /*!
             * @brief Destroys the instance
            */
            virtual void Destroy() = 0;

            /*!
             * @brief Retrieval function for the IoC container
             * @return Proxy for accessing the IoC-Container
            */
            virtual IoCProxy GetIoCContainer() = 0;

            /*!
             * @brief Lib level malloc() function
             * @param size Size of the memory block to allocate
             * @return Memory buffer of size
            */
            virtual void* LibAllocate(size_t size) = 0;

            /*!
             * @brief Lib level realloc() function
             * @param buffer Input buffer
             * @param size New size
             * @return Output buffer of new size
            */
            virtual void* LibReAllocate(void* buffer, size_t size) = 0;

            /*!
             * @brief Lib level free() function
             * @param buffer 
            */
            virtual void LibFree(void* buffer) = 0;

    };
}
