#include "hooks.h"

#include "../render/render.h"
#include "../main/dllInstance.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{
hookedFuncs& getHookedFuncsInstance()
{
    static hookedFuncs instance;
    return instance;
}

LRESULT WINAPI Hooked_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto& renderInfo = render::getRenderInfoInstance();

    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) == 0)
        return 1;

    return CallWindowProcA(renderInfo.wndProcOrig, hwnd, uMsg, wParam, lParam);
}

HRESULT Hooked_Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static auto originalPresent = getHookedFuncsInstance().presentHook.getOriginal();

    if (getDllInstance().shouldQuit)
        return originalPresent(pSwapChain, SyncInterval, Flags);

    render::onRender(pSwapChain);

    return originalPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT Hooked_ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    static auto originalResizeBuffers = getHookedFuncsInstance().resizeBuffersHook.getOriginal();

    if (getDllInstance().shouldQuit)
        return originalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    render::onResize();

    return originalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT WINAPI Hooked_CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
    static auto originalCreateSwapChain = getHookedFuncsInstance().createSwapChainHook.getOriginal();

    if (getDllInstance().shouldQuit)
        return originalCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);

    render::clearRenderTargetView();

    return originalCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}
}