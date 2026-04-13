#pragma once
#include "../render/render.h"

struct Menu
{
    bool opened = false;
};

namespace gui
{
extern void init();
extern void destroy();

extern Menu& getMenuInstance();
}