#pragma once

#include <sstream>
#include <string>
#include <windows.h>

#ifdef _DEBUG

#include <Debugapi.h>

struct debug_print
{
    template <class C>
    debug_print& operator<<(const C& a)
    {
        OutputDebugStringA((std::stringstream() << a).str().c_str());
        return *this;
    }
};

#else

struct debug_print
{
    template <class C>
    debug_print& operator<<(const C&)
    {
        return *this;
    }
};

#endif

extern debug_print debout;
