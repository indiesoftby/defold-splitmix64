# LuaJIT C Module — splitmix64.dll / splitmix64.so

Build the SplitMix64 Defold extension as a native LuaJIT module outside the engine. The Lua API matches `splitmix64.*` in Defold (`random`, `randomseed`, `new_instance`, dice constants, etc.).

## Build on Linux / macOS

### Prerequisites

- `g++` (C++14), `make`
- `libluajit-5.1-dev` (headers + link library)

Ubuntu:

```
sudo apt install g++ make libluajit-5.1-dev
```

### Building

```
cd luajit
make -f Makefile.luajit
```

Output: `../splitmix64/bin/x86_64-linux/splitmix64.so` (or `x86_64-win32` / `.dll` when cross-compiling from MinGW).

### Testing

```
cd luajit
make -f Makefile.luajit test
```

## Build on Windows x64

### Prerequisites

1. **LuaJIT** — `winget install --id DEVCOM.LuaJIT`  
   Files under `%LOCALAPPDATA%\Programs\LuaJIT`: `bin\luajit.exe`, `bin\lua51.dll`, `include\`, `lib\lua51.lib`.

2. **MSVC 2019** (Community or Build Tools) — `cl.exe`, `nmake.exe`.

### Building

From **VS 2019 Developer Command Prompt**:

```
cd luajit
nmake -f Makefile.luajit.msvc
```

Output: `..\splitmix64\bin\x86_64-win32\splitmix64.dll`.

Copy `lua51.dll` next to the module or on `PATH` when running `luajit`.

Clean:

```
nmake -f Makefile.luajit.msvc clean
```

### Testing

```
cd luajit
luajit test_splitmix64.lua
```

(Adjust `package.cpath` in the test script if your `.dll` path differs.)

## How it works

- **Shared code:** `splitmix64/src/ext_lua_api.cpp` implements the full module; Defold loads it via `ext_main.cpp` + `DM_DECLARE_EXTENSION`, LuaJIT via `ext_luajit.cpp` and `luaopen_splitmix64`.
- **Flag:** `-DSPLITMIX64_EXT_LUAJIT` makes `Splitmix64Ext::LuaOpen` leave the module table on the stack for `require("splitmix64")`.
- **Shims:** `luajit/dmsdk/sdk.h` supplies only the Lua C API — no `dmScript` / `dmLog` (this extension does not use them).

**Global `splitmix64`:** `luaL_register(L, "splitmix64", …)` always creates the global `_G.splitmix64`. After `require("splitmix64")` you can call `splitmix64.random()` without assigning the return value — it is the same table `require` returns. `local splitmix64 = require("splitmix64")` is only idiomatic (local binding, slightly faster lookups).

Same PRNG and registration as the Defold build.
