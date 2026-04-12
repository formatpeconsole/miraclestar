#pragma once
#include <memory>
#include <algorithm>
#include <cstdint>

class Ptr
{
public:
    Ptr() : ptr(0) {}
    Ptr(uintptr_t other) : ptr(other) {}

    template<typename U>
    U As()
    {
        return reinterpret_cast<U>(ptr);
    }

    Ptr Add(uintptr_t offset)
    {
        return Ptr(ptr + offset);
    }

    Ptr Sub(uintptr_t offset)
    {
        return Ptr(ptr - offset);
    }

    Ptr Deref()
    {
        return *reinterpret_cast<uintptr_t*>(ptr);
    }

    uintptr_t Raw() const
    {
        return ptr;
    }

private:
    uintptr_t ptr;
};