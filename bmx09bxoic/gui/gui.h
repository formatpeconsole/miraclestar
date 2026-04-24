#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "../render/render.h"
#include "binds/binds.h"
#include "item.h"

namespace gui
{
using namespace binds;
using namespace items;

struct Menu
{
    ItemOldValue<bool> openedOldValue{};
    bool opened = false;
    bool newOpened = true;

    Slider<int> hitChance{
        .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Hit Chance",
            .itemType = ITEM_SLIDER_INT,
        },
        .min = 0,
        .max = 100,
    };

    Slider<int> minDamage{
        .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Minimum Damage",
            .itemType = ITEM_SLIDER_INT,
        },
        .min = 0,
        .max = 100,
    };

    KeyBindManager keyBindManager{};
};

extern void init();
extern void destroy();
extern void updateAliveBindValues();
extern void renderDebugBindsWindow();
extern void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam);

extern Menu& getMenuInstance();
}