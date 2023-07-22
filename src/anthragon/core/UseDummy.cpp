/*
 * Anthragon C++ GUI library
 * (c) Copyright 2023 Moxibyte GmbH
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this
 * program. If not, see <https://opensource.org/license/mit/>.
 *
 */
#include <anthragon/core/UseDummy.h>

class DumIn : public IDummy
{
    public:
        DumIn(int _a, int _b) : a(_a), b(_b) {}

        void CallDummy() override
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        static std::shared_ptr<void> Factory(int a, int b)
        {
            return std::make_shared<DumIn>(DumIn{ a, b });
        }

    private:
        int a, b;
};

void UseDummy()
{
    anthragon::core::IoCContainer container;

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
}
