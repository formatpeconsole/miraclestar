#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>

#include "../render/render.h"

namespace gui
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
        ITEM_SLIDER,
        ITEM_COMBOBOX,
        ITEM_MULTICOMBOBOX,
        ITEM_COLOR,
    };

    constexpr auto MAX_BINDS = 1000;

    class IKeyBind
    {
    public:
        virtual int getType() = 0;
        virtual int getItemType() = 0;
        virtual int getKey() = 0;

        virtual void* getItemPtr() = 0;

        virtual void setType(int type) = 0;
        virtual void setItemType(int type) = 0;
        virtual void setKey(int key) = 0;

        virtual bool getPressed() = 0;
        virtual void setPressed(bool state) = 0;

        virtual void setOldValue() = 0;
        virtual void setNewValue() = 0;

        virtual void setValueToNew() = 0;
        virtual void setValueToOld() = 0;

        virtual bool isValueSet() = 0;
        virtual std::string getBindName() = 0;
        virtual std::string getBindValue() = 0;

        virtual ~IKeyBind() = default;
    };

    template<typename T>
    class KeyBind : public IKeyBind
    {
    public:
        KeyBind(T* item, T* bind, int type, int itemType, int key, std::string name)
            : itemPtr(item),
            bindItemPtr(bind),
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

        void setOldValue()
        {
            oldValue = *itemPtr;
        }

        void setNewValue()
        {
            newValue = *bindItemPtr;
            bindValue = std::to_string(newValue);
        }

        void setValueToNew()
        {
            if (!wrote)
            {
                oldValue = *itemPtr;
                wrote = true;
            }

            *itemPtr = newValue;
        }

        void setValueToOld()
        {
            *itemPtr = oldValue;

            if (wrote)
            {
                oldValue = *itemPtr;
                wrote = false;
            }
        }

    private:
        int type = BIND_NONE;
        int itemType = ITEM_NONE;
        int key = VK_INSERT;

        T* itemPtr = nullptr;
        T* bindItemPtr = nullptr;
        T oldValue{};
        T newValue{};

        bool wrote = false;
        bool pressed = false;

        std::string name{};
        std::string bindValue{};
    };

    struct UiBlock
    {
        bool blocked = false;
        int bindType = BIND_NONE;
        int activeKeyCount = 0;
    };

    using bindList = std::vector<std::shared_ptr<IKeyBind>>;
    class KeyBindManager
    {
    private:
        bindList keyBinds{};

    public:
        std::unordered_map<void*, UiBlock> uiBlock{};

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
        void addBind(T* ptr, T* bindPtr, int type, int itemType, int key, std::string name)
        {
            std::shared_ptr<IKeyBind> bind = std::make_shared<KeyBind<T>>(ptr, bindPtr, type, itemType, key, name);
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
                        bind->setValueToOld();
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
            for (auto bind : keyBinds)
            {
                if (bind->getType() == BIND_ALWAYS_ON
                    || bind->getType() == BIND_FORCE_OFF)
                    continue;

                if (bind->getItemType() == ITEM_UI_OPEN)
                {
                    if (bind->getPressed())
                        bind->setValueToNew();
                    else
                        bind->setValueToOld();

                    continue;
                }

                if (bind->getPressed())
                {
                    if (uiBlock.find(bind->getItemPtr()) == uiBlock.end())
                    {
                        uiBlock.insert(std::make_pair(bind->getItemPtr(), UiBlock{ false, bind->getType() }));
                    }

                    bool canHandleBlock = false;
                    const auto foundBlock = uiBlock.find(bind->getItemPtr());
                    if (foundBlock != uiBlock.end())
                        canHandleBlock = true;

                    if (canHandleBlock && !foundBlock->second.blocked)
                        bind->setValueToNew();

                    if (canHandleBlock)
                        foundBlock->second.blocked = true;
                }
                else
                {
                    bool erased = false;
                    const auto foundBlock = uiBlock.find(bind->getItemPtr());
                    if (foundBlock != uiBlock.end())
                    {
                        if (foundBlock->second.blocked && foundBlock->second.bindType == bind->getType())
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

    struct Menu
    {
        bool opened = false;
        bool newOpened = true;

        int testSlider = 50;
        int testSlider2 = 70;
        int testSlider3 = 100;

        int testSliderNew = 50;
        int testSliderNew2 = -100;
        int testSliderNew3 = 100;

        KeyBindManager keyBindManager{};
    };

    extern void init();
    extern void destroy();
    extern void updateAliveBindValues();
    extern void renderDebugBindsWindow();
    extern void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam);

    extern Menu& getMenuInstance();
}