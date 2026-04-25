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

std::string GetFormattedText(const std::string& text)
{
    size_t pos = text.find("##");
    if (pos == std::string::npos)
        return text;

    return text.substr(0, pos);
}

std::string GetCorrectBindValue(const std::string& value, 
    int itemType, 
    const std::optional<std::vector<std::string>>& selectionList)
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
        if (selectionList.has_value())
        {
            int index = std::stoi(value);
            if (index < 0 || index >= selectionList->size())
                return "Invalid";

            return selectionList->at(index);
        }
        return {};
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        if (selectionList.has_value())
        {
            unsigned int index = std::stoul(value);
            if (index == 0)
                return "Invalid";

            return gui::multicombobox::GetFlagPreview(index, selectionList.value());
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
        ImGui::Text("Active binds:");
        ImGui::Separator();

        auto& allItems = getMenuInstance().items;
        auto& binds = getMenuInstance().keyBindManager.getBindList();
        for (auto it = binds.begin(); it != binds.end(); ++it)
        {
            auto item = (*it);
            if (item->getItemType() == ITEM_UI_OPEN)
                continue;

            auto foundItem = std::find_if(allItems.begin(), allItems.end(), [item](const std::pair<void*, int>& pair) {
                return getItemValuePointerFromItemPointer(pair.first, pair.second) == item->getItemPtr()
                    && pair.second == item->getItemType();
            });

            if (foundItem == allItems.end())
                continue;

            bool isActive = item->getType() == BIND_RELEASE ? !item->getPressed() : item->getPressed();
            if (isActive)
            {
                std::string bindValue = GetCorrectBindValue(
                    item->getBindValue(), 
                    item->getItemType(),
                    getItemList(foundItem->first, foundItem->second));

                // [+] ItemName -> Value (BindMode)
                std::string infoBegin = "[+] " + item->getItemName();
                std::string infoEnd = " -> " + bindValue + " (" + gui::binds::getBindMode(item->getType()) + ")";

                ImGui::Text("%s%s", GetFormattedText(infoBegin).c_str(), infoEnd.c_str());
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

        std::string windowTitle = "BMX09BXOIC - " + std::string(__DATE__);
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