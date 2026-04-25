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

void renderMainUI()
{
    if (getMenuInstance().opened)
    {
        ImGui::SetNextWindowSize(ImVec2(780, 650));
        ImGui::Begin("hi", &getMenuInstance().opened, ImGuiWindowFlags_NoResize);
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