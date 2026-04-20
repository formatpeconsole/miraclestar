#pragma once

#include <vector>
#include <memory>

#include "../render/render.h"

namespace gui
{
enum BindType
{
    BIND_NONE = 0,
    BIND_ALWAYS_ON,
    BIND_TOGGLE,
    BIND_HOLD,
    BIND_RELEASE,
    BIND_FORCE_OFF,
};

constexpr auto MAX_BINDS = 1000;

class IKeyBind
{
public:
    virtual int getType() = 0;
    virtual int getKey() = 0;

    virtual void setType(int type) = 0;
    virtual void setKey(int key) = 0;

    virtual bool getPressed() = 0;
    virtual void setPressed(bool state) = 0;

    virtual void setOldValue() = 0;
    virtual void setNewValue() = 0;

    virtual void setValueToNew() = 0;
    virtual void setValueToOld() = 0;

    virtual ~IKeyBind() = default;
};

template<typename T>
class KeyBind : public IKeyBind
{
public:
    KeyBind(T* item, T* bind, int type, int key)
        : itemPtr(item), bindItemPtr(bind), type(type), key(key) {
    }

    int getType() override
    {
        return type;
    }

    int getKey() override
    {
        return key;
    }

    void setType(int other) override
    {
        type = other;
    }

    void setKey(int other) override
    {
        key = other;
    }

    bool getPressed() override
    {
        return pressed;
    }

    void setPressed(bool state) override
    {
        pressed = state;
    }

    void setOldValue()
    {
        oldValue = *itemPtr;
    }

    void setNewValue()
    {
        newValue = *bindItemPtr;
    }

    void setValueToNew()
    {
        *itemPtr = newValue;
    }

    void setValueToOld()
    {
        *itemPtr = oldValue;
    }

private:
    int type = BIND_NONE;
    int key = VK_INSERT;

    T* itemPtr = nullptr;
    T* bindItemPtr = nullptr;
    T oldValue{};
    T newValue{};

    bool pressed = false;
};

using bindList = std::vector<std::shared_ptr<IKeyBind>>;
class KeyBindManager
{
private:
    bindList keyBinds{};

public:
    void init()
    {
        keyBinds.reserve(MAX_BINDS);
    }

    void destroy()
    {
        keyBinds.clear();
    }

    template<typename T>
    void addBind(T* ptr, T* bindPtr, int type, int key)
    {
        std::shared_ptr<IKeyBind> bind = std::make_shared<KeyBind<T>>(ptr, bindPtr, type, key);
        keyBinds.emplace_back(bind);
    }

    bindList& getBindList()
    {
        return keyBinds;
    }

    void updateOtherBindStates()
    {
        for (auto bind : keyBinds)
        {
            const auto bindType = bind->getType();
            if (bindType != BIND_ALWAYS_ON && bindType != BIND_FORCE_OFF)
                continue;

            bind->setPressed(bindType == BIND_ALWAYS_ON ? true : false);

            switch (bindType)
            {
            case BIND_ALWAYS_ON:
            {
                if (bind->getPressed())
                    bind->setValueToNew();
            }
            break;
            case BIND_FORCE_OFF:
            {
                if (!bind->getPressed())
                    bind->setValueToNew();
            }
            break;
            }
        }
    }
};

struct Menu
{
    bool opened = false;
    bool newOpened = true;

    KeyBindManager keyBindManager{};
};

extern void init();
extern void destroy();
extern void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam);

extern Menu& getMenuInstance();
}