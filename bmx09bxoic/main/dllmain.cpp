#include <Windows.h>
#include <iostream>
#include <chrono>

#include "dllInstance.h"
#include "../threadpool/threadpool.h"
#include "../mem/sigscan.h"

using namespace std::chrono_literals;

Dll& getDllInstance()
{
    static Dll ptr;
    return ptr;
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

    const auto someFunc = sigscan::find(GetModuleHandleA("rendersystemdx11.dll"), "48 89 5C 24 ? 57 48 83 EC ? BF");
    DEBUG_LOG(std::hex << someFunc.Raw());

    const auto state = MessageBoxA(0, "HI", 0, 0);
    if (state == 1)
        instance.shouldQuit = true;

    while (!instance.shouldQuit)
        std::this_thread::sleep_for(1s);

    threadPool.destroy();

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