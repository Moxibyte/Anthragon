#pragma once

#include <sdf-render/win.h>

#include <string>
#include <format>
#include <exception>
#include <string_view>

#define ANT_CHECK(expr, what) \
do { \
    if(!(expr)) \
    { \
        throw ::ant::sdf::exception(__FUNCTION__, __LINE__, what); \
    } \
} while (false)

#define ANT_CHECK_HR(expr, what) \
do { \
    HRESULT __inline_hr = (expr); \
    if(FAILED(__inline_hr)) \
    { \
        throw ::ant::sdf::exception(__FUNCTION__, __LINE__, what, __inline_hr); \
    } \
} while(false)

namespace ant::sdf
{
    class exception : public std::exception
    {
        public:
            exception() = default;
            exception(std::string_view function, int line, std::string_view msg);
            exception(std::string_view function, int line, std::string_view msg, HRESULT hr);

            const char* what() const override;

        protected:
            void set_text(std::string_view function, int line, std::string_view msg);

        private:
            std::string m_msg;
    };
}
