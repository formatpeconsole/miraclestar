#include <string>
#include "gui.h"

#include "../guiItems/items.h"
#include "../config/config.h"

namespace gui
{
void init()
{
    binds::initBinds();
}

void destroy()
{
    binds::destroyBinds();
}

void* getItemValuePointerFromItemPointer(void* ptr, int type)
{
    switch (type)
    {
    case ITEM_CHECKBOX:
        return &reinterpret_cast<Item<bool>*>(ptr)->value;
    case ITEM_SLIDER_INT:
        return &reinterpret_cast<Item<int>*>(ptr)->value;
    case ITEM_SLIDER_FLOAT:
        return &reinterpret_cast<Item<float>*>(ptr)->value;
    case ITEM_COMBOBOX:
        return &reinterpret_cast<Item<int>*>(ptr)->value;
    case ITEM_MULTICOMBOBOX:
        return &reinterpret_cast<Item<unsigned int>*>(ptr)->value;
    case ITEM_COLOR:
        return &reinterpret_cast<Item<unsigned int>*>(ptr)->value;
    default:
        return {};
    }
}

std::optional<std::vector<std::string>> getItemList(void* ptr, int type)
{
    switch (type)
    {
    case ITEM_COMBOBOX:
    {
        auto item = reinterpret_cast<Item<int>*>(ptr);
        return item->itemsList;
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        auto item = reinterpret_cast<Item<unsigned int>*>(ptr);
        return item->itemsList;
    }
    break;
    default:
        return {};
    }
}

// get text without hashtags for ui naming
// for example
// we have name of item in UI called "Hit Chance##rage"
// but ##rage is used just to make unique hash
// so for UI Bind list better visualize name without this unique hash
// in result it will be "Hit Chance"
std::string getFormattedText(const std::string& text)
{
    size_t pos = text.find("##");
    if (pos == std::string::npos)
        return text;

    return text.substr(0, pos);
}

// in KeyBindManager i had to save value in string format for UI purposes
// now you have to get this value and visualize in ur UI however you want
std::string getBindValueFromString(const std::string& value, void* ptr, int itemType)
{
    switch (itemType)
    {
    case ITEM_CHECKBOX:
        return value == "1" ? "On" : "Off";
    case ITEM_SLIDER_FLOAT:
        return value + "f";
    case ITEM_SLIDER_INT:
        return value;
    case ITEM_COMBOBOX:
    {
        auto list = getItemList(ptr, itemType);
        if (list.has_value())
        {
            int index = std::stoi(value);
            if (index < 0 || index >= list->size())
                return "Invalid";

            return list->at(index);
        }
        return {};
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        auto list = getItemList(ptr, itemType);
        if (list.has_value())
        {
            unsigned int index = std::stoul(value);
            if (index == 0)
                return "Invalid";

            return gui::multicombobox::getActiveItems(index, list.value());
        }
        return {};
    }
    break;
    case ITEM_COLOR:
    {
        uint32_t color = std::stoul(value);

        auto r = (color >> 16) & 0xFF;
        auto g = (color >> 8) & 0xFF;
        auto b = color & 0xFF;
        auto a = (color >> 24) & 0xFF;

        // R: 255 G: 255 B: 255: A: 255
        return "R: "
            + std::to_string(r)
            + " G: "
            + std::to_string(g)
            + " B: "
            + std::to_string(b)
            + " A: "
            + std::to_string(a);
    }
    break;
    default:
        return value;
    }
}

void renderBindsDebugWindow()
{
    ImGui::Begin("BIND LIST", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        auto& binds = getMenuInstance().keyBindManager.getBindList();
        auto& itemsInMemory = getMenuInstance().itemsInMemory;
        for (auto it = binds.begin(); it != binds.end(); ++it)
        {
            auto item = (*it);
            if (item->getItemType() == ITEM_UI_OPEN)
                continue;

            auto itemThatBindUse = std::find_if(itemsInMemory.begin(), itemsInMemory.end(), [item](const std::pair<void*, int>& pair) {
                return getItemValuePointerFromItemPointer(pair.first, pair.second) == item->getItemPtr();
                });

            if (itemThatBindUse == itemsInMemory.end())
                continue;

            bool keyPressed = item->getType() == BIND_RELEASE ? !item->getPressed() : item->getPressed();
            if (keyPressed)
            {
                // bind format would be:
                // [+] ItemName -> Value (BindMode)
                std::string bindValue = getBindValueFromString(item->getBindValue(), itemThatBindUse->first, itemThatBindUse->second);
                std::string infoBegin = "[+] " + item->getItemName();
                std::string infoEnd = " -> " + bindValue + " (" + gui::binds::getBindMode(item->getType()) + ")";

                ImGui::Text("%s%s", getFormattedText(infoBegin).c_str(), infoEnd.c_str());
            }
        }
    }
    ImGui::End();
}

void renderMainUI()
{
    if (getMenuInstance().opened)
    {
        ImGui::SetNextWindowSize(ImVec2(780, 650));

        std::string windowTitle = "52HOOK - " + std::string(__DATE__);
        ImGui::Begin(windowTitle.c_str(), &getMenuInstance().opened, ImGuiWindowFlags_NoResize);
        {
            using namespace gui::items;
            checkbox::render(getMenuInstance().rage.enable);
            combobox::render(getMenuInstance().rage.targetSelection);
            slider::render(getMenuInstance().rage.hitChance);
            slider::render(getMenuInstance().rage.minDamage);
            multicombobox::render(getMenuInstance().rage.hitBoxes);
            slider::render(getMenuInstance().rage.aimRandomize);
            colorpicker::render(getMenuInstance().rage.aimHitboxColor);

            if (ImGui::SmallButton("Save"))
                config::saveConfig();
            ImGui::SameLine();
            if (ImGui::SmallButton("Load"))
                config::loadConfig();
        }
        ImGui::End();
    }
}

Menu& getMenuInstance()
{
    static Menu instance;
    return instance;
}
}