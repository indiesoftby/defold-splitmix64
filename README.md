# SplitMix64 RNG for Defold

This extension wraps SplitMix64 in a simple Lua API for the [Defold](https://defold.com/) game engine. The SplitMix64 algo is from [http://prng.di.unimi.it/splitmix64.c](http://prng.di.unimi.it/splitmix64.c).

## Installation

You can use it in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your `game.project` file and in the dependencies field under project add:

https://github.com/indiesoftby/defold-splitmix64/archive/main.zip

Or point to the ZIP file of a [specific release](https://github.com/indiesoftby/defold-splitmix64/releases).

## Usage

### splitmix64.randomseed(x)
Sets `x` as the "seed" for the pseudo-random generator: equal seeds produce equal sequences of numbers.

_PARAMETERS_
* __x__ <kbd>number</kbd>.

### splitmix64.random([m],[n])
When called without arguments, returns a uniform pseudo-random real number in the range `[0,1)`. When called with an integer number `m`, `splitmix64.random` returns a uniform pseudo-random integer in the range `[1, m]`. When called with two integer numbers `m` and `n`, `splitmix64.random` returns a uniform pseudo-random integer in the range `[m, n]`.

_PARAMETERS_
* __m__ <kbd>number</kbd>.
* __n__ <kbd>number</kbd>.

_RETURNS_
* __value__ <kbd>number</kbd> - A pseudo-random number.

### Example

```lua
splitmix64.randomseed(1)

print(splitmix64.random())
print(splitmix64.random(1, 10000))
```

### Advanced Usage

You can also globally substitute the built-in `math.random` with `splitmix64`:

```
math.randomseed = splitmix64.randomseed
math.random = splitmix64.random
```
