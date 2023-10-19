#pragma once

#include <anthragon/IInversionController.h>

#include <unordered_map>

namespace Anthragon::Detail::Core
{
    /*!
     * @brief IoC Implementation
    */
    class IoCContainer : public IInversionController
    {
        private:
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
                 * @brief Type of the API that the function implements
                */
                const std::type_info* api = nullptr;

                /*!
                 * @brief Registration to redirect to on request. Only valid when "type" = "RegistrationType::Redirect"
                */
                Locator redirect;

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
            using Container = std::unordered_map<Locator, Registration>;

        public:
            void Register(const Locator& locator, const std::type_info* api, const std::any& factory) override;
            void RegisterSingleton(const Locator& locator, const std::type_info* api, const std::any& factory) override;
            void RegisterSingleton(const Locator& locator, const std::type_info* api, VoidPointer externalObject) override;
            bool RegisterRedirect(const Locator& source, const Locator& target) override;
            VoidPointer Get(const Locator& locator, const std::type_info* api, const Invoker& invoker) override;

            /*!
             * @brief Clears all IoC registrations
            */
            void Clear();

        private:
            /*!
             * @brief Iteratively query's the container for a matching entry
             * @param locator Input locator (Will query as is plus all "nm/.../*" combinations
             * @return Element iterator or end
            */
            Container::iterator Query(const Locator& locator);

            /*!
             * @brief Default invoker implementation
             *
             * (direct forward to parameterless factory)
             * @param factory Factory to forward
             * @return Pointer to instance
            */
            static VoidPointer DefaultInvoker(std::any& factory);

        private:
            /*!
             * @brief Container instance
            */
            Container m_container;
    };
}
