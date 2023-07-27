#include <string>
#include <cstdio>

template<typename... Args>
std::string format_string(const char* format, Args... args) {
    int size = std::snprintf(nullptr, 0, format, args...);
    std::string result(size + 1, '\0');
    std::snprintf(&result[0], size + 1, format, args...);
    result.pop_back(); // Remove the extra null-terminator added by snprintf
    return result;
}
