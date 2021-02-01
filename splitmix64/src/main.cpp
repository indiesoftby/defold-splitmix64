// Extension lib defines
#define LIB_NAME "splitmix64"
#define MODULE_NAME "splitmix64"

#include <dmsdk/sdk.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/*  Written in 2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* This is a fixed-increment version of Java 8's SplittableRandom generator
See http://dx.doi.org/10.1145/2714064.2660195 and
http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

It is a very fast generator passing BigCrush, and it can be useful if
for some reason you absolutely want 64 bits of state. */

static uint64_t x; /* The state can be seeded with any value. */

static uint64_t next() {
    uint64_t z = (x += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

#define MT64_MUL (1.0 / 9007199254740992.0)

static int Random(lua_State *L) {
    lua_Integer low, up;
    switch (lua_gettop(L)) { /* check number of arguments */
    case 0: {                /* no arguments */
        double r = (double)(next() >> 11) * MT64_MUL;
        lua_pushnumber(L, (lua_Number)r); /* Number between 0 and 1 */
        return 1;
    }
    case 1: { /* only upper limit */
        low = 1;
        up = luaL_checkinteger(L, 1);
        break;
    }
    case 2: { /* lower and upper limits */
        low = luaL_checkinteger(L, 1);
        up = luaL_checkinteger(L, 2);
        break;
    }
    default:
        return luaL_error(L, "wrong number of arguments");
    }
    /* random integer in the interval [low, up] */
    luaL_argcheck(L, low <= up, 1, "interval is empty");
#ifdef LUA_MAXINTEGER
    luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1, "interval too large");
#else
    luaL_argcheck(L, low >= 0 || up <= INT_MAX + low, 1, "interval too large");
#endif
    uint64_t i = next() % ((uint64_t)abs(up - low) + 1);
    lua_pushinteger(L, (lua_Integer)i + low);
    return 1;
}

static int RandomSeed(lua_State *L) {
    double arg = (double)luaL_checknumber(L, 1);
    x = (uint64_t)floor(arg);
    return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] = {{"random", Random},
                                          {"randomseed", RandomSeed},
                                          /* Sentinel: */
                                          {NULL, NULL}};

static void LuaInit(lua_State *L) {
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result InitializeExtension(dmExtension::Params *params) {
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(splitmix64, LIB_NAME, 0, 0, InitializeExtension, 0, 0, 0)
