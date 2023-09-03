#include "exception.h"

ant::sdf::exception::exception(std::string_view function, int line, std::string_view msg)
{
    set_text(function, line, msg);
}

ant::sdf::exception::exception(std::string_view function, int line, std::string_view msg, HRESULT hr)
{
    std::string hr_msg = "<unknown>";
    char* hr_msg_a;
    if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, hr, 0, (LPSTR)&hr_msg_a, 0, nullptr) > 0)
    {
        hr_msg = hr_msg_a;
        LocalFree(hr_msg_a);

        while (!hr_msg.empty() && hr_msg.at(hr_msg.length() - 1) == ' ')
        {
            hr_msg = hr_msg.substr(0, hr_msg.length() - 1);
        }
        if (!hr_msg.empty() && hr_msg.at(hr_msg.length() - 1) == '.')
        {
            hr_msg = hr_msg.substr(0, hr_msg.length() - 1);
        }
    }

    set_text(function, line, std::format("{}\n{} (0x{:X})", msg, hr_msg, (uint32_t)hr));
}

const char* ant::sdf::exception::what() const
{
    return m_msg.c_str();
}

void ant::sdf::exception::set_text(std::string_view function, int line, std::string_view msg)
{
    m_msg = std::format("{}\nIn function \"{}\" on line {}", msg, function, line);
}
