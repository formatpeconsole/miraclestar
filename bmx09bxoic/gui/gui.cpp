#include "gui.h"

namespace gui
{
void init()
{
    getMenuInstance().keyBindManager.init();
    getMenuInstance().keyBindManager.addBind(
        &getMenuInstance().opened,
        &getMenuInstance().newOpened,
        BIND_TOGGLE,
        VK_INSERT
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

void destroy()
{
    getMenuInstance().keyBindManager.destroy();
}

void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto& bindList = getMenuInstance().keyBindManager.getBindList();
    const auto correctBind = std::find_if(bindList.begin(), bindList.end(), [wParam](const std::shared_ptr<IKeyBind>& keyBind)
        {
            return keyBind->getKey() == wParam;
        });

    if (correctBind != bindList.end())
    {
        auto bindToUpdate = (*correctBind);
        const auto bindType = bindToUpdate->getType();

        if (bindType != BIND_ALWAYS_ON && bindType != BIND_FORCE_OFF)
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
                        bindToUpdate->setPressed(!bindToUpdate->getPressed());
                }
            }
            break;
            case BIND_HOLD:
            case BIND_RELEASE:
            {
                bool releaseType = (bindType == BIND_RELEASE);

                if (uMsg == WM_KEYDOWN)
                {
                    bool newState = !((lParam >> 30) & 1);
                    if (newState)
                        bindToUpdate->setPressed(releaseType ? false : true);
                }
                else if (uMsg == WM_KEYUP)
                {
                    bool newState = ((lParam >> 30) & 1);
                    if (newState)
                        bindToUpdate->setPressed(releaseType ? true : false);
                }
            }
            break;
            }

            if (bindToUpdate->getPressed())
                bindToUpdate->setValueToNew();
            else
                bindToUpdate->setValueToOld();
        }
    }
}

Menu& getMenuInstance()
{
    static Menu instance;
    return instance;
}
}