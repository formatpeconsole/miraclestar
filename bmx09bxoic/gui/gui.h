#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <any>

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

using itemsInMemoryList = std::vector<std::pair<void*, int>>;
struct Menu
{
    ItemOldValue<bool> openedOldValue{};
    bool opened = false;
    bool newOpened = true;

    RageTab rage;

    itemsInMemoryList itemsInMemory{
        { static_cast<void*>(&rage.enable.item), rage.enable.item.itemType },
        { static_cast<void*>(&rage.hitChance.item), rage.hitChance.item.itemType },
        { static_cast<void*>(&rage.minDamage.item), rage.minDamage.item.itemType },
        { static_cast<void*>(&rage.targetSelection.item), rage.targetSelection.item.itemType },
        { static_cast<void*>(&rage.hitBoxes.item), rage.hitBoxes.item.itemType },
        { static_cast<void*>(&rage.aimRandomize.item), rage.aimRandomize.item.itemType },
        { static_cast<void*>(&rage.aimHitboxColor.item), rage.aimHitboxColor.item.itemType },
    };

    KeyBindManager keyBindManager{};
};

extern void init();
extern void destroy();

extern void renderBindsDebugWindow();
extern void renderMainUI();

extern Menu& getMenuInstance();
}