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

std::filesystem::path ant::sdf::find_file(const std::filesystem::path& path)
{
    if (path.is_absolute())
    {
        ANT_CHECK(std::filesystem::exists(path) && !std::filesystem::is_directory(path), "Can't find file");
        return path;
    }
    else
    {
        auto resolved_path = std::filesystem::current_path() / path;
        if (!std::filesystem::exists(resolved_path))
        {
            resolved_path = get_binary_directory() / path;
            ANT_CHECK(std::filesystem::exists(resolved_path), "Can't find file");
        }
        ANT_CHECK(!std::filesystem::is_directory(resolved_path), "Can't find file");
        return resolved_path;
    }
}
