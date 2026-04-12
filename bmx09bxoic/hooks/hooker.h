#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <format>
#include <algorithm>

enum HOOKS : unsigned int
{
    DX11_PRESENT = 0,
    DX11_RESIZE_BUFFERS,

    HOOKS_MAX
};

namespace hooker
{
void init();
void destroy();
}