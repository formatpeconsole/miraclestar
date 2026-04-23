#include "binds.h"
#include "../gui.h"

namespace gui::binds
{
void initBinds()
{
    getMenuInstance().keyBindManager.init();
    getMenuInstance().keyBindManager.addBind(
        &getMenuInstance().opened,
        &getMenuInstance().newOpened,
        &getMenuInstance().openedOldValue,
        BIND_TOGGLE,
        ITEM_UI_OPEN,
        VK_INSERT,
        {}
    );

    auto& bindList = getMenuInstance().keyBindManager.getBindList();
    const auto menuKey = std::find_if(bindList.begin(), bindList.end(), [](const std::shared_ptr<IKeyBind>& keyBind)
        {
            return keyBind->getKey() == VK_INSERT;
        });

    if (menuKey != bindList.end())
    {
        auto bindToUpdate = (*menuKey);
        bindToUpdate->setOldValue();
        bindToUpdate->setNewValue();
    }
}
void destroyBinds()
{
    getMenuInstance().keyBindManager.destroy();
}

void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto& bindManager = getMenuInstance().keyBindManager;
    auto& bindList = bindManager.getBindList();
    const auto correctBind = std::find_if(bindList.begin(), bindList.end(), [wParam](const std::shared_ptr<IKeyBind>& keyBind)
        {
            return keyBind->getKey() == wParam;
        });

    if (correctBind != bindList.end())
    {
        auto bindToUpdate = (*correctBind);
        const auto bindType = bindToUpdate->getType();

     //   if (bindType != BIND_ALWAYS_ON && bindType != BIND_FORCE_OFF)
        {
            switch (bindType)
            {
            case BIND_TOGGLE:
            {
                // according to winapi docs
                // 30th bit of lParam contains previous state of message
                // that button sends

                // update ui pressed state only when you didn't press it before
                // (allows to bypass spamming value when user hold key)
                if (uMsg == WM_KEYDOWN)
                {
                    bool newState = !((lParam >> 30) & 1);
                    if (newState)
                    {
                        if (!bindToUpdate->getPressed())
                            bindManager.onKeyDown(bindToUpdate);
                        else
                            bindManager.onKeyUp(bindToUpdate);

                        bindToUpdate->setPressed(!bindToUpdate->getPressed());
                    }
                }
                else if (uMsg == WM_KEYUP)
                {
                    bindManager.onKeyUp(bindToUpdate);
                }
            }
            break;
            case BIND_HOLD:
            case BIND_RELEASE:
            {
                if (uMsg == WM_KEYDOWN)
                {
                    if (!bindToUpdate->getPressed())
                    {
                        bindManager.onKeyDown(bindToUpdate);
                        bindToUpdate->setPressed(true);
                    }
                }
                else if (uMsg == WM_KEYUP)
                {
                    if (bindToUpdate->getPressed())
                    {
                        bindManager.onKeyUp(bindToUpdate);
                        bindToUpdate->setPressed(false);
                    }
                }
            }
            break;
            }
        }
    }
}

std::string getBindType(int type)
{
    switch (type)
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
    }
    return {};
}

void renderDebugBindsWindow()
{
    auto& bindList = getMenuInstance().keyBindManager.getBindList();
    ImGui::Begin("bind debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::Text("binds: ");

        int counter = 0;
        for (auto bind : bindList)
        {
            if (bind->getItemType() == ITEM_UI_OPEN)
                continue;

            bool validState = false;
            if (bind->getType() == BIND_ALWAYS_ON || bind->getType() == BIND_FORCE_OFF)
            {
                validState = true;
            }
            else
                validState = bind->getType() == BIND_RELEASE ? !bind->getPressed() : bind->getPressed();

            if (validState)
            {
                ImGui::Text("[+] %s -> %s | %s", bind->getBindName().c_str(), bind->getBindValue().c_str(), getBindType(bind->getType()).c_str());
            }

            ++counter;
        }
    }
    ImGui::End();
}
}