#include <iostream>

#include <anthragon/IModuleManager.h>
#include <anthragon/Helpers/Pointers.h>

int main()
{
    auto& mm = Anthragon::GetModuleManager();
    if (mm.Load())
    {
        auto* ctx = mm.CreateContext();

        ctx->Destroy();
        mm.Unload();
    }
}
