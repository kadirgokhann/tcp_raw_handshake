#pragma once
#include <string>

namespace utils
{
    std::string GetLocalIp();
    unsigned short GetChecksum(void *b, int len);
}