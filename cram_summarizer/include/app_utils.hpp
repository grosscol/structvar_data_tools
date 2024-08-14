#ifndef APP_UTILS
#define APP_UTILS

#include <string_view>
#include <charconv>

/* 
 * Utility method wrapping from_chars to essentially do stoi for views
 * https://mobiarch.wordpress.com/2022/12/12/string-to-number-conversion-in-modern-c/
 */
template <typename T>
bool view_to_numeric(std::string_view view, T& value) {
    if (view.empty()) {
        return false;
    }
     
    const char* first = view.data();
    const char* last = view.data() + view.length();
     
    std::from_chars_result res = std::from_chars(first, last, value);
     
    if (res.ec != std::errc()) {
        return false;
    }
     
    if (res.ptr != last) {
        return false;
    }
     
    return true;
}

#endif
