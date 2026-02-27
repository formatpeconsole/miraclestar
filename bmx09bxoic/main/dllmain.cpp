#include <Windows.h>
#include <iostream>
#include <chrono>

#include "dllInstance.h"
#include "../threadpool/threadpool.h"

using namespace std::chrono_literals;

Dll& getDllInstance()
{
    static Dll ptr;
    return ptr;
}

void printRandom()
{
    auto& threadPool = getSimpleThreadPoolInstance();

    for (int i = 0; i < 100; ++i) 
    {
        if (threadPool.shouldBreakThread())
            return;

        DEBUG_LOG(i << "id-> " << std::this_thread::get_id());
        std::this_thread::sleep_for(100ms);
    }
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

    for (int i = 0; i < 5; ++i)
        threadPool.addTask(printRandom);

    threadPool.waitWhenReady();

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