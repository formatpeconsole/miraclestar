# 52HOOK

The main goal of this base was make working Multi-Bind system same as in other projects 

And make it works without specified UI-Backend

Key-features:
- Multi-Bind System
- Config System with Multi-Bind support
- UI Example
- ThreadPool

![](https://i.imgur.com/OkBlakE.png)

## Features of Multi-Bind system
Multi-Bind system allows to user customize any item by binding any amount of bind with different types and keys

![](https://i.imgur.com/2Yz3AIO.jpeg)

### Benefits of this implementation:
- Key-Locking. When user adds multiple binds on same item and tries to press diferrent buttons - it will lock other binds expect pressed one on this item till user stops pressing main bind.
- Binding any type of values that wont depends on UI. In this case i used ImGui for example but if you have own frame work - you can easily perform any operations with this binds system
- Customization. You are not limited in any use-keys of it. Just use binds however you want!

## Bind usage example:
- Initializing
```cpp
// First you have to create object of KeyBindManager
// In my case it's in Menu struct:
KeyBindManager keyBindManager;

// Then you have to init this manager at your Main:
getMenuInstance().keyBindManager.init();
```

- Initializing key handling
```cpp
// In your WNDProc function you have to call handleMainBinds and updateMainBindStates
// As it shown in example:
LRESULT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    gui::binds::handleMainBinds(uMsg, wParam, lParam);
    getMenuInstance().keyBindManager.updateMainBindStates();
    return CallWindowProcA(getRenderInfoInstance().wndProcOrig, hwnd, uMsg, wParam, lParam);
}

// In your render loop you have to also call other functions for make this system work:
HRESULT Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    // handles all new values that were recorded to UI 
    getMenuInstance().keyBindManager.updateAliveBindValues();

    // handles all bind states that won't react on key (Always On / Release / Force Off)
    getMenuInstance().keyBindManager.updateOtherBindStates();

    // handles other binds again due to delays in WndProc
    // makes it much safe
    getMenuInstance().keyBindManager.updateMainBindStates();

    return PresentCall(pSwapChain, SyncInterval, Flags);
}
```

- Destroying
```cpp
// Put this function at the end of your main / finish of program
getMenuInstance().keyBindManager.destroy();
```

- Add item to bind
```cpp
// In this example i added opening of UI by insert in this system
// First you have to record bind by using addBind method:

// Args are:
// ptr - pointer to your item value (in this case it's pointer too boolean for ui open)
//
// bindPtr - pointer to your bind item value
//
// oldValue - pointer to storage of previous value on bind after it gets disabled
//
// type - type of your bind:
//
//   - BIND_NONE = default, just to skip empty binds in future
//   - BIND_ALWAYS_ON
//   - BIND_HOLD
//   - BIND_TOGGLE
//   - BIND_RELEASE
//   - BIND_FORCE_OFF
//
// itemType - type of your item depends on your UI 
// in my case types are:
//
//   - ITEM_NONE = same reason as for BIND_NONE
//   - ITEM_UI_OPEN
//   - ITEM_CHECKBOX
//   - ITEM_SLIDER_INT
//   - ITEM_SLIDER_FLOAT
//   - ITEM_COMBOBOX
//   - ITEM_MULTICOMBOBOX
//   - ITEM_COLOR
//
// name - name of bind for using it in UI or Configs
// for UI open button it's empty because it won't be visualized
//
// itemName - name of your item in UI (it can be used for visualizing binds as i did in the example)
// for UI open button it's empty for the same reason as above
getMenuInstance().keyBindManager.addBind(
    &getMenuInstance().opened,
    &getMenuInstance().newOpened,
    &getMenuInstance().openedOldValue,
    BIND_TOGGLE,
    ITEM_UI_OPEN,
    VK_INSERT,
    {},
    {}
 );

// after you added your bind you HAVE to add to updating new value
// that should be used for overriding bind
// in this case value will change from true to false
// so new value will be always TRUE
// for chaning values in real-time you have to call updateAliveBindValues in your render & key handling loops
auto& bindList = getMenuInstance().keyBindManager.getBindList();
const auto menuKey = std::find_if(bindList.begin(), bindList.end(), [](const std::shared_ptr<IKeyBind>& keyBind)
    {
        return keyBind->getKey() == VK_INSERT;
    });

if (menuKey != bindList.end())
{
    auto bindToUpdate = (*menuKey);
    bindToUpdate->setOldValue();
    bindToUpdate->updateNewValue();
}
 ```
- Now you can use this system however you want :)

## Source usage
To use this source you have to install dependencies by using vcpkg (https://github.com/microsoft/vcpkg): 

```
vcpkg install --triplet-x64-windows-static
```

Enjoy! :)

## P.S.
(C) Made by miraclestar [19.04.2026-26.04.2026]
