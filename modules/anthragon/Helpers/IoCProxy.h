#pragma once

#include <anthragon/IInversionController.h>

namespace Anthragon
{
    class IoCProxy
    {
        public:
            IoCProxy() = default;
            inline IoCProxy(IInversionController& controller) :
                m_ioc(&controller)
            {}
            IoCProxy(const IoCProxy&) = default;
            IoCProxy(IoCProxy&&) noexcept = default;

            IoCProxy& operator=(const IoCProxy&) = default;
            IoCProxy& operator=(IoCProxy&&) noexcept = default;


            /*!
             * @brief Registers implementation with default constructor
             * @tparam T API to register
             * @param locator Locator to register on
            */
            template<typename T>
            void Register(const IInversionController::Locator& locator)
            {
                std::function<IInversionController::VoidPointer()> defaultFactory =
                    []()
                {
                    return std::make_shared<T>();
                };
                Register<T>(locator, defaultFactory);
            }

            /*!
             * @brief Registers implementation with specific constructors
             * @tparam API to register
             * @tparam ...Args Factory arguments
             * @param locator Locator to register on
             * @param factory Factory for object construction
            */
            template<typename T, typename... Args>
            void Register(const IInversionController::Locator& locator, const std::function<IInversionController::VoidPointer(Args...)>& factory)
            {
                m_ioc->Register(locator, &typeid(T), factory);
            }

            /*!
             * @brief Registers an existing object as a IoC singleton
             * @tparam T ABI to register
             * @param externalObject external managed object
            */
            template<typename T>
            void RegisterSingleton(const IInversionController::Locator& locator, IInversionController::Pointer<T> externalObject)
            {
                m_ioc->RegisterSingleton(locator, &typeid(T), (IInversionController::VoidPointer)externalObject);
            }

            /*!
             * @brief Registers a factory for object creation
             * @tparam T ABI to register
             * @tparam ...Args Arguments for singleton construction
             * @param locator Locator to register on
             * @param factory Factory for singleton creation
            */
            template<typename T, typename... Args>
            void RegisterSingleton(const IInversionController::Locator& locator, const std::function<IInversionController::VoidPointer(Args...)>& factory)
            {
                m_ioc->RegisterSingleton(locator, &typeid(T), std::any(factory));
            }

            /*!
             * @brief Registers an IoC redirection from source to target
             * @param source Source locator to which shall be listen to. Will always override exiting registrations!
             * @param target Target locator to which shall be redirect to. Target will undergo an normal query!
             * @return True if source existed and target could be installed.
            */
            bool RegisterRedirect(const IInversionController::Locator& source, const IInversionController::Locator& target)
            {
                return m_ioc->RegisterRedirect(source, target);
            }

            /*!
             * @brief Complex / Get construct function
             * @tparam T ABI Type to retrieve
             * @tparam ...Args Optional arguments for construction
             * @param locator Input locator to query
             * @param ...args Optional arguments for construction
             * @return Retrieve instance
            */
            template<typename T, typename... Args>
            IInversionController::Pointer<T> Get(const IInversionController::Locator& locator, Args... args)
            {
                auto invoker =
                    [&](std::any& factory)
                {
                    return std::any_cast<std::function<IInversionController::VoidPointer(Args...)>>(factory)(std::forward<Args>(args)...);
                };

                return std::static_pointer_cast<T, void>(m_ioc->Get(locator, &typeid(T), invoker));
            }

        private:
            IInversionController* m_ioc = nullptr;
    };
}
