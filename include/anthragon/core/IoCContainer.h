#pragma once

#include <anthragon/abi/Anthragon.h>

#include <any>
#include <string>
#include <memory>
#include <typeinfo>
#include <filesystem>
#include <functional>
#include <unordered_map>

namespace ANT_NAMESPACE::core
{
    /*!
     * @brief Path based locator for the IoC container 
    */
    using IoCLocator = std::filesystem::path;

    /*!
     * @brief The IoC container is used to register ABI implementations
    */
    class IoCContainer
    {
        public:
            /*!
             * @brief How the registration is to be interpreted (Factory, singleton, etc.)
            */
            enum class RegistrationType
            {
                /*!
                 * @brief The registration is invalid
                */
                Null = 0, 

                /*!
                 * @brief The registered implementation is an external T* 
                */
                ExternalSingleton,

                /*!
                 * @brief The registered implementation is a factory for creating a single instance Singleton
                */
                FactorySingleton,

                /*!
                 * @brief The registered implementation is a factory for creating objects
                */
                FactoryConstruct,

                /*!
                 * @brief The registered entity redirects to another implementation
                */
                Redirect,
            };

        private:
            /*!
             * @brief IoC registration entry
            */
            struct Registration
            {
                /*!
                 * @brief Indicates how to interpret the stored content
                */
                RegistrationType type = RegistrationType::Null;

                /*!
                 * @brief Type of the ABI that the function implements
                */
                const std::type_info* abi = nullptr;

                /*!
                 * @brief Registration to redirect to on request. Only valid when "type" = "RegistrationType::Redirect"
                */
                IoCLocator redirect;

                /*!
                 * @brief Registration subject. "Pointer to instance" as std::shared_ptr<void>. Only valid when 
                 * "type" = "RegistrationType::ExternalSigelton" or "RegistrationType::FactorySingelton"
                */
                std::any subject;

                /*!
                 * @brief Factory to create subject / instance. As std::function<std::shared_ptr<void>()> Only valid 
                 * when "type" = "RegistrationType::FactorySingelton" or "type" = "RegistrationType::FactoryConstruct"
                */
                std::any factory;
            };

            /*!
             * @brief Inner container type
            */
            using Container = std::unordered_map<IoCLocator, Registration>;

            /*!
             * @brief An invoker is used as a proxy between IoC containers implementation (static c++)
             * And template generative calls to IoC containers Get().
            */
            using Invoker = std::function<std::shared_ptr<void>(std::any& factory)>;

        public:
            /*!
             * @brief Registers implementation with default constructor
             * @tparam T ABI to register 
             * @param locator Locator to register on
            */
            template<typename T>
            void Register(const IoCLocator& locator)
            {
                std::function<std::shared_ptr<void>()> defaultFactory =
                    []()
                    {
                        return std::make_shared<T>();
                    };
                Register<T>(locator, defaultFactory);
            }

            /*!
             * @brief Registers implementation with specific constructors
             * @tparam T ABI to register
             * @tparam ...Args Factory arguments
             * @param locator Locator to register on
             * @param factory Factory for object construction
            */
            template<typename T, typename... Args>
            void Register(const IoCLocator& locator, const std::function<std::shared_ptr<void>(Args...)>& factory)
            {
                auto& registration = m_container[locator];
                registration.type = RegistrationType::FactoryConstruct;
                registration.abi = &typeid(T);
                registration.factory = factory;
            }

            /*!
             * @brief Registers an existing object as a IoC singleton
             * @tparam T ABI to register
             * @param externalObject external managed object
            */
            template<typename T>
            void RegisterSingleton(const IoCLocator& locator, std::shared_ptr<T> externalObject)
            {
                auto& registration = m_container[locator];
                registration.type = RegistrationType::ExternalSingleton;
                registration.abi = &typeid(T);
                registration.subject = std::static_pointer_cast<void, T>(externalObject);
            }

            /*!
             * @brief Registers a factory for object creation
             * @tparam T ABI to register
             * @tparam ...Args Arguments for singleton construction
             * @param locator Locator to register on
             * @param factory Factory for singleton creation
            */
            template<typename T, typename... Args>
            void RegisterSingleton(const IoCLocator& locator, const std::function<std::shared_ptr<void>(Args...)>& factory)
            {
                auto& registration = m_container[locator];
                registration.type = RegistrationType::FactorySingleton;
                registration.abi = &typeid(T);
                registration.factory = factory;
            }

            /*!
             * @brief Registers an IoC redirection from source to target
             * @param source Source locator to which shall be listen to. Will always override exiting registrations!
             * @param target Target locator to which shall be redirect to. Target will undergo an normal query!
             * @return True if source existed and target could be installed. 
            */
            bool RegisterRedirect(const IoCLocator& source, const IoCLocator& target);

            /*!
             * @brief Complex / Get construct function
             * @tparam T ABI Type to retrieve
             * @tparam ...Args Optional arguments for construction
             * @param locator Input locator to query
             * @param ...args Optional arguments for construction
             * @return Retrieve instance
            */
            template<typename T, typename... Args>
            std::shared_ptr<T> Get(const IoCLocator& locator, Args... args)
            {
                auto invoker = 
                    [&](std::any& factory)
                    {
                        return std::any_cast<std::function<std::shared_ptr<void>(Args...)>>(factory)(std::forward<Args>(args)...);
                    };

                RegistrationType h;
                auto rv = std::static_pointer_cast<T, void>(Get(locator, &typeid(T), h, invoker));
                return rv;
            }

            /*!
             * @brief Complex / Get construct function
             * @tparam T ABI Type to retrieve
             * @tparam ...Args Optional arguments for construction
             * @param locator Input locator to query
             * @param ...args Optional arguments for construction
             * @param hint Optional pointer to output hint (how object was retrieved)
             * @return Retrieve instance
            */
            template<typename T, typename... Args>
            std::shared_ptr<T> Get(const IoCLocator& locator, RegistrationType* hint, Args... args)
            {
                auto invoker =
                    [&](std::any& factory)
                {
                    return std::any_cast<std::function<std::shared_ptr<void>(Args...)>>(factory)(std::forward<Args>(args)...);
                };

                RegistrationType h;
                auto rv = std::static_pointer_cast<T, void>(Get(locator, &typeid(T), h, invoker));
                if (hint)
                {
                    *hint = h;
                }
                return rv;
            }

            /*!
             * @brief Most simple get (only use locator)
             * 
             * This function only works on objects that are already constructed or don't need parameters for construction
             * @param locator Input locator to query
             * @param hint Optional pointer to output hint (how object was retrieved)
             * @return 
            */
            std::shared_ptr<void> Get(const IoCLocator& locator, RegistrationType* hint = nullptr);

        private:
            /*!
             * @brief Will walk the IoC container by given locator (and abi) to find matching entry
             * @param locator Input locator
             * @param abi ABI to check entry against. Can be null -> checks will skipp
             * @param hint Output hint to indicate registration type
             * @param invoker Invoker proxy for factories
             * @return Shared pointer as void or nullptr
            */
            std::shared_ptr<void> Get(const IoCLocator& locator, const std::type_info* abi, RegistrationType& hint, const Invoker& invoker);

            /*!
             * @brief Iteratively query's the container for a matching entry
             * @param locator Input locator (Will query as is plus all "nm/.../*" combinations
             * @return Element iterator or end
            */
            Container::iterator Query(const IoCLocator& locator);

            /*!
             * @brief Default invoker implementation 
             * 
             * (direct forward to parameterless factory)
             * @param factory Factory to forward
             * @return Pointer to instance
            */
            static std::shared_ptr<void> DefaultInvoker(std::any& factory);

        private:
            /*!
             * @brief Container instance
            */
            Container m_container;
    };
}
