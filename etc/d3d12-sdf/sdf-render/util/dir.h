#pragma once

#include <sdf-render/win.h>
#include <sdf-render/util/exception.h>

#include <string>
#include <filesystem>

namespace ant::sdf
{
    const std::filesystem::path& get_binary_directory();
    std::filesystem::path find_file(const std::filesystem::path& path);
}
