--[[
SplitMix64 API documentation
Fast pseudo-random number generator native extension for the Defold game engine
--]]

---@class defold_api.splitmix64
splitmix64 = {}

---@alias splitmix64.DICE
---|`splitmix64.D4`
---|`splitmix64.D6`
---|`splitmix64.D8`
---|`splitmix64.D10`
---|`splitmix64.D12`
---|`splitmix64.D20`
---|`splitmix64.D100`

---Dice type constant for 4-sided die.
---@type number
splitmix64.D4 = nil

---Dice type constant for 6-sided die.
---@type number
splitmix64.D6 = nil

---Dice type constant for 8-sided die.
---@type number
splitmix64.D8 = nil

---Dice type constant for 10-sided die (0-9).
---@type number
splitmix64.D10 = nil

---Dice type constant for 12-sided die.
---@type number
splitmix64.D12 = nil

---Dice type constant for 20-sided die.
---@type number
splitmix64.D20 = nil

---Dice type constant for percentile die (0, 10, 20, ..., 90).
---@type number
splitmix64.D100 = nil

---When called without arguments, returns a uniform pseudo-random real number in the range [0,1).
---When called with an integer number m, returns a uniform pseudo-random integer in the range [1, m].
---When called with two integer numbers m and n, returns a uniform pseudo-random integer in the range [m, n].
---@param m? number Upper bound (or lower bound if n is also provided)
---@param n? number Upper bound when m is the lower bound
---@return number value Pseudo-random number
function splitmix64.random(m, n) end

---Sets x as the "seed" for the pseudo-random generator: equal seeds produce equal sequences of numbers.
---Accepts a number or a string (decimal representation of uint64) for full 64-bit precision.
---@param x number|string Seed value as number or string (e.g. "18446744073709551615" for full uint64 range)
function splitmix64.randomseed(x) end

---Returns the current internal state as a string. Use this to save/restore the RNG state or pass
---to randomseed for full 64-bit precision (Lua double cannot represent all uint64 values).
---@return string state Current state as a decimal string
function splitmix64.state() end

---Returns a random value from array t. Raises an error if the array is empty.
---@param t table Array of values to choose from
---@return any value Random element from the array
function splitmix64.randomchoice(t) end

---Takes a table where keys are choices and values are weights. Returns a random key.
---Keys are sorted deterministically (numbers first, then strings lexicographically) before selection,
---so the result is reproducible for the same seed. Weights must be >= 0.
---Raises an error if all weights are 0 or any weight is negative.
---@param t table Table of key-weight pairs
---@return any key Random key chosen by weight
function splitmix64.weightedchoice(t) end

---Toss a coin. Returns 0 or 1.
---@return number value 0 or 1
function splitmix64.toss() end

---Roll one or more dice of the given type. Returns a table of individual rolls and the total sum.
---@param roll number Number of dice to roll (must be > 0)
---@param type splitmix64.DICE Dice type (splitmix64.D4, D6, D8, D10, D12, D20, or D100)
---@return table rolls Table of individual roll results
---@return number total Sum of all rolls
function splitmix64.dice(roll, type) end

---Shuffles an array using the Fisher-Yates algorithm.
---Returns a shuffled copy of the array, or if inplace is true, shuffles the original array in-place and returns it.
---@param t table Array to shuffle
---@param inplace? boolean If true, shuffle the array in-place instead of creating a copy. Defaults to false
---@return table result The shuffled array (a new table, or the same table if inplace is true)
function splitmix64.shuffle(t, inplace) end

---Creates a new RNG instance with independent state. Optional seed (number or string) for reproducibility.
---The returned instance has the same API as the module but with isolated state:
---random, randomseed, state, randomchoice, weightedchoice, toss, dice, shuffle.
---@param seed? number|string Optional seed for the new instance
---@return userdata rng RNG instance with methods matching the splitmix64 module API
function splitmix64.new_instance(seed) end
