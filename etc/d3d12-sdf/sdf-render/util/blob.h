#pragma once

#include <sdf-render/util/exception.h>

#include <vector>
#include <fstream>
#include <filesystem>

namespace ant::sdf
{
    class blob
    {
        public:
            blob() = default;
            blob(const std::filesystem::path& file);
            blob(const blob&) = default;
            blob(blob&&) noexcept = default;

            blob& operator=(const blob&) = default;
            blob& operator=(blob&&) noexcept = default;

            inline const void* data() const { return m_data.data(); }
            inline size_t size() const { return m_data.size(); }
        private:
            std::vector<char> m_data;
    };
}
