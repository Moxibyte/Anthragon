#pragma once

#include <sdf-render/win.h>
#include <sdf-render/util/exception.h>

namespace ant::sdf
{
    class d3d_context
    {
        public:
            d3d_context();
            d3d_context(const d3d_context&) = delete;
            d3d_context(d3d_context&&) noexcept = delete;

            ~d3d_context();
            
            d3d_context& operator=(const d3d_context&) = delete;
            d3d_context& operator=(d3d_context&&) noexcept = delete;

            inline auto get_factory() { return m_dxgi_factory; }
            inline auto get_device() { return m_device; }

        private:
            comptr<IDXGIFactory7> m_dxgi_factory;
            comptr<ID3D12Device> m_device;

            #ifdef _DEBUG
            comptr<ID3D12Debug> m_d3d_debug;
            comptr<IDXGIDebug1> m_dxgi_debug;
            #endif      
    };
}
