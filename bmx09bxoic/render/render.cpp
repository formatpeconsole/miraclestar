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

decltype(&gui::binds::addNewbind) bindCallback = addNewbind;

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
                ImGui::SliderInt("Current", &getMenuInstance().testSlider, -100, 100);
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
                            gui::binds::keyBind(currentBind, selectedBind.value()->bindKey, selectedBind.value()->foundKey);
                        }

                        ImGui::SliderInt(valueName.c_str(), &selectedBind.value()->value, -100, 100);
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::End();

            binds::renderDebugBindsWindow();
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