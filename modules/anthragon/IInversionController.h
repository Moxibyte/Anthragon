#pragma once

#include <any>
#include <memory>
#include <string>
#include <functional>

namespace Anthragon
{
    /*!
     * @brief Controller class for inversion of control
    */
    class IInversionController
    {
        public:
            template<typename T>
            using Pointer = std::shared_ptr<T>;
            using VoidPointer = Pointer<void>;

            using Locator = std::string;
            using Invoker = std::function<VoidPointer(std::any& factory)>;

        public:
            virtual ~IInversionController() = default;
            
            /*!
             * @brief Registers a factory for object creation
             * @param locator Name of registration endpoint
             * @param api Object implementing Interface
             * @param factory Factory function encoded as any
            */
            virtual void Register(const Locator& locator, const std::type_info* api, const std::any& factory) = 0;

            /*!
             * @brief Registers a factory for singleton creation
             * @param locator Name of registration endpoint
             * @param api Object implementing Interface
             * @param factory Factory function encoded as any
            */
            virtual void RegisterSingleton(const Locator& locator, const std::type_info* api, const std::any& factory) = 0;

            /*!
             * @brief Registers an external singleton
             * @param locator Name of registration endpoint
             * @param api Object implementing Interface
             * @param externalObject Pointer to external singleton
            */
            virtual void RegisterSingleton(const Locator& locator, const std::type_info* api, VoidPointer externalObject) = 0;

            /*!
             * @brief Resisters a redirection from source to target
             * @param source Source locator to which shall be listen to. Will always override exiting registrations!
             * @param target Target locator to which shall be redirect to. Target will undergo an normal query!
             * @return True if source existed and target could be installed. 
            */
            virtual bool RegisterRedirect(const Locator& source, const Locator& target) = 0;

            /*!
             * @brief IoC retrieval interface
             * @param locator Name of registration endpoint
             * @param api Interface that shall be retrieved
             * @param invoker Invoker proxy for factories
             * @return Shared pointer as void or nullptr
            */
            virtual VoidPointer Get(const Locator& locator, const std::type_info* api, const Invoker& invoker) = 0;
    };
}
