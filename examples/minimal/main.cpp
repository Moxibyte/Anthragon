#include <iostream>

#include <anthragon/IModuleManager.h>
#include <anthragon/Helpers/IoCProxy.h>

class IDummy
{
    public:
        virtual ~IDummy() = default;

        /*!
        * @brief This shall do nothing
        */
        virtual void CallDummy() = 0;
};

class DumIn : public IDummy
{
    public:
        DumIn(int _a, int _b) : a(_a), b(_b) {}

        void CallDummy() override
        {
            ;
        }

        static Anthragon::IInversionController::VoidPointer Factory(int a, int b)
        {
            return std::make_shared<DumIn>(DumIn{ a, b });
        }

    private:
        int a, b;
};

int main()
{
    auto loader = Anthragon::CreateModuleManager();
    if (loader->Load())
    {
        auto container = loader->GetIoCContainer();

        // External singleton
        auto d1 = std::make_shared<DumIn>(1, 2);
        container.RegisterSingleton<IDummy>("d1", d1);
        auto d1x = container.Get<IDummy>("d1");

        // Factory (singleton)
        container.RegisterSingleton<IDummy, int, int>("d2", &DumIn::Factory);
        auto d2x = container.Get<IDummy>("d2", 3, 4);
        auto d2x2 = container.Get<IDummy>("d2", 5, 6);
        auto d2x3 = container.Get<IDummy>("d2");

        // Object
        container.Register<IDummy, int, int>("d3", &DumIn::Factory);
        auto d3x = container.Get<IDummy>("d3", 11, 12);
        auto d3x2 = container.Get<IDummy>("d3", 21, 22);
        auto d3x3 = container.Get<IDummy>("d3", 31, 32);

        // Redirect
        container.RegisterRedirect("default", "d3");
        auto ddf = container.Get<IDummy>("default", 101, 102);

        loader->Shutdown();
    }
}
