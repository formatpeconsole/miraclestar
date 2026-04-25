#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "../gui/item.h"
#include "../gui/gui.h"

namespace gui::items::combobox
{
inline void addComboBoxBind(ComboBox& comboBox)
{
    auto& item = comboBox.item;
    auto& newBind = item.binds.emplace_back();
    newBind.name = "ComboBoxBind-For " + item.name + " " + uuid::getUuid();

    getMenuInstance().keyBindManager.addBind(
        &item.value,
        &newBind.value,
        &item.oldValue,
        BIND_NONE,
        item.itemType,
        0,
        newBind.name
    );
}

inline decltype(&addComboBoxBind) comboBoxBindCallback = addComboBoxBind;

inline std::string getComboItemsList(ComboBox& comboBox)
{
    std::string itemsList{};
    for (const auto& item : comboBox.items)
    {
        itemsList += item + '\0';
    }
    itemsList += '\0';
    return itemsList;
}

inline void render(ComboBox& comboBox)
{
    auto& item = comboBox.item;

    std::string itemKey = std::to_string(reinterpret_cast<uintptr_t>(&item));
    std::string itemValueKey = std::to_string(reinterpret_cast<uintptr_t>(&item.value));

    std::string bindItemPopup = "bind-popup-comboBox##" + itemKey;
    std::string bindCombo = "##binds-for-" + itemValueKey;

    std::string bindAdd = "+##bind-add-for-" + itemKey;

    std::string bindOpenPopup = "* ##" + itemKey;

    ImGui::Combo(item.name.c_str(), &item.value, getComboItemsList(comboBox).c_str());
    item.value = std::clamp(item.value, 0, static_cast<int>(comboBox.items.size()) - 1);

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
                    comboBoxBindCallback(comboBox);

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
                        comboBoxBindCallback(comboBox);
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
                gui::binds::keyBind<std::list<BindValues<int>>>(currentBind, value);
            }

            ImGui::Combo(valueName.c_str(), &value->value, getComboItemsList(comboBox).c_str());
            value->value = std::clamp(value->value, 0, static_cast<int>(comboBox.items.size()) - 1);
        }

        ImGui::EndPopup();
    }
}
}