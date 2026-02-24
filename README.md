[![SplixMix64 Logo](cover.png)](https://github.com/indiesoftby/defold-splitmix64)

# SplitMix64 PRNG for Defold

This extension wraps SplitMix64 in a simple Lua API for the [Defold](https://defold.com/) game engine. The SplitMix64 algo is from [http://prng.di.unimi.it/splitmix64.c](http://prng.di.unimi.it/splitmix64.c), and it's a very fast generator passing BigCrush.

**The main idea of this extension is to get the same random numbers from the same seeds on ALL platforms supported by Defold.**

Take a look at [**the web build 🐲**](https://indiesoftby.github.io/defold-splitmix64/index.html) of the included example, which tests random values from the same seeds.

> Splitmix64 is the default pseudo-random number generator algorithm in Java and is included / available in many other languages. It uses a fairly simple algorithm that, though it is considered to be poor for cryptographic purposes, is **very fast to calculate**, and is **"good enough" for many random number needs**. It passes several fairly rigorous PRNG "fitness" tests that some more complex algorithms fail.

## Installation

You can use it in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your `game.project` file and in the dependencies field under project add:

https://github.com/indiesoftby/defold-splitmix64/archive/main.zip

Or point to the ZIP file of a [specific release](https://github.com/indiesoftby/defold-splitmix64/releases).

## Usage

### splitmix64.randomseed(x)
Sets `x` as the "seed" for the pseudo-random generator: equal seeds produce equal sequences of numbers.

_PARAMETERS_
* __x__ <kbd>number</kbd> or <kbd>string</kbd> — Seed value. Use a string (e.g. `"18446744073709551615"`) for full 64-bit precision; Lua `number` cannot represent all uint64 values.

### splitmix64.state()
Returns the current internal state as a string. Use to save/restore state or pass to `randomseed` for full 64-bit precision.

_RETURNS_
* __state__ <kbd>string</kbd> — Current state as decimal string.

### splitmix64.random([m],[n])
When called without arguments, returns a uniform pseudo-random real number in the range `[0,1)`. When called with an integer number `m`, `splitmix64.random` returns a uniform pseudo-random integer in the range `[1, m]`. When called with two integer numbers `m` and `n`, `splitmix64.random` returns a uniform pseudo-random integer in the range `[m, n]`.

_PARAMETERS_
* __m__ <kbd>number</kbd>.
* __n__ <kbd>number</kbd>.

_RETURNS_
* __value__ <kbd>number</kbd> - A pseudo-random number.

### splitmix64.randomchoice(t)
Returns a random value from array `t`. If the array is empty an error is raised.

_PARAMETERS_
* __t__ <kbd>table</kbd> - Array of values to choose from.

_RETURNS_
* __value__ - Random element from the array.

### splitmix64.weightedchoice(t)
Takes table `t` where keys are choices and values are weights. Returns a random key. Weights must be ≥ 0. If all weights are 0 or a weight is negative, an error is raised.

_PARAMETERS_
* __t__ <kbd>table</kbd> - Table of key-weight pairs.

_RETURNS_
* __key__ - Random key chosen by weight.

### splitmix64.toss()
Toss a coin. Returns 0 or 1.

_RETURNS_
* __value__ <kbd>number</kbd> - 0 or 1.

### splitmix64.dice(roll, type)
Roll one or more dice of the given type. Returns a table of individual rolls and the total sum.

_PARAMETERS_
* __roll__ <kbd>number</kbd> - Number of dice to roll (must be > 0).
* __type__ <kbd>number</kbd> - Dice type: `splitmix64.D4`, `splitmix64.D6`, `splitmix64.D8`, `splitmix64.D10`, `splitmix64.D12`, `splitmix64.D20`, or `splitmix64.D100`.

_RETURNS_
* __rolls__ <kbd>table</kbd> - Table of individual roll results.
* __total__ <kbd>number</kbd> - Sum of all rolls.

### Dice type constants
* `splitmix64.D4`, `splitmix64.D6`, `splitmix64.D8`, `splitmix64.D10`, `splitmix64.D12`, `splitmix64.D20`, `splitmix64.D100`

### Example

```lua
splitmix64.randomseed(1)

print(splitmix64.random())
print(splitmix64.random(1, 10000))

-- Random choice from array
print(splitmix64.randomchoice({true, false}))

-- Weighted choice (cat twice as likely as dog)
print(splitmix64.weightedchoice({ ["cat"] = 10, ["dog"] = 5, ["frog"] = 0 }))

-- Coin toss
print(splitmix64.toss())

-- Roll 2d6
local rolls, total = splitmix64.dice(2, splitmix64.D6)
print(total, rolls[1], rolls[2])
```

### Save/Restore state

```lua
local state = splitmix64.state()
-- ... use random ...
splitmix64.randomseed(state)  -- restore exact state
```

### Advanced Usage

You can also globally substitute the built-in `math.random` with `splitmix64`:

```
math.randomseed = splitmix64.randomseed
math.random = splitmix64.random
```
