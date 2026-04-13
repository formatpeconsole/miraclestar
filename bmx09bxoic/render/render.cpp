#include "render.h"

#include "../hooks/hooks.h"

namespace render
{
void init(IDXGISwapChain* pSwapChain)
{
    auto& renderInfo = getRenderInfoInstance();
    if (renderInfo.init)
        return;

    DXGI_SWAP_CHAIN_DESC SwapChainDesc;

    if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&renderInfo.device))))
        return;

    renderInfo.device->GetImmediateContext(&renderInfo.deviceContext);

    if (FAILED(pSwapChain->GetDesc(&SwapChainDesc)))
        return;

    renderInfo.cs2Window = SwapChainDesc.OutputWindow;
    renderInfo.imguiContext = ImGui::CreateContext();

    ImGui::SetCurrentContext(renderInfo.imguiContext);

    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui_ImplWin32_Init(renderInfo.cs2Window);
    ImGui_ImplDX11_Init(renderInfo.device, renderInfo.deviceContext);

    renderInfo.wndProcOrig = (WNDPROC)SetWindowLongPtrA(renderInfo.cs2Window, GWLP_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();

    renderInfo.init = true;
}

void clearRenderTargetView()
{
    auto& renderInfo = getRenderInfoInstance();
    if (renderInfo.renderTargetView)
    {
        renderInfo.renderTargetView->Release();
        renderInfo.renderTargetView = nullptr;
    }
}

void destroy()
{
    auto& renderInfo = getRenderInfoInstance();
    SetWindowLongPtrA(renderInfo.cs2Window, GWLP_WNDPROC, (LONG_PTR)renderInfo.wndProcOrig);

    clearRenderTargetView();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    renderInfo.init = false;
}

void onResize()
{
    destroy();
}

void onRender(IDXGISwapChain* pSwapChain)
{
    auto& renderInfo = getRenderInfoInstance();
    if (!renderInfo.init)
        init(pSwapChain);
    else
    {
        if (!renderInfo.renderTargetView)
        {
            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

            D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
            memset(&RenderTargetDesc, 0, sizeof(RenderTargetDesc));

            RenderTargetDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

            if (pBackBuffer)
            {
                renderInfo.device->CreateRenderTargetView(pBackBuffer, &RenderTargetDesc, &renderInfo.renderTargetView);
                pBackBuffer->Release();
            }
        }

        ImGui::SetCurrentContext(renderInfo.imguiContext);

        renderInfo.deviceContext->OMGetRenderTargets(1, &renderInfo.mainRenderTarget, 0);
        renderInfo.deviceContext->OMSetRenderTargets(1, &renderInfo.renderTargetView, 0);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(780, 650));
        ImGui::Begin("hi", nullptr, ImGuiWindowFlags_NoResize);
        {

        }
        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        renderInfo.deviceContext->OMSetRenderTargets(1, &renderInfo.mainRenderTarget, 0);
    }
}

RenderInfo& getRenderInfoInstance()
{
    static RenderInfo renderInfo;
    return renderInfo;
}
}