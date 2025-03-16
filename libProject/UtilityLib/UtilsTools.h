#pragma once

#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <atlstr.h>

class UtilsTools
{
public:
    static uint64_t GetTimestampMs();
    static std::string GetUUID();

    static std::string UrlEncode(const std::string& str);
    static std::string UrlDecode(const std::string& str);
};