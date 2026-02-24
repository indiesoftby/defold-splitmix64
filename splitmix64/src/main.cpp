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

static uint64_t bounded_rand(uint64_t n) {
    uint64_t r = next();
    return r % n;
}

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
    uint64_t n = (uint64_t)abs(up - low) + 1;
    uint64_t i = bounded_rand(n);
    lua_pushinteger(L, (lua_Integer)i + low);
    return 1;
}

static int RandomSeed(lua_State *L) {
    double arg = (double)luaL_checknumber(L, 1);
    x = (uint64_t)floor(arg);
    return 0;
}

enum DiceType {
    D4 = 4,
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20,
    D100 = 100
};

static int Toss(lua_State *L) {
    uint64_t r = bounded_rand(2);
    lua_pushinteger(L, (lua_Integer)r);
    return 1;
}

static int RandomChoice(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Integer len = lua_objlen(L, 1);
    if (len == 0) {
        return luaL_error(L, "table is empty");
    }
    lua_Integer idx = (lua_Integer)bounded_rand((uint64_t)len) + 1;
    lua_rawgeti(L, 1, (int)idx);
    return 1;
}

static int WeightedChoice(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Number sum = 0;
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        lua_Number w = luaL_checknumber(L, -1);
        if (w < 0) {
            return luaL_error(L, "weight value less than zero");
        }
        sum += w;
        lua_pop(L, 1);
    }
    if (sum == 0) {
        return luaL_error(L, "all weights are zero");
    }
    lua_Number rnd = (double)(next() >> 11) * MT64_MUL * sum;
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        lua_Number w = lua_tonumber(L, -1);
        lua_pushvalue(L, -2);
        lua_replace(L, 2);
        if (rnd < w) {
            lua_pop(L, 1);
            lua_pushvalue(L, 2);
            lua_remove(L, 2);
            return 1;
        }
        rnd -= w;
        lua_pop(L, 1);
    }
    lua_pushvalue(L, 2);
    lua_remove(L, 2);
    return 1;
}

static int Dice(lua_State *L) {
    lua_Integer roll_count = luaL_checkinteger(L, 1);
    if (roll_count <= 0) {
        return luaL_error(L, "roll must be bigger than 0");
    }
    lua_Integer type_val = luaL_checkinteger(L, 2);
    int type = (int)type_val;

    lua_createtable(L, (int)roll_count, (int)roll_count);
    int table_idx = lua_gettop(L);
    lua_Integer total = 0;

    for (lua_Integer i = 0; i < roll_count; ++i) {
        lua_Integer num;
        if (type == D100) {
            num = (lua_Integer)bounded_rand(10) * 10;
        } else if (type == D10) {
            num = (lua_Integer)bounded_rand(10);
        } else if (type == D4 || type == D6 || type == D8 || type == D12 || type == D20) {
            num = (lua_Integer)bounded_rand((uint64_t)type) + 1;
        } else {
            return luaL_error(L, "invalid dice type: %d (use D4, D6, D8, D10, D12, D20, D100)", type);
        }
        total += num;
        lua_pushinteger(L, num);
        lua_rawseti(L, table_idx, (int)(i + 1));
    }

    lua_pushinteger(L, total);
    return 2;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] = {{"random", Random},
                                          {"randomseed", RandomSeed},
                                          {"randomchoice", RandomChoice},
                                          {"weightedchoice", WeightedChoice},
                                          {"toss", Toss},
                                          {"dice", Dice},
                                          /* Sentinel: */
                                          {NULL, NULL}};

static void LuaInit(lua_State *L) {
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

#define SETCONSTANT(name) \
    lua_pushinteger(L, (lua_Integer)name); \
    lua_setfield(L, -2, #name);
    SETCONSTANT(D4);
    SETCONSTANT(D6);
    SETCONSTANT(D8);
    SETCONSTANT(D10);
    SETCONSTANT(D12);
    SETCONSTANT(D20);
    SETCONSTANT(D100);
#undef SETCONSTANT

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result InitializeExtension(dmExtension::Params *params) {
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(splitmix64, LIB_NAME, 0, 0, InitializeExtension, 0, 0, 0)
