// LuaJIT shim: minimal dmsdk/sdk.h — only Lua C API (SplitMix has no dmScript / dmLog).
// Replaces Defold SDK headers for standalone LuaJIT builds.

#ifndef DMSDK_SDK_H
#define DMSDK_SDK_H

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#endif
