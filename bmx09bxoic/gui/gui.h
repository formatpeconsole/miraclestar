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

struct RageTab
{
    CheckBox enable{
        .item = {
            .oldValue = {},
            .value = false,
            .binds = {},
            .preview = {},
            .name = "Enable##rage",
            .itemType = ITEM_CHECKBOX,
            .itemsList = {},
        }
    };

    Slider<int> hitChance{
        .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Hit Chance##rage",
            .itemType = ITEM_SLIDER_INT,
            .itemsList = {},
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
            .name = "Minimum Damage##rage",
            .itemType = ITEM_SLIDER_INT,
            .itemsList = {},
        },
        .min = 0,
        .max = 100,
    };

    ComboBox targetSelection{
        .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Target Selection##rage",
            .itemType = ITEM_COMBOBOX,
            .itemsList = {
                "Lowest Health",
                "Closest To Crosshair",
                "Best Damage",
                "FOV",
            }
        },
    };

    MultiComboBox hitBoxes{
         .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Hitboxes##rage",
            .itemType = ITEM_MULTICOMBOBOX,
            .itemsList = {
                "Head",
                "Neck",
                "Chest",
                "Stomach",
                "Pelvis",
                "Legs",
                "Feet"
            },
        },
    };

    Slider<float> aimRandomize{
        .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Randomize##rage",
            .itemType = ITEM_SLIDER_FLOAT,
            .itemsList = {},
        },
        .min = 0.f,
        .max = 100.f,
    };

    ColorPicker aimHitboxColor{
         .item = {
            .oldValue = {},
            .value = 0,
            .binds = {},
            .preview = {},
            .name = "Aim Hitbox Color##rage",
            .itemType = ITEM_COLOR,
            .itemsList = {},
        },
    };
};

struct Menu
{
    ItemOldValue<bool> openedOldValue{};
    bool opened = false;
    bool newOpened = true;

    RageTab rage;

    KeyBindManager keyBindManager{};
};

extern void init();
extern void destroy();

extern void renderBindsDebugWindow();
extern void renderMainUI();

extern Menu& getMenuInstance();
}