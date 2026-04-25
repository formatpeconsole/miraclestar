#include "config.h"

#include <fstream>
#include <iostream>
#include <json.hpp>

#include "../gui/gui.h"
#include "../utils/uuid.h"

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
std::string getSliderName(const Slider<T>& slider)
{
    return getItemType(slider.item.itemType) + "-" + slider.item.name;
}

template<typename T>
void addItem(nlohmann::json& jsonToWrite, const Item<T>& item)
{
    auto& itemJson = jsonToWrite;
    itemJson["value"] = item.value;
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
    item.value = jsonResult["value"].get<T>();

    item.preview.erased = false;
    item.preview.selection = false;
    item.preview.selectedBind.reset();
    item.preview.label = "No binds.";

    item.binds.clear();

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
    {
        loadItem(rageSection[getSliderName(instance.hitChance)], instance.hitChance.item);
        loadItem(rageSection[getSliderName(instance.minDamage)], instance.minDamage.item);
    }
}

void saveConfig()
{
    std::ofstream configFile(getFileName());
   
    nlohmann::json jsonToWrite{};

    auto& instance = getMenuInstance();
    auto& rageSection = jsonToWrite["Rage"];
    {
        addItem(rageSection[getSliderName(instance.hitChance)], instance.hitChance.item);
        addItem(rageSection[getSliderName(instance.minDamage)], instance.minDamage.item);
    }

    configFile << jsonToWrite;
    std::cout << jsonToWrite;
}

std::string getFileName()
{
    return "testFile.txt";
}
}