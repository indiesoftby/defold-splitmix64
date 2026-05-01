-- LuaJIT test script — mirrors example/example.script (no Defold UI/assert branches).
-- Usage (from luajit/): make -f Makefile.luajit test

local ext = jit.os == "Windows" and ".dll" or ".so"
local platform = jit.os == "Windows" and "x86_64-win32" or "x86_64-linux"
package.cpath = "../splitmix64/bin/" .. platform .. "/?" .. ext .. ";" .. package.cpath

local splitmix64 = require("splitmix64")

local EPSILON = 0.00001

do
    local known_rng = splitmix64.new_instance(0)
    local known_doubles = {
        0.88331080821364260647,
        0.43152799704850997031,
        0.02643377159259774345,
        0.97088197815382848432,
        0.10634669156721243688,
    }
    for i, expected in ipairs(known_doubles) do
        local actual = known_rng.random()
        assert(math.abs(actual - expected) < EPSILON, "known double[" .. i .. "] mismatch: " .. actual .. " ~= " .. expected)
    end
    print("known values test passed")

    splitmix64.randomseed(123456)
    local saved_state = splitmix64.state()

    local rng = splitmix64.new_instance(42)
    assert(math.abs(rng.random() - 0.74156487877182) < EPSILON, "rng.random() mismatch")
    assert(rng.random(1, 100) == 92, "rng.random(1, 100) mismatch")
    rng.randomseed(999)
    assert(rng.state() == "999", "rng.state() mismatch")
    assert(rng.randomchoice({ "a", "b", "c" }) == "b", "rng.randomchoice mismatch")
    assert(rng.weightedchoice({ red = 1, blue = 2, green = 3 }) == "red", "rng.weightedchoice mismatch")
    assert(rng.toss() == 0, "rng.toss() mismatch")
    local rolls, total = rng.dice(2, splitmix64.D6)
    assert(rolls[1] == 4 and rolls[2] == 3, "rng.dice rolls mismatch")
    assert(total == 7, "rng.dice total mismatch")

    local orig = { 10, 20, 30, 40, 50, 60 }
    local shuffled = rng.shuffle(orig)
    assert(#shuffled == 6, "shuffle copy: wrong length")
    assert(
        orig[1] == 10 and orig[2] == 20 and orig[3] == 30 and orig[4] == 40 and orig[5] == 50 and orig[6] == 60,
        "shuffle copy: original modified"
    )
    assert(shuffled ~= orig, "shuffle copy: returned same table")
    assert(
        shuffled[1] == 30 and shuffled[2] == 10 and shuffled[3] == 40 and shuffled[4] == 50 and shuffled[5] == 20
            and shuffled[6] == 60,
        "shuffle copy: values mismatch"
    )

    local t = { 1, 2, 3, 4, 5 }
    local ret = rng.shuffle(t, true)
    assert(ret == t, "shuffle inplace: returned different table")
    assert(#t == 5, "shuffle inplace: wrong length")
    assert(t[1] == 4 and t[2] == 3 and t[3] == 5 and t[4] == 2 and t[5] == 1, "shuffle inplace: values mismatch")

    local empty = rng.shuffle({})
    assert(#empty == 0, "shuffle empty: wrong length")

    print("shuffle tests passed")

    assert(saved_state == splitmix64.state(), "module state changed by new_instance calls")
    print("new_instance test passed: module state unchanged")
end

do
    splitmix64.randomseed(1)

    assert(splitmix64.random(1000) == 466, "test1")
    assert(splitmix64.random(5, 10000) == 4884, "test2")
    assert(math.abs(splitmix64.random() - 0.9710027535868) < EPSILON, "test3")
    assert(splitmix64.random(-1000000, -5000) == -952085, "test4")
    assert(splitmix64.random(1) == 1, "test5")
    assert(math.abs(splitmix64.random() - 0.76289439191176) < EPSILON, "test6")
    print("deterministic module.random tests passed")
end

print("LuaJIT splitmix64: all tests passed")
