#pragma once

#include <sdf-render/win.h>
#include <sdf-render/util/comptr.h>
#include <sdf-render/util/exception.h>

#include <vector>
#include <memory>
#include <filesystem>

namespace ant::sdf
{
    class image
    {
        public:
            using ptr = std::shared_ptr<image>;

        public:
            image() = delete;
            image(const std::filesystem::path& img_path);
            image(const image&) = delete;
            image(image&&) noexcept = delete;
            
            image& operator=(const image&) = delete;
            image& operator=(image&&) noexcept = delete;

            void evict();

            inline uint32_t get_width() const { return m_width; }
            inline uint32_t get_height() const { return m_height; }
            inline uint32_t get_bpp() const { return m_bpp; }
            inline uint32_t get_channel_count() const { return m_channel_count; }
            inline DXGI_FORMAT get_format() const { return m_format; }
            inline const void* get_data() const { return m_data.data(); }

        private:
            struct guid_to_dxgi_t { GUID guid; DXGI_FORMAT dxgi; };
            static const std::vector<guid_to_dxgi_t> guid_to_dxgi;

        private:
            std::vector<char> m_data;

            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint32_t m_bpp = 0;
            uint32_t m_channel_count = 0;
            DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
    };
}
