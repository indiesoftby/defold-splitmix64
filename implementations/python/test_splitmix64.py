"""Tests for splitmix64 — mirrors the C99 test suite."""

import math
import sys

from splitmix64 import SplitMix64


def assert_true(cond: bool, msg: str) -> None:
    if not cond:
        print(f"FAIL: {msg}", file=sys.stderr)
        sys.exit(1)


def test_deterministic() -> None:
    a = SplitMix64(42)
    b = SplitMix64(42)
    for _ in range(1000):
        assert_true(a.next() == b.next(), "same seed must produce identical sequence")
    print("  deterministic ........... OK")


def test_different_seeds() -> None:
    a = SplitMix64(1)
    b = SplitMix64(2)
    differ = sum(1 for _ in range(100) if a.next() != b.next())
    assert_true(differ > 90, "different seeds should produce different values")
    print("  different seeds ......... OK")


def test_next_double() -> None:
    s = SplitMix64(123)
    for _ in range(10000):
        d = s.next_double()
        assert_true(0.0 <= d < 1.0, "next_double must be in [0, 1)")
    print("  next_double range ....... OK")


def test_random_range() -> None:
    s = SplitMix64(77)
    for _ in range(10000):
        r = s.random_range(10)
        assert_true(0 <= r < 10, "random_range(10) must be in [0, 10)")
    # range of 1 must always return 0
    for _ in range(100):
        assert_true(s.random_range(1) == 0, "random_range(1) must be 0")
    print("  random_range ............ OK")


def test_random_int() -> None:
    s = SplitMix64(999)
    for _ in range(10000):
        v = s.random_int(-5, 5)
        assert_true(-5 <= v <= 5, "random_int(-5,5) out of range")
    # single-value range
    for _ in range(100):
        assert_true(s.random_int(7, 7) == 7, "random_int(7,7) must be 7")
    print("  random_int .............. OK")


def test_toss() -> None:
    s = SplitMix64(0)
    zeros = 0
    ones = 0
    for _ in range(10000):
        t = s.toss()
        assert_true(t == 0 or t == 1, "toss must be 0 or 1")
        if t == 0:
            zeros += 1
        else:
            ones += 1
    assert_true(zeros > 4000 and ones > 4000, "toss should be roughly balanced")
    print("  toss .................... OK")


def test_get_state() -> None:
    s = SplitMix64(12345)
    assert_true(s.get_state() == 12345, "get_state right after seed")
    s.next()
    assert_true(s.get_state() != 12345, "state changes after next()")
    print("  get_state ............... OK")


def test_known_doubles() -> None:
    s = SplitMix64(0)
    expected = [
        0.88331080821364260647,
        0.43152799704850997031,
        0.02643377159259774345,
        0.97088197815382848432,
        0.10634669156721243688,
    ]
    epsilon = 1e-15
    for i, exp in enumerate(expected):
        v = s.next_double()
        if math.fabs(v - exp) > epsilon:
            print(f"FAIL: next_double()[{i}] = {v:.20f}, expected {exp:.20f}", file=sys.stderr)
            sys.exit(1)
    print("  known doubles ........... OK")


def test_known_values() -> None:
    s = SplitMix64(0)
    expected = [
        16294208416658607535,
        7960286522194355700,
        487617019471545679,
        17909611376780542444,
    ]
    for i, exp in enumerate(expected):
        v = s.next()
        if v != exp:
            print(f"FAIL: next()[{i}] = {v}, expected {exp}", file=sys.stderr)
            sys.exit(1)
    print("  known values ............ OK")


if __name__ == "__main__":
    print("splitmix64 Python tests:")
    test_deterministic()
    test_different_seeds()
    test_next_double()
    test_random_range()
    test_random_int()
    test_toss()
    test_get_state()
    test_known_doubles()
    test_known_values()
    print("All tests passed.")
