#include <Windows.h>
#include <iostream>
#include <chrono>

#include "dllInstance.h"

#include "../threadpool/threadpool.h"

#include "../mem/sigscan.h"
#include "../in-game/sigs/sigs-handles.h"

using namespace std::chrono_literals;

Dll& getDllInstance()
{
    static Dll ptr;
    return ptr;
}

static void findModuleHandles()
{
    DEBUG_LOG("searching module handles...");

    auto& moduleHandles = getModuleHandlesInstance();
    moduleHandles.gameoverlayrenderer64     = GetModuleHandleA("GameOverlayRenderer64.dll");

    DEBUG_LOG("[+]");
}

static void initDllSignatures()
{
    DEBUG_LOG("scanning signatures...");

    auto& moduleHandles = getModuleHandlesInstance();

    auto& signatures = getSignaturesInstance();
    signatures.overlay_Present          = sigscan::find(moduleHandles.gameoverlayrenderer64, "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8");
    signatures.overlay_ResizeBuffers    = sigscan::find(moduleHandles.gameoverlayrenderer64, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC ? 44 8B E2");

    DEBUG_LOG("[+]");
}

static void hookGameFunctions()
{
    DEBUG_LOG("hooking functions...");



    DEBUG_LOG("[+]");
}

static void unHookGameFunctions()
{
    DEBUG_LOG("destroying hooks...");



    DEBUG_LOG("[+]");
}

void APIENTRY Main(HMODULE handle)
{
    createConsole();

    auto& threadPool = getSimpleThreadPoolInstance();
    threadPool.init();

    DEBUG_LOG("creating instance...");

    auto& instance = getDllInstance();

    DEBUG_LOG("[+]");
    DEBUG_LOG("continue...");

    findModuleHandles();
    initDllSignatures();
    hookGameFunctions();

    const auto state = MessageBoxA(0, "HI", 0, 0);
    if (state == IDOK)
        instance.shouldQuit = true;

    while (!instance.shouldQuit)
        std::this_thread::sleep_for(1s);

    threadPool.destroy();
    unHookGameFunctions();

    destroyConsole();
    FreeLibraryAndExitThread(handle, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, 0);
    }
    break;
    }
    return TRUE;
}