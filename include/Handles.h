#ifndef HACKLIB_HANDLES_H
#define HACKLIB_HANDLES_H

#ifdef _WIN32
#include <Windows.h>
#endif


namespace hl
{
#ifdef _WIN32

/// Represents a module handle. Can interpreted as pointer to the module base address.
using ModuleHandle = HINSTANCE;
/// Represents a window handle.
using WindowHandle = HWND;
/// Represents a null value for hl::ModuleHandle.
static const ModuleHandle NullModuleHandle = NULL;

#else

/// Represents a module handle. Analogous to Windows this represents the module base address, not a dlopen handle!
using ModuleHandle = void*;
/// Represents a window handle. Must be compatible with X11 Window type.
using WindowHandle = long unsigned int;
/// Represents a null value for hl::ModuleHandle.
static const ModuleHandle NullModuleHandle = nullptr;

#endif
}

#endif
