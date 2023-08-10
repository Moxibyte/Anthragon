#pragma once

#include <sdf-render/win.h>
#include <sdf-render/d3d/d3d_context.h>
#include <sdf-render/d3d/d3d_executor.h>
#include <sdf-render/d3d/d3d_command_list.h>
#include <sdf-render/d3d/d3d_descriptor_heap.h>
#include <sdf-render/presentation/window_factory.h>

#include <memory>
#include <functional>

namespace ant::sdf
{
    class window
    {
        public:
            using ptr = std::shared_ptr<window>;

        public:
            window() = delete;
            window(d3d_context::ptr ctx, d3d_executor::ptr queue, std::wstring_view name, DWORD style, DWORD ex_style, int x, int y, int width, int height);
            window(const window&) = delete;
            window(window&&) noexcept = delete;
            ~window();

            window& operator=(const window&) = delete;
            window& operator=(window&&) noexcept = delete;

            void begin_frame(d3d_command_list::ptr cmd_list);
            void end_frame(d3d_command_list::ptr cmd_list);

            void resize();
            void present();
            void process_messages();

            inline bool should_close() const { return m_close_received; }
            inline bool should_resize() const { return m_resized; }
            inline uint32_t get_width() const { return m_width; }
            inline uint32_t get_height() const { return m_height; }

        private:
            void get_buffers();
            void release_buffers();
            
            LRESULT msg_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);

        private:
            d3d_context::ptr m_ctx;
            comptr<IDXGISwapChain4> m_swap_chain;
            size_t m_buffer_index = 0;

            d3d_descriptor_heap::ptr m_rtv_heap;
            comptr<ID3D12Resource> m_buffers[2];

            HWND m_wnd = nullptr;
            bool m_close_received = false;

            bool m_resized = false;
            uint32_t m_width = 0;
            uint32_t m_height = 0;
    };
}
