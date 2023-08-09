#include "dir.h"

const std::filesystem::path& ant::sdf::get_binary_directory()
{
    static std::filesystem::path bindir;
    if (bindir.empty())
    {
        wchar_t binname[MAX_PATH];
        ANT_CHECK(GetModuleFileNameW(nullptr, binname, MAX_PATH) > 0, "Failed to retrive module file name");
        bindir = binname;
        bindir.remove_filename();
    }

    return bindir;
}
