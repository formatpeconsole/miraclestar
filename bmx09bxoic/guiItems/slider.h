#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "../gui/item.h"
#include "../gui/gui.h"

namespace gui::items::slider
{
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

inline void printAllItems()
{
    auto& instance = getMenuInstance();

    printf("Slider<int> hitChance \n");
    printf("Main value: %d \n", instance.hitChance.item.value);
    printf("Item type: %s \n", getItemType(instance.hitChance.item.itemType).c_str());

    printf("Binds: \n");

    for (auto& i : instance.hitChance.item.binds)
    {
        printf("    Bind name: %s \n", i.name.c_str());
        printf("    Value: %d \n", i.value);
        printf("    Bind mode: %s \n", getBindMode(i.bindMode).c_str());
        printf("    Bind key: %d \n", i.bindKey);
        printf("\n");
    }

    printf("\n");
    printf("\n");

    printf("Slider<int> minDamage \n");
    printf("Main value: %d \n", instance.minDamage.item.value);
    printf("Item type: %s \n", getItemType(instance.minDamage.item.itemType).c_str());

    printf("Binds: \n");

    for (auto& i : instance.minDamage.item.binds)
    {
        printf("    Bind name: %s \n", i.name.c_str());
        printf("    Value: %d \n", i.value);
        printf("    Bind mode: %s \n", getBindMode(i.bindMode).c_str());
        printf("    Bind key: %d \n", i.bindKey);
    }
}

template<typename T>
inline void addSliderBind(Slider<T>& slider)
{
    auto& item = slider.item;
    auto& newBind = item.binds.emplace_back();
    newBind.name = "SliderBind-For " + item.name + " " + uuid::getUuid();

    getMenuInstance().keyBindManager.addBind(
        &item.value,
        &newBind.value,
        &item.oldValue,
        BIND_NONE,
        item.itemType,
        0,
        newBind.name
    );

    printAllItems();
}

template<typename T>
inline decltype(&addSliderBind<T>) bindCallback = addSliderBind<T>;

template<typename T>
inline void render(Slider<T>& slider)
{
    auto& item = slider.item;

    std::string itemKey = std::to_string(reinterpret_cast<uintptr_t>(&item));
    std::string itemValueKey = std::to_string(reinterpret_cast<uintptr_t>(&item.value));

    std::string bindItemPopup = "bind-popup-slider##" + itemKey;
    std::string bindCombo = "##binds-for-" + itemValueKey;

    std::string bindAdd = "+##bind-add-for-" + itemKey;

    std::string bindOpenPopup = "* ##" + itemKey;

    ImGui::SliderInt(item.name.c_str(), &item.value, slider.min, slider.max);
    item.value = std::clamp(item.value, slider.min, slider.max);

    ImGui::SameLine();
    if (ImGui::SmallButton(bindOpenPopup.c_str()))
    {
        ImGui::OpenPopup(bindItemPopup.c_str());
    }

    if (ImGui::BeginPopup(bindItemPopup.c_str()))
    {
        ImGui::Text("Binds");

        auto& preview = item.preview;

        if (item.binds.empty())
            preview.label = "No binds.";
        else
        {
            if (preview.selectedBind.has_value()
                && preview.selectedBind.value() != item.binds.end())
                preview.label = preview.selectedBind.value()->name;
        }

        if (ImGui::BeginCombo(bindCombo.c_str(), preview.label.c_str()))
        {
            if (item.binds.empty())
            {
                ImGui::Text("No binds found! Click + to add bind");

                if (ImGui::SmallButton(bindAdd.c_str()))
                {
                    bindCallback<T>(slider);

                    preview.selection = 0;
                    preview.selectedBind.reset();
                }
            }
            else
            {
                int bindsIter = 0;
                auto& binds = item.binds;
                for (auto it = binds.begin(); it != binds.end();)
                {
                    if (preview.erased && preview.selection == bindsIter)
                    {
                        preview.selectedBind = it;
                        preview.erased = false;
                    }

                    if (ImGui::Selectable(it->name.c_str(), preview.selection == bindsIter, 0, ImVec2(100, 15)))
                    {
                        preview.selection = bindsIter;
                        preview.selectedBind = it;
                    }

                    ImGui::SameLine();
                    ++bindsIter;

                    const std::string bindPlus = "+##bind_" + it->name;
                    const std::string bindMinus = "-##bind_" + it->name;

                    if (ImGui::SmallButton(bindPlus.c_str()))
                    {
                        bindCallback<T>(slider);
                        preview.selectedBind.reset();
                        preview.selection = bindsIter;
                        continue;
                    }

                    ImGui::SameLine();

                    if (ImGui::SmallButton(bindMinus.c_str()))
                    {
                        getMenuInstance().keyBindManager.removeBind(getMenuInstance().keyBindManager.findBind(&it->value)->getBindName());
                        it = binds.erase(it);
                        preview.selectedBind.reset();
                        preview.selection = bindsIter;
                        preview.erased = true;
                        continue;
                    }

                    ++it;
                }
            }

            ImGui::EndCombo();
        }

        if (preview.selectedBind.has_value())
        {
            auto& value = preview.selectedBind.value();

            std::string valueName = "Value ##" + value->name;
            std::string bindType = "##bind-type-to" + value->name;

            auto currentBind = getMenuInstance().keyBindManager.findBind(&value->value);
            if (currentBind != nullptr)
            {
                ImGui::Text("Type");
                ImGui::Combo(bindType.c_str(), &value->bindMode, "Always On\0Hold\0Toggle\0Release\0Force Off\0");

                currentBind->setType(value->bindMode + 1);

                ImGui::Text("Current Key");
                ImGui::SameLine();
                gui::binds::keyBind<std::list<BindValues<T>>>(currentBind, value);
            }

            ImGui::SliderInt(valueName.c_str(), &value->value, slider.min, slider.max);
        }

        ImGui::EndPopup();
    }
}
}