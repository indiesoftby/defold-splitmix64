// LuaJIT entry: same Lua API as Defold; compile ext_lua_api.cpp with -DSPLITMIX64_EXT_LUAJIT.

#include "../splitmix64/src/ext.hpp"

extern "C" int luaopen_splitmix64(lua_State *L)
{
    Splitmix64Ext::LuaOpen(L);
    return 1;
}
