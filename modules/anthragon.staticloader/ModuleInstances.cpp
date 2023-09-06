#include "ModuleInstances.h"

#include "StaticLoader.h"

#include <anthragon.core/Module.h>

void Anthragon::Detail::RegisterModuleInstances(StaticLoader& loader)
{
    #ifdef ANT_HAS_ANTHRAGON_CORE
    loader.RegisterStaticModuleInstance("anthragon.core", std::move(std::make_shared<Core::Module>()));
    #endif
}
