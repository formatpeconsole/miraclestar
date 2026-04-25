#include "render.h"

#include "../hooks/hooks.h"
#include "../gui/gui.h"
#include "../guiItems/items.h"
#include "../config/config.h"

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

    RegisterHotKey(getRenderInfoInstance().cs2Window, 100, 0, VK_MENU);
    RegisterHotKey(getRenderInfoInstance().cs2Window, 100, 0, VK_LMENU);
    RegisterHotKey(getRenderInfoInstance().cs2Window, 100, 0, VK_RMENU);

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
                using namespace gui::items;
                checkbox::render(getMenuInstance().rage.enable);
                combobox::render(getMenuInstance().rage.targetSelection);
                slider::render(getMenuInstance().rage.hitChance);
                slider::render(getMenuInstance().rage.minDamage);
                multicombobox::render(getMenuInstance().rage.hitBoxes);
                colorpicker::render(getMenuInstance().rage.aimHitboxColor);

                if (ImGui::SmallButton("Save"))
                    config::saveConfig();
                ImGui::SameLine();
                if (ImGui::SmallButton("Load"))
                    config::loadConfig();
            }
            ImGui::End();
        }

        binds::renderDebugBindsWindow();

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