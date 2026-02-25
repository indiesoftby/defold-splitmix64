"""SplitMix64 PRNG — pure Python 3.11 implementation.

Original algorithm by Sebastiano Vigna (2015), public domain.
See http://creativecommons.org/publicdomain/zero/1.0/

This is a fixed-increment version of Java 8's SplittableRandom generator.
See http://dx.doi.org/10.1145/2714064.2660195
"""

_MASK64 = 0xFFFFFFFFFFFFFFFF
_MT64_MUL = 1.0 / 9007199254740992.0  # 1 / 2^53


class SplitMix64:
    __slots__ = ("_x",)

    def __init__(self, seed: int = 0) -> None:
        self.seed(seed)

    def seed(self, seed: int) -> None:
        """Seed the generator."""
        self._x = seed & _MASK64

    def get_state(self) -> int:
        """Return current internal state (useful for save / restore)."""
        return self._x

    def next(self) -> int:
        """Generate the next raw 64-bit value."""
        self._x = (self._x + 0x9E3779B97F4A7C15) & _MASK64
        z = self._x
        z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) & _MASK64
        z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & _MASK64
        return z ^ (z >> 31)

    def next_double(self) -> float:
        """Random double in [0, 1) with 53 bits of mantissa precision."""
        return float(self.next() >> 11) * _MT64_MUL

    def random_range(self, n: int) -> int:
        """Random int in [0, n).  n must be > 0."""
        return self.next() % n

    def random_int(self, low: int, high: int) -> int:
        """Random int in [low, high] (inclusive).  low <= high required."""
        n = high - low + 1
        return self.random_range(n) + low

    def toss(self) -> int:
        """Coin-toss: returns 0 or 1."""
        return self.random_range(2)
