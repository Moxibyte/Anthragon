#include "window_factory.h"

HWND ant::sdf::window_factory::new_window(std::wstring_view name, DWORD style, DWORD ex_style, int x, int y, int width, int height, proc msg_proc)
{
    RECT cr;
    cr.top = cr.left = 0;
    cr.right = width;
    cr.bottom = height;
    AdjustWindowRectEx(&cr, style, false, ex_style);

    return CreateWindowExW(ex_style, (LPCWSTR)m_class_atom, name.data(), style, x, y, cr.right - cr.left, cr.bottom - cr.top, nullptr, nullptr, m_h_instance, new proc(msg_proc));
}

ant::sdf::window_factory::window_factory() :
    m_h_instance(GetModuleHandleW(nullptr))
{
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &window_factory::msg_proc_setup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_h_instance;
    wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"ant::sdf::window";
    wcex.hIconSm = wcex.hIcon;

    m_class_atom = RegisterClassExW(&wcex);
    ANT_CHECK(m_class_atom != 0, "Failed to register window class");
}

LRESULT ant::sdf::window_factory::msg_proc_setup(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    if (msg == WM_NCCREATE)
    {
        auto* cr = (CREATESTRUCTW*)l_param;
        SetWindowLongPtrW(wnd, GWLP_USERDATA, (LONG_PTR)cr->lpCreateParams);
        SetWindowLongPtrW(wnd, GWLP_WNDPROC, (LONG_PTR)&window_factory::msg_proc_run);
        return msg_proc_run(wnd, msg, w_param, l_param);
    }
    return DefWindowProcW(wnd, msg, w_param, l_param);
}

LRESULT ant::sdf::window_factory::msg_proc_run(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    auto* window_proc = (proc*)GetWindowLongPtrW(wnd, GWLP_USERDATA);
    if (msg == WM_NCDESTROY)
    {
        delete window_proc;
        SetWindowLongPtrW(wnd, GWLP_WNDPROC, (LONG_PTR)&::DefWindowProcW);
        return DefWindowProcW(wnd, msg, w_param, l_param);
    }

    return (*window_proc)(wnd, msg, w_param, l_param);
}
