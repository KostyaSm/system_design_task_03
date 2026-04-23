#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace utils {

inline std::string ToLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return str;
}

}