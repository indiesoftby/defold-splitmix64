// Extension lib defines
#define LIB_NAME "splitmix64"
#define MODULE_NAME "splitmix64"

#include <dmsdk/sdk.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
    uint64_t x;
} Splitmix64State;

static uint64_t next(Splitmix64State *s) {
    uint64_t z = (s->x += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

#define MT64_MUL (1.0 / 9007199254740992.0)

static double next_double(Splitmix64State *s) {
    return (double)(next(s) >> 11) * MT64_MUL;
}

static uint64_t bounded_rand(Splitmix64State *s, uint64_t n) {
    uint64_t r = next(s);
    return r % n;
}

static Splitmix64State g_state;

static Splitmix64State *get_state(lua_State *L, int *arg_offset) {
    if (lua_type(L, lua_upvalueindex(1)) == LUA_TUSERDATA) {
        *arg_offset = 0;
        return (Splitmix64State *)lua_touserdata(L, lua_upvalueindex(1));
    }
    *arg_offset = 0;
    return &g_state;
}

static int Random(lua_State *L) {
    int off;
    Splitmix64State *s = get_state(L, &off);
    lua_Integer low, up;
    switch (lua_gettop(L) - off) {
    case 0: {
        double r = next_double(s);
        lua_pushnumber(L, (lua_Number)r);
        return 1;
    }
    case 1:
        low = 1;
        up = luaL_checkinteger(L, 1 + off);
        break;
    case 2:
        low = luaL_checkinteger(L, 1 + off);
        up = luaL_checkinteger(L, 2 + off);
        break;
    default:
        return luaL_error(L, "wrong number of arguments");
    }
    luaL_argcheck(L, low <= up, 1 + off, "interval is empty");
#ifdef LUA_MAXINTEGER
    luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1 + off, "interval too large");
#else
    luaL_argcheck(L, low >= 0 || up <= INT_MAX + low, 1 + off, "interval too large");
#endif
    uint64_t n = (uint64_t)abs(up - low) + 1;
    uint64_t i = bounded_rand(s, n);
    lua_pushinteger(L, (lua_Integer)i + low);
    return 1;
}

static int RandomSeed(lua_State *L) {
    int off;
    Splitmix64State *s = get_state(L, &off);
    int arg_idx = 1 + off;
    int arg_type = lua_type(L, arg_idx);
    if (arg_type == LUA_TSTRING) {
        const char *str = lua_tostring(L, arg_idx);
        char *end = NULL;
        unsigned long long val = strtoull(str, &end, 10);
        if (end == str || *end != '\0') {
            return luaL_error(L, "invalid seed string: expected decimal number");
        }
        s->x = (uint64_t)val;
    } else if (arg_type == LUA_TNUMBER) {
        double arg = (double)luaL_checknumber(L, arg_idx);
        s->x = (uint64_t)floor(arg);
    } else {
        return luaL_argerror(L, arg_idx, "expected number or string");
    }
    return 0;
}

static int RandomState(lua_State *L) {
    int off;
    Splitmix64State *s = get_state(L, &off);
    char buf[32];
    snprintf(buf, sizeof(buf), "%" PRIu64, s->x);
    lua_pushstring(L, buf);
    return 1;
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
    int off;
    Splitmix64State *s = get_state(L, &off);
    uint64_t r = bounded_rand(s, 2);
    lua_pushinteger(L, (lua_Integer)r);
    return 1;
}

static int RandomChoice(lua_State *L) {
    int off;
    Splitmix64State *s = get_state(L, &off);
    int tidx = 1 + off;
    luaL_checktype(L, tidx, LUA_TTABLE);
    lua_Integer len = lua_objlen(L, tidx);
    if (len == 0) {
        return luaL_error(L, "table is empty");
    }
    lua_Integer idx = (lua_Integer)bounded_rand(s, (uint64_t)len) + 1;
    lua_rawgeti(L, tidx, (int)idx);
    return 1;
}

static int compare_keys(lua_State *L, int a, int b) {
    int ta = lua_type(L, a);
    int tb = lua_type(L, b);
    if (ta != tb) return ta - tb;
    switch (ta) {
    case LUA_TNUMBER: {
        lua_Number na = lua_tonumber(L, a);
        lua_Number nb = lua_tonumber(L, b);
        return (na < nb) ? -1 : (na > nb) ? 1 : 0;
    }
    case LUA_TSTRING: {
        size_t la, lb;
        const char *sa = lua_tolstring(L, a, &la);
        const char *sb = lua_tolstring(L, b, &lb);
        size_t min_len = (la < lb) ? la : lb;
        int cmp = memcmp(sa, sb, min_len);
        if (cmp != 0) return cmp;
        return (la < lb) ? -1 : (la > lb) ? 1 : 0;
    }
    case LUA_TBOOLEAN:
        return lua_toboolean(L, a) - lua_toboolean(L, b);
    default: {
        const void *pa = lua_topointer(L, a);
        const void *pb = lua_topointer(L, b);
        return (pa < pb) ? -1 : (pa > pb) ? 1 : 0;
    }
    }
}

static int WeightedChoice(lua_State *L) {
    int off;
    Splitmix64State *s = get_state(L, &off);
    int tidx = 1 + off;
    luaL_checktype(L, tidx, LUA_TTABLE);

    lua_Number sum = 0;
    int count = 0;
    lua_pushnil(L);
    while (lua_next(L, tidx) != 0) {
        lua_Number w = luaL_checknumber(L, -1);
        if (w < 0) {
            return luaL_error(L, "weight value less than zero");
        }
        sum += w;
        count++;
        lua_pop(L, 1);
    }
    if (count == 0) {
        return luaL_error(L, "table is empty");
    }
    if (sum == 0) {
        return luaL_error(L, "all weights are zero");
    }

    luaL_checkstack(L, count + 4, "too many weight entries");

    int base = lua_gettop(L) + 1;
    lua_pushnil(L);
    while (lua_next(L, tidx) != 0) {
        lua_pop(L, 1);
        lua_pushvalue(L, -1);
        lua_insert(L, -2);
    }

    for (int j = 1; j < count; j++) {
        lua_pushvalue(L, base + j);
        int tmp_idx = lua_gettop(L);
        int k = j - 1;
        while (k >= 0) {
            int cmp = compare_keys(L, base + k, tmp_idx);
            if (cmp <= 0) break;
            lua_pushvalue(L, base + k);
            lua_replace(L, base + k + 1);
            k--;
        }
        lua_replace(L, base + k + 1);
    }

    lua_Number rnd = next_double(s) * sum;
    for (int j = 0; j < count; j++) {
        lua_pushvalue(L, base + j);
        lua_gettable(L, tidx);
        lua_Number w = lua_tonumber(L, -1);
        lua_pop(L, 1);
        if (rnd < w) {
            lua_pushvalue(L, base + j);
            return 1;
        }
        rnd -= w;
    }

    lua_pushvalue(L, base + count - 1);
    return 1;
}

static int Dice(lua_State *L) {
    int off;
    Splitmix64State *s = get_state(L, &off);
    lua_Integer roll_count = luaL_checkinteger(L, 1 + off);
    if (roll_count <= 0) {
        return luaL_error(L, "roll must be bigger than 0");
    }
    lua_Integer type_val = luaL_checkinteger(L, 2 + off);
    int type = (int)type_val;

    lua_createtable(L, (int)roll_count, (int)roll_count);
    int table_idx = lua_gettop(L);
    lua_Integer total = 0;

    for (lua_Integer i = 0; i < roll_count; ++i) {
        lua_Integer num;
        if (type == D100) {
            num = (lua_Integer)bounded_rand(s, 10) * 10;
        } else if (type == D10) {
            num = (lua_Integer)bounded_rand(s, 10);
        } else if (type == D4 || type == D6 || type == D8 || type == D12 || type == D20) {
            num = (lua_Integer)bounded_rand(s, (uint64_t)type) + 1;
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

static int NewInstance(lua_State *L) {
    int nargs = lua_gettop(L);
    uint64_t seed = 0;
    if (nargs >= 1) {
        int arg_type = lua_type(L, 1);
        if (arg_type == LUA_TSTRING) {
            const char *str = lua_tostring(L, 1);
            char *end = NULL;
            unsigned long long val = strtoull(str, &end, 10);
            if (end == str || *end != '\0') {
                return luaL_error(L, "invalid seed string: expected decimal number");
            }
            seed = (uint64_t)val;
        } else if (arg_type == LUA_TNUMBER) {
            double arg = (double)luaL_checknumber(L, 1);
            seed = (uint64_t)floor(arg);
        } else {
            return luaL_argerror(L, 1, "expected number or string");
        }
    }
    Splitmix64State *s = (Splitmix64State *)lua_newuserdata(L, sizeof(Splitmix64State));
    s->x = seed;
    int ud_idx = lua_gettop(L);

    lua_createtable(L, 0, 14);
    int tbl_idx = lua_gettop(L);

#define PUSH_METHOD(name, func) \
    lua_pushvalue(L, ud_idx); \
    lua_pushcclosure(L, func, 1); \
    lua_setfield(L, tbl_idx, name);
    PUSH_METHOD("random", Random)
    PUSH_METHOD("randomseed", RandomSeed)
    PUSH_METHOD("state", RandomState)
    PUSH_METHOD("randomchoice", RandomChoice)
    PUSH_METHOD("weightedchoice", WeightedChoice)
    PUSH_METHOD("toss", Toss)
    PUSH_METHOD("dice", Dice)
#undef PUSH_METHOD

    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] = {{"random", Random},
                                          {"randomseed", RandomSeed},
                                          {"state", RandomState},
                                          {"randomchoice", RandomChoice},
                                          {"weightedchoice", WeightedChoice},
                                          {"toss", Toss},
                                          {"dice", Dice},
                                          {"new_instance", NewInstance},
                                          /* Sentinel: */
                                          {NULL, NULL}};

static void LuaInit(lua_State *L) {
    int top = lua_gettop(L);

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
