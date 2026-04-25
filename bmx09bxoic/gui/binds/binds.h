#pragma once
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <Windows.h>

#include "../../render/render.h"
#include "../../utils/uuid.h"

namespace gui::binds
{
enum BindType
{
    BIND_NONE = 0,
    BIND_ALWAYS_ON,
    BIND_HOLD,
    BIND_TOGGLE,
    BIND_RELEASE,
    BIND_FORCE_OFF,
};

enum ItemType
{
    ITEM_NONE = 0,
    ITEM_UI_OPEN,
    ITEM_CHECKBOX,
    ITEM_SLIDER_INT,
    ITEM_SLIDER_FLOAT,
    ITEM_COMBOBOX,
    ITEM_MULTICOMBOBOX,
    ITEM_COLOR,
};

constexpr auto MAX_BINDS = 1000;

template<typename T>
struct ItemOldValue
{
    bool wrote{};
    T value;

    void setOldValue(T newValue)
    {
        if (!wrote)
        {
            value = newValue;
            wrote = true;
        }
    }

    void resetOldValue(T newValue)
    {
        if (wrote)
        {
            value = newValue;
            wrote = false;
        }
    }

    T getOldValue()
    {
        return value;
    }
};

class IKeyBind
{
public:
    virtual int getType() = 0;
    virtual int getOldType() = 0;
    virtual int getItemType() = 0;
    virtual int getKey() = 0;

    virtual void* getItemPtr() = 0;
    virtual void* getBindPtr() = 0;

    virtual void setType(int type) = 0;
    virtual void setItemType(int type) = 0;
    virtual void setKey(int key) = 0;
    virtual void updateOldType() = 0;

    virtual bool getPressed() = 0;
    virtual void setPressed(bool state) = 0;

    virtual void setOldValue() = 0;
    virtual void setNewValue() = 0;

    virtual void setValueToNew() = 0;
    virtual void setValueToOld() = 0;
    virtual void setBindToOff() = 0;
    virtual void setBindToOn() = 0;

    virtual bool isValueSet() = 0;
    virtual std::string getBindName() = 0;
    virtual std::string getBindValue() = 0;

    virtual ~IKeyBind() = default;
};

template<typename T>
class KeyBind : public IKeyBind
{
public:
    KeyBind(T* item, T* bind, ItemOldValue<T>* oldValue, int type, int itemType, int key, std::string name)
        : itemPtr(item),
        bindItemPtr(bind),
        oldValue(oldValue),
        type(type),
        itemType(itemType),
        key(key),
        name(name)
    {
    }

    int getType() override
    {
        return type;
    }

    int getOldType() override
    {
        return oldType;
    }

    int getItemType() override
    {
        return itemType;
    }

    int getKey() override
    {
        return key;
    }

    std::string getBindName() override
    {
        return name;
    }

    std::string getBindValue() override
    {
        return bindValue;
    }

    void* getItemPtr() override
    {
        return reinterpret_cast<void*>(itemPtr);
    }

    void* getBindPtr() override
    {
        return reinterpret_cast<void*>(bindItemPtr);
    }

    void setType(int other) override
    {
        type = other;
    }

    void setItemType(int other) override
    {
        itemType = other;
    }

    void setKey(int other) override
    {
        key = other;
    }

    void updateOldType() override
    {
        oldType = type;
    }

    bool getPressed() override
    {
        return pressed;
    }

    void setPressed(bool state) override
    {
        pressed = state;
    }

    bool isValueSet() override
    {
        return wrote;
    }

    void setOldValue() override
    {
        oldValue->setOldValue(*itemPtr);
    }

    void setNewValue() override
    {
        newValue = *bindItemPtr;
        bindValue = std::to_string(newValue);
    }

    void setValueToNew() override
    {
        if (!wrote)
        {
            oldValue->setOldValue(*itemPtr);
            wrote = true;
        }

        *itemPtr = newValue;
    }

    void setValueToOld() override
    {
        *itemPtr = oldValue->getOldValue();

        if (wrote)
        {
            oldValue->resetOldValue(*itemPtr);
            wrote = false;
        }
    }

    void setBindToOff() override
    {
        if (!setOff)
        {
            setPressed(false);
            setOff = true;
        }
    }

    void setBindToOn() override
    {
        if (!setOff)
        {
            setPressed(true);
            setOff = true;
        }
    }

private:
    int type = BIND_NONE;
    int oldType = BIND_NONE;
    int itemType = ITEM_NONE;
    int key = VK_INSERT;

    T* itemPtr = nullptr;
    T* bindItemPtr = nullptr;
    ItemOldValue<T>* oldValue{};
    T newValue{};

    bool wrote = false;
    bool pressed = false;
    bool setOff = false;

    std::string name{};
    std::string bindValue{};
};

struct UiBlock
{
    bool blocked = false;
    int bindType = BIND_NONE;
    int activeKeyCount = 0;
    std::string bindName{};
};

using bindList = std::vector<std::shared_ptr<IKeyBind>>;
class KeyBindManager
{
private:
    bindList keyBinds{};
    std::unordered_map<void*, UiBlock> uiBlock{};

public:

    void init()
    {
        keyBinds.reserve(MAX_BINDS);
    }

    void destroy()
    {
        keyBinds.clear();
        uiBlock.clear();
    }

    template<typename T>
    void addBind(T* ptr, T* bindPtr, ItemOldValue<T>* oldValue, int type, int itemType, int key, std::string name)
    {
        std::string bindName = name + uuid::getUuid();

        std::shared_ptr<IKeyBind> bind = std::make_shared<KeyBind<T>>(ptr, bindPtr, oldValue, type, itemType, key, bindName);
        bind->setOldValue();
        keyBinds.emplace_back(bind);
    }

    template<typename T>
    std::shared_ptr<IKeyBind> findBindByItem(T* itemPtr)
    {
        const auto it = std::find_if(keyBinds.begin(), keyBinds.end(), [itemPtr](const std::shared_ptr<IKeyBind>& bind) {
            return bind->getItemPtr() == itemPtr;
            });
        if (it != keyBinds.end())
        {
            return *it;
        }
        return {};
    }

    template<typename T>
    std::shared_ptr<IKeyBind> findBind(T* bindPtr)
    {
        const auto it = std::find_if(keyBinds.begin(), keyBinds.end(), [bindPtr](const std::shared_ptr<IKeyBind>& bind) {
            return bind->getBindPtr() == bindPtr;
            });

        if (it != keyBinds.end())
        {
            return *it;
        }

        return {};
    }

    void removeBind(std::string name)
    {
        const auto it = std::find_if(keyBinds.begin(), keyBinds.end(), [&name](const std::shared_ptr<IKeyBind>& bind) {
            return bind->getBindName() == name;
            });
        if (it != keyBinds.end())
        {
            const auto foundBlock = uiBlock.find((*it)->getItemPtr());
            if (foundBlock != uiBlock.end())
            {
                uiBlock.erase(foundBlock);
            }

            (*it)->setValueToOld();
            keyBinds.erase(it);
        }
    }

    void eraseAllBinds()
    {
        for (auto it = keyBinds.begin(); it != keyBinds.end();)
        {
            if ((*it)->getItemType() == ITEM_UI_OPEN)
            {
                it = std::next(it);
                continue;
            }

            const auto foundBlock = uiBlock.find((*it)->getItemPtr());
            if (foundBlock != uiBlock.end())
            {
                uiBlock.erase(foundBlock);
            }
            (*it)->setValueToOld();
            it = keyBinds.erase(it);
        }
    }

    bindList& getBindList()
    {
        return keyBinds;
    }

    void updateOtherBindStates()
    {
        bindList sortedBinds = keyBinds;
        std::stable_sort(sortedBinds.begin(), sortedBinds.end(),
            [](const auto& a, const auto& b) {
                return a->getPressed() > b->getPressed();
            });

        for (auto bind : sortedBinds)
        {
            const auto bindType = bind->getType();
            if (bind->getItemType() == ITEM_UI_OPEN)
                continue;

            if (bind->getOldType() != bind->getType())
            {
                if (bind->getPressed())
                {
                    if (bind->getType() == BIND_RELEASE)
                        bind->setValueToOld();
                }

                const auto foundBlock = uiBlock.find(bind->getItemPtr());
                if (foundBlock != uiBlock.end())
                {
                    uiBlock.erase(foundBlock);
                }

                bind->updateOldType();
                continue;
            }

            if (bindType == BIND_HOLD || bindType == BIND_TOGGLE)
                continue;

            switch (bindType)
            {
            case BIND_ALWAYS_ON:
            {
                if (uiBlock.find(bind->getItemPtr()) == uiBlock.end())
                {
                    uiBlock.insert(std::make_pair(bind->getItemPtr(), UiBlock{ false, bind->getType(), {} }));
                }

                const auto foundBlock = uiBlock.find(bind->getItemPtr());
                if (foundBlock != uiBlock.end())
                {
                    bind->setValueToNew();
                    if (!foundBlock->second.blocked)
                    {
                        foundBlock->second.blocked = true;
                        foundBlock->second.bindName = bind->getBindName();
                    }
                }
            }
            break;
            case BIND_FORCE_OFF:
            {
                if (uiBlock.find(bind->getItemPtr()) == uiBlock.end())
                {
                    uiBlock.insert(std::make_pair(bind->getItemPtr(), UiBlock{ false, bind->getType(), {} }));
                }

                const auto foundBlock = uiBlock.find(bind->getItemPtr());
                if (foundBlock != uiBlock.end())
                {
                    if (!foundBlock->second.blocked)
                    {
                        bind->setValueToOld();
                        foundBlock->second.blocked = true;
                        foundBlock->second.bindName = bind->getBindName();
                    }
                }
            }
            break;
            case BIND_RELEASE:
            {
                if (!bind->getPressed())
                {
                    if (uiBlock.find(bind->getItemPtr()) == uiBlock.end())
                    {
                        uiBlock.insert(std::make_pair(bind->getItemPtr(), UiBlock{ false, bind->getType(), {} }));
                    }

                    bool canHandleBlock = false;
                    const auto foundBlock = uiBlock.find(bind->getItemPtr());
                    if (foundBlock != uiBlock.end())
                        canHandleBlock = true;

                    if (canHandleBlock)
                    {
                        if (!foundBlock->second.blocked)
                        {
                            bind->setValueToNew();
                            foundBlock->second.blocked = true;
                            foundBlock->second.bindName = bind->getBindName();
                        }
                    }
                }
                else
                {
                    bool erased = false;
                    const auto foundBlock = uiBlock.find(bind->getItemPtr());
                    if (foundBlock != uiBlock.end())
                    {
                        if (foundBlock->second.blocked
                            && foundBlock->second.bindType == bind->getType()
                            && !foundBlock->second.bindName.compare(bind->getBindName()))
                        {
                            uiBlock.erase(foundBlock);
                            erased = true;
                        }
                    }

                    if (erased)
                        bind->setValueToOld();
                }
            }
            break;
            }
        }
    }

    void updateAliveBindValues()
    {
        for (auto bind : keyBinds)
        {
            if (bind->getItemType() == ITEM_UI_OPEN)
                continue;

            bind->setNewValue();
        }
    }

    void onKeyDown(const std::shared_ptr<IKeyBind>& bind)
    {
        const auto foundBlock = uiBlock.find(bind->getItemPtr());
        if (foundBlock != uiBlock.end())
            foundBlock->second.activeKeyCount++;
    }

    void onKeyUp(const std::shared_ptr<IKeyBind>& bind)
    {
        const auto foundBlock = uiBlock.find(bind->getItemPtr());
        if (foundBlock != uiBlock.end())
        {
            if (foundBlock->second.activeKeyCount > 0)
                foundBlock->second.activeKeyCount--;
        }
    }

    void updateMainBindStates()
    {
        bindList sortedBinds = keyBinds;
        std::stable_sort(sortedBinds.begin(), sortedBinds.end(),
            [](const auto& a, const auto& b) {
                return a->getPressed() < b->getPressed();
            });

        for (auto bind : sortedBinds)
        {
            if (bind->getType() == BIND_NONE)
                continue;

            if (bind->getItemType() == ITEM_UI_OPEN)
            {
                if (bind->getPressed())
                    bind->setValueToNew();
                else
                    bind->setValueToOld();

                continue;
            }

            if (bind->getOldType() != bind->getType())
            {
                bind->setValueToOld();
                bind->setPressed(false);

                const auto foundBlock = uiBlock.find(bind->getItemPtr());
                if (foundBlock != uiBlock.end())
                {
                    uiBlock.erase(foundBlock);
                }

                bind->updateOldType();
                continue;
            }

            if (bind->getType() == BIND_ALWAYS_ON
                || bind->getType() == BIND_FORCE_OFF
                || bind->getType() == BIND_RELEASE)
                continue;

            if (bind->getPressed())
            {
                if (uiBlock.find(bind->getItemPtr()) == uiBlock.end())
                {
                    uiBlock.insert(std::make_pair(bind->getItemPtr(), UiBlock{ false, bind->getType(), {} }));
                }

                bool canHandleBlock = false;
                const auto foundBlock = uiBlock.find(bind->getItemPtr());
                if (foundBlock != uiBlock.end())
                    canHandleBlock = true;

                if (canHandleBlock && !foundBlock->second.blocked)
                    bind->setValueToNew();

                if (canHandleBlock)
                {
                    if (!foundBlock->second.blocked)
                    {
                        foundBlock->second.blocked = true;
                        foundBlock->second.bindName = bind->getBindName();
                    }
                }
            }
            else
            {
                bool erased = false;
                const auto foundBlock = uiBlock.find(bind->getItemPtr());
                if (foundBlock != uiBlock.end())
                {
                    if (foundBlock->second.blocked
                        && foundBlock->second.bindType == bind->getType()
                        && !foundBlock->second.bindName.compare(bind->getBindName()))
                    {
                        if (foundBlock->second.activeKeyCount <= 0)
                        {
                            uiBlock.erase(foundBlock);
                            erased = true;
                        }
                    }
                }

                if (erased)
                    bind->setValueToOld();
            }
        }
    }
};

void initBinds();
void destroyBinds();
void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam);
void renderDebugBindsWindow();

WPARAM ImGui_ImplWin32_ImGuiKeyToKeyEvent(ImGuiKey imgui_key);
std::string ImGui_ImplWin32_VKeyToString(int wParam);

template<typename T>
void keyBind(const std::shared_ptr<IKeyBind> bind, typename T::iterator it)
{
    if (!it->keyEmpty)
    {
        if (ImGui::SmallButton(it->label.c_str()))
        {
            it->keyEmpty = true;
        }
    }
    else
    {
        it->label = "...";
        ImGui::SmallButton(it->label.c_str());

        if (ImGui::IsKeyDown(ImGuiKey_Escape))
        {
            it->bindKey = -1;
            it->label = "None";
            it->keyEmpty = false;
            return;
        }

        bool keyFound = false;
        for (int i = ImGuiMouseButton_Left; i < ImGuiMouseButton_COUNT; i++)
        {
            if (ImGui::GetIO().MouseDown[i])
            {
                switch (i)
                {
                case 0:
                    it->bindKey = VK_LBUTTON;
                    break;
                case 1:
                    it->bindKey = VK_RBUTTON;
                    break;
                case 2:
                    it->bindKey = VK_MBUTTON;
                    break;
                case 3:
                    it->bindKey = VK_XBUTTON1;
                    break;
                case 4:
                    it->bindKey = VK_XBUTTON2;
                    break;
                }

                it->label = ImGui_ImplWin32_VKeyToString(it->bindKey);
                it->keyEmpty = false;
                bind->setKey(it->bindKey);
                keyFound = true;
                break;
            }
        }

        if (keyFound)
            return;

        for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; ++i)
        {
            if (ImGui::IsKeyDown((ImGuiKey)i))
            {
                it->bindKey = (int)ImGui_ImplWin32_ImGuiKeyToKeyEvent((ImGuiKey)i);
                it->label = ImGui_ImplWin32_VKeyToString(it->bindKey);
                it->keyEmpty = false;
                bind->setKey(it->bindKey);
                break;
            }
        }
    }
}
}