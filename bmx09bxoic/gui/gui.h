#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "../render/render.h"
#include "binds/binds.h"

namespace gui
{
using namespace binds;

struct sliderBindInt
{
    int value{};
    int bindMode{};
    int bindKey{};
    std::string name{};
};

struct Menu
{
    ItemOldValue<bool> openedOldValue{};
    bool opened = false;
    bool newOpened = true;

    ItemOldValue<int> testSliderOldValue{};
    int testSlider = 50;
    std::list<sliderBindInt> testSliderBinds{};

    KeyBindManager keyBindManager{};
};

extern void init();
extern void destroy();
extern void updateAliveBindValues();
extern void renderDebugBindsWindow();
extern void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam);

extern Menu& getMenuInstance();
}