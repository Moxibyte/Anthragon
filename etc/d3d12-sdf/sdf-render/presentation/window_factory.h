#pragma once

#include <sdf-render/win.h>
#include <sdf-render/util/exception.h>

#include <memory>
#include <functional>
#include <string_view>

namespace ant::sdf
{
    class window_factory
    {
        public:
            using proc = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

        public:
            window_factory(const window_factory&) = delete;
            window_factory(window_factory&&) noexcept = delete;

            window_factory& operator=(const window_factory&) = delete;
            window_factory& operator=(window_factory&&) noexcept = delete;

            HWND new_window(std::wstring_view name, DWORD style, DWORD ex_style, int x, int y, int width, int height, proc msg_proc);

        public:
            static inline window_factory& get()
            {
                static window_factory* instance;
                if (!instance)
                {
                    instance = new window_factory();
                }
                return *instance;
            }

        private:
            window_factory();
            ~window_factory() = default;

        private:
            static LRESULT CALLBACK msg_proc_setup(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);
            static LRESULT CALLBACK msg_proc_run(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);

        private:
            const HINSTANCE m_h_instance;
            ATOM m_class_atom = 0;
    };
}
