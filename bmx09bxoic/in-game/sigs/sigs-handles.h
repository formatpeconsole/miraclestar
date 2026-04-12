#pragma once
#include <memory>
#include <Windows.h>

#include "../../mem/ptr.h"

struct ModuleHandles
{
    HANDLE gameoverlayrenderer64;
};

struct Signatures
{
    Ptr overlay_Present;
    Ptr overlay_ResizeBuffers;
};

extern Signatures& getSignaturesInstance();
extern ModuleHandles& getModuleHandlesInstance();
