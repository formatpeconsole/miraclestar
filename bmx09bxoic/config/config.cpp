#include "config.h"

#include <fstream>
#include <iostream>
#include <json.hpp>

#include "../gui/gui.h"
#include "../utils/uuid.h"

#define SAVE_ITEM(section, element) addItem(section[getItemName(element.item)], element.item);
#define LOAD_ITEM(section, element) loadItem(section[getItemName(element.item)], element.item);

namespace config
{
using namespace gui;

inline std::string getItemType(int type)
{
    switch (type)
    {
    case ITEM_CHECKBOX:
        return "Checkbox";
    case ITEM_SLIDER_INT:
        return "Slider<int>";
    case ITEM_SLIDER_FLOAT:
        return "Slider<float>";
    case ITEM_COMBOBOX:
        return "ComboBox";
    case ITEM_MULTICOMBOBOX:
        return "MultiComboBox";
    case ITEM_COLOR:
        return "Color";
    default:
        return "";
    }
}

inline std::string getBindMode(int mode)
{
    switch (mode)
    {
    case BIND_ALWAYS_ON:
        return "Always On";
    case BIND_HOLD:
        return "Hold";
    case BIND_TOGGLE:
        return "Toggle";
    case BIND_RELEASE:
        return "Release";
    case BIND_FORCE_OFF:
        return "Force Off";
    default:
        return "";
    }
}

std::string readFile(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return "";

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

template<typename T>
std::string getItemName(const Item<T>& item)
{
    return getItemType(item.itemType) + "-" + item.name;
}

template<typename T>
void addItem(nlohmann::json& jsonToWrite, const Item<T>& item)
{
    auto& itemJson = jsonToWrite;

    T valueToRead = item.value;

    auto activeBind = getMenuInstance().keyBindManager.findBindByItem(&item.value);
    if (activeBind != nullptr)
    {
        bool pressed = activeBind->getPressed();
        if (activeBind->getType() == BIND_RELEASE)
            pressed = !activeBind->getPressed();

        if (pressed && item.oldValue.wrote)
            valueToRead = item.oldValue.value;
    }

    itemJson["value"] = valueToRead;
    itemJson["itemType"] = getItemType(item.itemType);
    itemJson["binds"] = nlohmann::json::array();
    for (auto& i : item.binds)
    {
        if (i.name.empty())
            continue;
        nlohmann::json bindJson{};
        bindJson["name"] = i.name;
        bindJson["value"] = i.value;
        bindJson["bindMode"] = i.bindMode;
        bindJson["bindModeName"] = getBindMode(i.bindMode + 1);
        bindJson["bindKey"] = i.bindKey;
        itemJson["binds"].push_back(bindJson);
    }
}

template<typename T>
void loadItem(nlohmann::json& jsonResult, Item<T>& item)
{
    item.preview.erased = false;
    item.preview.selection = false;
    item.preview.selectedBind.reset();
    item.preview.label = "No binds.";

    item.binds.clear();

    if (jsonResult.empty())
        return;

    item.value = jsonResult["value"].get<T>();

    auto& bindsSection = jsonResult["binds"];
    for (auto& bind : bindsSection)
    {
        auto& newBind = item.binds.emplace_back();
        newBind.name = bind["name"].get<std::string>();
        newBind.value = bind["value"].get<T>();
        newBind.bindMode = bind["bindMode"].get<int>();
        newBind.bindKey = bind["bindKey"].get<int>();
        newBind.keyEmpty = false;
        newBind.label = ImGui_ImplWin32_VKeyToString(newBind.bindKey);

        getMenuInstance().keyBindManager.addBind(
            &item.value,
            &newBind.value,
            &item.oldValue,
            newBind.bindMode + 1,
            item.itemType,
            newBind.bindKey,
            newBind.name
        );
    }
}

void loadConfig()
{
    auto fileToString = readFile(getFileName());
    auto jsonResult = nlohmann::json::parse(fileToString);

    auto& instance = getMenuInstance();
    instance.keyBindManager.eraseAllBinds();

    auto& rageSection = jsonResult["Rage"];
    if (!rageSection.empty())
    {
        LOAD_ITEM(rageSection, instance.rage.enable);
        LOAD_ITEM(rageSection, instance.rage.hitChance);
        LOAD_ITEM(rageSection, instance.rage.minDamage);
        LOAD_ITEM(rageSection, instance.rage.targetSelection);
        LOAD_ITEM(rageSection, instance.rage.hitBoxes);
        LOAD_ITEM(rageSection, instance.rage.aimHitboxColor);
    }
}

void saveConfig()
{
    std::ofstream configFile(getFileName());
   
    nlohmann::json jsonToWrite{};

    auto& instance = getMenuInstance();
    auto& rageSection = jsonToWrite["Rage"];
    {
        SAVE_ITEM(rageSection, instance.rage.enable);
        SAVE_ITEM(rageSection, instance.rage.hitChance);
        SAVE_ITEM(rageSection, instance.rage.minDamage);
        SAVE_ITEM(rageSection, instance.rage.targetSelection);
        SAVE_ITEM(rageSection, instance.rage.hitBoxes);
        SAVE_ITEM(rageSection, instance.rage.aimHitboxColor);
    }

    configFile << jsonToWrite;
}

std::string getFileName()
{
    return "testFile.txt";
}
}