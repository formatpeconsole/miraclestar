#include "render.h"

#include "../hooks/hooks.h"
#include "../gui/gui.h"

namespace render
{
using namespace gui;

void init(IDXGISwapChain* pSwapChain)
{
    if (getRenderInfoInstance().init)
        return;

    DXGI_SWAP_CHAIN_DESC SwapChainDesc;

    if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&getRenderInfoInstance().device))))
        return;

    getRenderInfoInstance().device->GetImmediateContext(&getRenderInfoInstance().deviceContext);

    if (FAILED(pSwapChain->GetDesc(&SwapChainDesc)))
        return;

    getRenderInfoInstance().cs2Window = SwapChainDesc.OutputWindow;
    getRenderInfoInstance().imguiContext = ImGui::CreateContext();

    ImGui::SetCurrentContext(getRenderInfoInstance().imguiContext);

    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui_ImplWin32_Init(getRenderInfoInstance().cs2Window);
    ImGui_ImplDX11_Init(getRenderInfoInstance().device, getRenderInfoInstance().deviceContext);

    getRenderInfoInstance().wndProcOrig = (WNDPROC)SetWindowLongPtrA(getRenderInfoInstance().cs2Window, GWLP_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
    getRenderInfoInstance().init = true;
}

void clearRenderTargetView()
{
    if (getRenderInfoInstance().renderTargetView)
    {
        getRenderInfoInstance().renderTargetView->Release();
        getRenderInfoInstance().renderTargetView = nullptr;
    }
}

void destroy()
{
    SetWindowLongPtrA(getRenderInfoInstance().cs2Window, GWLP_WNDPROC, (LONG_PTR)getRenderInfoInstance().wndProcOrig);

    clearRenderTargetView();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    getRenderInfoInstance().init = false;
}

void onResize()
{
    destroy();
}

void addNewbind(int& counter)
{
    auto& newBind = getMenuInstance().testSliderBinds.emplace_back();
    newBind.name = "Test Slider " + std::to_string(counter++);

    getMenuInstance().keyBindManager.addBind(
        &getMenuInstance().testSlider,
        &newBind.value,
        &getMenuInstance().testSliderOldValue,
        BIND_TOGGLE,
        ITEM_SLIDER,
        0,
        newBind.name
    );
}

decltype(&addNewbind) bindCallback = addNewbind;

WPARAM ImGui_ImplWin32_ImGuiKeyToKeyEvent(ImGuiKey imgui_key)
{
    if (imgui_key == ImGuiKey_KeypadEnter)
        return VK_RETURN; 

    switch (imgui_key)
    {
    case ImGuiKey_Tab: return VK_TAB;
    case ImGuiKey_LeftArrow: return VK_LEFT;
    case ImGuiKey_RightArrow: return VK_RIGHT;
    case ImGuiKey_UpArrow: return VK_UP;
    case ImGuiKey_DownArrow: return VK_DOWN;
    case ImGuiKey_PageUp: return VK_PRIOR;
    case ImGuiKey_PageDown: return VK_NEXT;
    case ImGuiKey_Home: return VK_HOME;
    case ImGuiKey_End: return VK_END;
    case ImGuiKey_Insert: return VK_INSERT;
    case ImGuiKey_Delete: return VK_DELETE;
    case ImGuiKey_Backspace: return VK_BACK;
    case ImGuiKey_Space: return VK_SPACE;
    case ImGuiKey_Enter: return VK_RETURN;
    case ImGuiKey_Escape: return VK_ESCAPE;
    case ImGuiKey_Comma: return VK_OEM_COMMA;
    case ImGuiKey_Period: return VK_OEM_PERIOD;
    case ImGuiKey_CapsLock: return VK_CAPITAL;
    case ImGuiKey_ScrollLock: return ImGuiKey_ScrollLock;
    case ImGuiKey_NumLock: return VK_NUMLOCK;
    case ImGuiKey_PrintScreen: return VK_SNAPSHOT;
    case ImGuiKey_Pause: return VK_PAUSE;
    case ImGuiKey_Keypad0: return VK_NUMPAD0;
    case ImGuiKey_Keypad1: return VK_NUMPAD1;
    case ImGuiKey_Keypad2: return VK_NUMPAD2;
    case ImGuiKey_Keypad3: return VK_NUMPAD3;
    case ImGuiKey_Keypad4: return VK_NUMPAD4;
    case ImGuiKey_Keypad5: return VK_NUMPAD5;
    case ImGuiKey_Keypad6: return VK_NUMPAD6;
    case ImGuiKey_Keypad7: return VK_NUMPAD7;
    case ImGuiKey_Keypad8: return VK_NUMPAD8;
    case ImGuiKey_Keypad9: return VK_NUMPAD9;
    case ImGuiKey_KeypadDecimal: return VK_DECIMAL;
    case ImGuiKey_KeypadDivide: return VK_DIVIDE;
    case ImGuiKey_KeypadMultiply: return VK_MULTIPLY;
    case ImGuiKey_KeypadSubtract: return VK_SUBTRACT;
    case ImGuiKey_KeypadAdd: return VK_ADD;
    case ImGuiKey_LeftShift: return VK_LSHIFT;
    case ImGuiKey_LeftCtrl: return VK_LCONTROL;
    case ImGuiKey_LeftAlt: return VK_LMENU;
    case ImGuiKey_LeftSuper: return VK_LWIN;
    case ImGuiKey_RightShift: return VK_RSHIFT;
    case ImGuiKey_RightCtrl: return VK_RCONTROL;
    case ImGuiKey_RightAlt: return VK_RMENU;
    case ImGuiKey_RightSuper: return VK_RWIN;
    case ImGuiKey_Menu: return VK_APPS;
    case ImGuiKey_0: return '0';
    case ImGuiKey_1: return '1';
    case ImGuiKey_2: return '2';
    case ImGuiKey_3: return '3';
    case ImGuiKey_4: return '4';
    case ImGuiKey_5: return '5';
    case ImGuiKey_6: return '6';
    case ImGuiKey_7: return '7';
    case ImGuiKey_8: return '8';
    case ImGuiKey_9: return '9';
    case ImGuiKey_A: return 'A';
    case ImGuiKey_B: return 'B';
    case ImGuiKey_C: return 'C';
    case ImGuiKey_D: return 'D';
    case ImGuiKey_E: return 'E';
    case ImGuiKey_F: return 'F';
    case ImGuiKey_G: return 'G';
    case ImGuiKey_H: return 'H';
    case ImGuiKey_I: return 'I';
    case ImGuiKey_J: return 'J';
    case ImGuiKey_K: return 'K';
    case ImGuiKey_L: return 'L';
    case ImGuiKey_M: return 'M';
    case ImGuiKey_N: return 'N';
    case ImGuiKey_O: return 'O';
    case ImGuiKey_P: return 'P';
    case ImGuiKey_Q: return 'Q';
    case ImGuiKey_R: return 'R';
    case ImGuiKey_S: return 'S';
    case ImGuiKey_T: return 'T';
    case ImGuiKey_U: return 'U';
    case ImGuiKey_V: return 'V';
    case ImGuiKey_W: return 'W';
    case ImGuiKey_X: return 'X';
    case ImGuiKey_Y: return 'Y';
    case ImGuiKey_Z: return 'Z';
    case ImGuiKey_F1: return VK_F1;
    case ImGuiKey_F2: return VK_F2;
    case ImGuiKey_F3: return VK_F3;
    case ImGuiKey_F4: return VK_F4;
    case ImGuiKey_F5: return VK_F5;
    case ImGuiKey_F6: return VK_F6;
    case ImGuiKey_F7: return VK_F7;
    case ImGuiKey_F8: return VK_F8;
    case ImGuiKey_F9: return VK_F9;
    case ImGuiKey_F10: return VK_F10;
    case ImGuiKey_F11: return VK_F11;
    case ImGuiKey_F12: return VK_F12;
    case ImGuiKey_F13: return VK_F13;
    case ImGuiKey_F14: return VK_F14;
    case ImGuiKey_F15: return VK_F15;
    case ImGuiKey_F16: return VK_F16;
    case ImGuiKey_F17: return VK_F17;
    case ImGuiKey_F18: return VK_F18;
    case ImGuiKey_F19: return VK_F19;
    case ImGuiKey_F20: return VK_F20;
    case ImGuiKey_F21: return VK_F21;
    case ImGuiKey_F22: return VK_F22;
    case ImGuiKey_F23: return VK_F23;
    case ImGuiKey_F24: return VK_F24;
    case ImGuiKey_AppBack: return VK_BROWSER_BACK;
    case ImGuiKey_AppForward: return VK_BROWSER_FORWARD;
    case ImGuiKey_GraveAccent: return 41;  // `~
    case ImGuiKey_Minus: return 12;         // -
    case ImGuiKey_Equal: return 13;         // =
    case ImGuiKey_LeftBracket: return 26;   // [
    case ImGuiKey_RightBracket: return 27;  // ]
    case ImGuiKey_Oem102: return 86;        // < > \ (102-key)
    case ImGuiKey_Backslash: return 43;     // '\'
    case ImGuiKey_Semicolon: return 39;     // ;
    case ImGuiKey_Apostrophe: return 40;    // '
    case ImGuiKey_Slash: return 53;         // /

    default: break;
    }

    return 0; // Invalid key
}

std::string ImGui_ImplWin32_VKeyToString(int wParam)
{
    switch (wParam)
    {
    case VK_LBUTTON: return "LButton";
    case VK_RBUTTON: return "RButton";
    case VK_CANCEL: return "Cancel";
    case VK_MBUTTON: return "MButton";
    case VK_XBUTTON1: return "XButton1";
    case VK_XBUTTON2: return "XButton2";
    case VK_BACK: return "Backspace";
    case VK_TAB: return "Tab";
    case VK_CLEAR: return "Clear";
    case VK_RETURN: return "Enter";
    case VK_SHIFT: return "Shift";
    case VK_CONTROL: return "Ctrl";
    case VK_MENU: return "Alt";
    case VK_PAUSE: return "Pause";
    case VK_CAPITAL: return "CapsLock";
    case VK_KANA: return "Kana";
    case VK_JUNJA: return "Junja";
    case VK_FINAL: return "Final";
    case VK_HANJA: return "Hanja";
    case VK_ESCAPE: return "Escape";
    case VK_CONVERT: return "Convert";
    case VK_NONCONVERT: return "NonConvert";
    case VK_ACCEPT: return "Accept";
    case VK_MODECHANGE: return "ModeChange";
    case VK_SPACE: return "Space";
    case VK_PRIOR: return "PageUp";
    case VK_NEXT: return "PageDown";
    case VK_END: return "End";
    case VK_HOME: return "Home";
    case VK_LEFT: return "Left";
    case VK_UP: return "Up";
    case VK_RIGHT: return "Right";
    case VK_DOWN: return "Down";
    case VK_SELECT: return "Select";
    case VK_PRINT: return "Print";
    case VK_EXECUTE: return "Execute";
    case VK_SNAPSHOT: return "PrintScreen";
    case VK_INSERT: return "Insert";
    case VK_DELETE: return "Delete";
    case VK_HELP: return "Help";
    case VK_LWIN: return "LeftSuper";
    case VK_RWIN: return "RightSuper";
    case VK_APPS: return "Menu";
    case VK_SLEEP: return "Sleep";
    case VK_NUMPAD0: return "Keypad0";
    case VK_NUMPAD1: return "Keypad1";
    case VK_NUMPAD2: return "Keypad2";
    case VK_NUMPAD3: return "Keypad3";
    case VK_NUMPAD4: return "Keypad4";
    case VK_NUMPAD5: return "Keypad5";
    case VK_NUMPAD6: return "Keypad6";
    case VK_NUMPAD7: return "Keypad7";
    case VK_NUMPAD8: return "Keypad8";
    case VK_NUMPAD9: return "Keypad9";
    case VK_MULTIPLY: return "KeypadMultiply";
    case VK_ADD: return "KeypadAdd";
    case VK_SEPARATOR: return "Separator";
    case VK_SUBTRACT: return "KeypadSubtract";
    case VK_DECIMAL: return "KeypadDecimal";
    case VK_DIVIDE: return "KeypadDivide";
    case VK_F1: return "F1";
    case VK_F2: return "F2";
    case VK_F3: return "F3";
    case VK_F4: return "F4";
    case VK_F5: return "F5";
    case VK_F6: return "F6";
    case VK_F7: return "F7";
    case VK_F8: return "F8";
    case VK_F9: return "F9";
    case VK_F10: return "F10";
    case VK_F11: return "F11";
    case VK_F12: return "F12";
    case VK_F13: return "F13";
    case VK_F14: return "F14";
    case VK_F15: return "F15";
    case VK_F16: return "F16";
    case VK_F17: return "F17";
    case VK_F18: return "F18";
    case VK_F19: return "F19";
    case VK_F20: return "F20";
    case VK_F21: return "F21";
    case VK_F22: return "F22";
    case VK_F23: return "F23";
    case VK_F24: return "F24";
    case VK_NUMLOCK: return "NumLock";
    case VK_SCROLL: return "ScrollLock";
    case VK_LSHIFT: return "LeftShift";
    case VK_RSHIFT: return "RightShift";
    case VK_LCONTROL: return "LeftCtrl";
    case VK_RCONTROL: return "RightCtrl";
    case VK_LMENU: return "LeftAlt";
    case VK_RMENU: return "RightAlt";
    case VK_BROWSER_BACK: return "AppBack";
    case VK_BROWSER_FORWARD: return "AppForward";
    case VK_BROWSER_REFRESH: return "BrowserRefresh";
    case VK_BROWSER_STOP: return "BrowserStop";
    case VK_BROWSER_SEARCH: return "BrowserSearch";
    case VK_BROWSER_FAVORITES: return "BrowserFavorites";
    case VK_BROWSER_HOME: return "BrowserHome";
    case VK_VOLUME_MUTE: return "VolumeMute";
    case VK_VOLUME_DOWN: return "VolumeDown";
    case VK_VOLUME_UP: return "VolumeUp";
    case VK_MEDIA_NEXT_TRACK: return "MediaNext";
    case VK_MEDIA_PREV_TRACK: return "MediaPrev";
    case VK_MEDIA_STOP: return "MediaStop";
    case VK_MEDIA_PLAY_PAUSE: return "MediaPlayPause";
    case VK_LAUNCH_MAIL: return "LaunchMail";
    case VK_LAUNCH_MEDIA_SELECT: return "LaunchMedia";
    case VK_LAUNCH_APP1: return "LaunchApp1";
    case VK_LAUNCH_APP2: return "LaunchApp2";

        // OEM клавиши (зависят от раскладки)
    case VK_OEM_1: return "OEM_1";      // ;:/
    case VK_OEM_PLUS: return "OEM_PLUS"; // =+
    case VK_OEM_COMMA: return "OEM_COMMA"; // ,<
    case VK_OEM_MINUS: return "OEM_MINUS"; // -_
    case VK_OEM_PERIOD: return "OEM_PERIOD"; // .>
    case VK_OEM_2: return "OEM_2";      // /?
    case VK_OEM_3: return "OEM_3";      // `~
    case VK_OEM_4: return "OEM_4";      // [{ 
    case VK_OEM_5: return "OEM_5";      // \|
    case VK_OEM_6: return "OEM_6";      // ]}
    case VK_OEM_7: return "OEM_7";      // '"
    case VK_OEM_8: return "OEM_8";
    case VK_OEM_AX: return "OEM_AX";
    case VK_OEM_102: return "OEM_102";
    case VK_ICO_HELP: return "ICO_HELP";
    case VK_ICO_00: return "ICO_00";

    case '0': return "0";
    case '1': return "1";
    case '2': return "2";
    case '3': return "3";
    case '4': return "4";
    case '5': return "5";
    case '6': return "6";
    case '7': return "7";
    case '8': return "8";
    case '9': return "9";

    case 'A': return "A";
    case 'B': return "B";
    case 'C': return "C";
    case 'D': return "D";
    case 'E': return "E";
    case 'F': return "F";
    case 'G': return "G";
    case 'H': return "H";
    case 'I': return "I";
    case 'J': return "J";
    case 'K': return "K";
    case 'L': return "L";
    case 'M': return "M";
    case 'N': return "N";
    case 'O': return "O";
    case 'P': return "P";
    case 'Q': return "Q";
    case 'R': return "R";
    case 'S': return "S";
    case 'T': return "T";
    case 'U': return "U";
    case 'V': return "V";
    case 'W': return "W";
    case 'X': return "X";
    case 'Y': return "Y";
    case 'Z': return "Z";

    default:
        return {};
        break;
    }
}

void keyBind(const std::shared_ptr<IKeyBind> bind, int& key)
{
    static std::string label = "Press any key";
    static bool searchingKey = false;

    if (!searchingKey)
    {
        if (ImGui::SmallButton(label.c_str()))
        {
            searchingKey = true;
        }
    }
    else
    {
        label = "...";
        ImGui::SmallButton(label.c_str());

        for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; ++i)
        {
            if (ImGui::IsKeyDown((ImGuiKey)i))
            {
                key = ImGui_ImplWin32_ImGuiKeyToKeyEvent((ImGuiKey)i);
                label = ImGui_ImplWin32_VKeyToString(key);
                searchingKey = false;
                bind->setKey(key);
                break;
            }
        }
    }
}

void onRender(IDXGISwapChain* pSwapChain)
{
    if (!getRenderInfoInstance().init)
        init(pSwapChain);
    else
    {
        if (!getRenderInfoInstance().renderTargetView)
        {
            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

            D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
            memset(&RenderTargetDesc, 0, sizeof(RenderTargetDesc));

            RenderTargetDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

            if (pBackBuffer)
            {
                getRenderInfoInstance().device->CreateRenderTargetView(pBackBuffer, &RenderTargetDesc, &getRenderInfoInstance().renderTargetView);
                pBackBuffer->Release();
            }
        }

        ImGui::SetCurrentContext(getRenderInfoInstance().imguiContext);

        getRenderInfoInstance().deviceContext->OMGetRenderTargets(1, &getRenderInfoInstance().mainRenderTarget, 0);
        getRenderInfoInstance().deviceContext->OMSetRenderTargets(1, &getRenderInfoInstance().renderTargetView, 0);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        if (getMenuInstance().opened)
        {
            ImGui::SetNextWindowSize(ImVec2(780, 650));
            ImGui::Begin("hi", &getMenuInstance().opened, ImGuiWindowFlags_NoResize);
            {
                ImGui::SliderInt("Current", &getMenuInstance().testSlider, 0, 100);
                ImGui::SameLine();
                if (ImGui::SmallButton("..."))
                {
                    ImGui::OpenPopup("bind_popup");
                }

                static int counter = 0;
                if (ImGui::BeginPopup("bind_popup"))
                {
                    ImGui::Text("Binds");
                    std::string bindCombo = "##binds-for-" + std::to_string(reinterpret_cast<uintptr_t>(&getMenuInstance().testSlider));

                    static int selection{};
                    std::string preview{};

                    static std::optional<std::list<sliderBindInt>::iterator> selectedBind = {};
                    if (getMenuInstance().testSliderBinds.empty())
                        preview = "No binds.";
                    else
                    {
                        if (selectedBind.has_value() && selectedBind.value() != getMenuInstance().testSliderBinds.end())
                            preview = selectedBind.value()->name;
                    }

                    if (ImGui::BeginCombo(bindCombo.c_str(), preview.c_str()))
                    {
                        if (getMenuInstance().testSliderBinds.empty())
                        {
                            ImGui::Text("No binds found! Click + to add bind");

                            if (ImGui::SmallButton("+##bind_add"))
                            {
                                bindCallback(counter);
                                selection = 0;
                                selectedBind.reset();
                            }
                        }
                        else
                        {
                            static bool wasErased = false;
                            int bindsIter = 0;
                            auto& sliderBinds = getMenuInstance().testSliderBinds;
                            for (auto it = sliderBinds.begin(); it != sliderBinds.end();)
                            {
                                if (wasErased && selection == bindsIter)
                                {
                                    selectedBind = it;
                                    wasErased = false;
                                }

                                if (ImGui::Selectable(it->name.c_str(), selection == bindsIter, 0, ImVec2(100, 15)))
                                {
                                    selection = bindsIter;
                                    selectedBind = it;
                                }

                                ImGui::SameLine();
                                ++bindsIter;

                                const std::string bindPlus = "+##bind_" + it->name;
                                const std::string bindMinus = "-##bind_" + it->name;

                                if (ImGui::SmallButton(bindPlus.c_str()))
                                {
                                    bindCallback(counter);
                                    selectedBind.reset();
                                    selection = bindsIter;
                                    continue;
                                }

                                ImGui::SameLine();

                                if (ImGui::SmallButton(bindMinus.c_str()))
                                {
                                    getMenuInstance().keyBindManager.removeBind(getMenuInstance().keyBindManager.findBind(&it->value)->getBindName());
                                    it = sliderBinds.erase(it);
                                    selectedBind.reset();
                                    selection = bindsIter;
                                    wasErased = true;
                                    continue;
                                }

                                ++it;
                            }
                        }

                        ImGui::EndCombo();
                    }

                    if (selectedBind.has_value())
                    {
                        std::string valueName = "Value ##" + selectedBind.value()->name;
                        std::string bindType = "##bind-type-to" + selectedBind.value()->name;

                        auto currentBind = getMenuInstance().keyBindManager.findBind(&selectedBind.value()->value);
                        if (currentBind != nullptr)
                        {
                            ImGui::Text("Type");
                            ImGui::Combo(bindType.c_str(), &selectedBind.value()->bindMode, "Always On\0Hold\0Toggle\0Release\0Force Off\0");

                            currentBind->setType(selectedBind.value()->bindMode + 1);

                            ImGui::Text("Current Key");
                            ImGui::SameLine();
                            keyBind(currentBind, selectedBind.value()->bindKey);
                        }

                        ImGui::SliderInt(valueName.c_str(), &selectedBind.value()->value, -100, 100);
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::End();

            renderDebugBindsWindow();
        }

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        getRenderInfoInstance().deviceContext->OMSetRenderTargets(1, &getRenderInfoInstance().mainRenderTarget, 0);
    }
}

RenderInfo& getRenderInfoInstance()
{
    static RenderInfo renderInfo;
    return renderInfo;
}
}