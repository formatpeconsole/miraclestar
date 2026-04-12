#include "sigscan.h"

namespace sigscan
{
static std::vector<int> patternToByte(const char* mask)
{
    std::vector<int> result{};
    size_t len = strlen(mask);
    size_t i = 0;

    while (i < len)
    {
        if (mask[i] == '?')
        {
            i++;
            if (i < len && mask[i] == '?')
                i++;
            result.push_back(-1);
        }
        else
        {
            char hex[3] = { mask[i], mask[i + 1], '\0' };
            result.push_back((int)strtoul(hex, nullptr, 16));
            i += 2;
        }
    }
    return result;
}

Ptr find(HANDLE module, const std::string& mask)
{
    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = patternToByte(mask.data());
    auto scannedBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scannedBytes[i + j] != (uint8_t)d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
            return Ptr((uintptr_t)&scannedBytes[i]);
    }
    return Ptr(0);
}
}