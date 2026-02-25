#include "splitmix64.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond, msg)                                                 \
    do {                                                                  \
        if (!(cond)) {                                                    \
            fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg); \
            exit(1);                                                      \
        }                                                                 \
    } while (0)

static void test_deterministic(void) {
    splitmix64_state_t a, b;
    splitmix64_seed(&a, 42);
    splitmix64_seed(&b, 42);
    for (int i = 0; i < 1000; i++) {
        ASSERT(splitmix64_next(&a) == splitmix64_next(&b),
               "same seed must produce identical sequence");
    }
    printf("  deterministic ........... OK\n");
}

static void test_different_seeds(void) {
    splitmix64_state_t a, b;
    splitmix64_seed(&a, 1);
    splitmix64_seed(&b, 2);
    int differ = 0;
    for (int i = 0; i < 100; i++) {
        if (splitmix64_next(&a) != splitmix64_next(&b)) differ++;
    }
    ASSERT(differ > 90, "different seeds should produce different values");
    printf("  different seeds ......... OK\n");
}

static void test_next_double(void) {
    splitmix64_state_t s;
    splitmix64_seed(&s, 123);
    for (int i = 0; i < 10000; i++) {
        double d = splitmix64_next_double(&s);
        ASSERT(d >= 0.0 && d < 1.0, "next_double must be in [0, 1)");
    }
    printf("  next_double range ....... OK\n");
}

static void test_random_range(void) {
    splitmix64_state_t s;
    splitmix64_seed(&s, 77);
    for (int i = 0; i < 10000; i++) {
        uint64_t r = splitmix64_random_range(&s, 10);
        ASSERT(r < 10, "random_range(10) must be in [0, 10)");
    }
    /* range of 1 must always return 0 */
    for (int i = 0; i < 100; i++) {
        ASSERT(splitmix64_random_range(&s, 1) == 0, "random_range(1) must be 0");
    }
    printf("  random_range ............ OK\n");
}

static void test_random_int(void) {
    splitmix64_state_t s;
    splitmix64_seed(&s, 999);
    for (int i = 0; i < 10000; i++) {
        int64_t v = splitmix64_random_int(&s, -5, 5);
        ASSERT(v >= -5 && v <= 5, "random_int(-5,5) out of range");
    }
    /* single-value range */
    for (int i = 0; i < 100; i++) {
        ASSERT(splitmix64_random_int(&s, 7, 7) == 7, "random_int(7,7) must be 7");
    }
    printf("  random_int .............. OK\n");
}

static void test_toss(void) {
    splitmix64_state_t s;
    splitmix64_seed(&s, 0);
    int zeros = 0, ones = 0;
    for (int i = 0; i < 10000; i++) {
        int t = splitmix64_toss(&s);
        ASSERT(t == 0 || t == 1, "toss must be 0 or 1");
        if (t == 0) zeros++;
        else ones++;
    }
    ASSERT(zeros > 4000 && ones > 4000, "toss should be roughly balanced");
    printf("  toss .................... OK\n");
}

static void test_get_state(void) {
    splitmix64_state_t s;
    splitmix64_seed(&s, 12345);
    ASSERT(splitmix64_get_state(&s) == 12345, "get_state right after seed");
    splitmix64_next(&s);
    ASSERT(splitmix64_get_state(&s) != 12345, "state changes after next()");
    printf("  get_state ............... OK\n");
}

static void test_known_doubles(void) {
    /* Reference double outputs with seed=0 for cross-platform verification. */
    splitmix64_state_t s;
    splitmix64_seed(&s, 0);
    const double expected[] = {
        0.88331080821364260647,
        0.43152799704850997031,
        0.02643377159259774345,
        0.97088197815382848432,
        0.10634669156721243688,
    };
    const double EPSILON = 1e-15;
    for (int i = 0; i < 5; i++) {
        double v = splitmix64_next_double(&s);
        if (fabs(v - expected[i]) > EPSILON) {
            fprintf(stderr, "FAIL: next_double()[%d] = %.20f, expected %.20f\n",
                    i, v, expected[i]);
            exit(1);
        }
    }
    printf("  known doubles ........... OK\n");
}

static void test_known_values(void) {
    /* Verify first 4 outputs with seed=0 against reference implementation. */
    splitmix64_state_t s;
    splitmix64_seed(&s, 0);
    uint64_t expected[] = {
        UINT64_C(16294208416658607535),
        UINT64_C(7960286522194355700),
        UINT64_C(487617019471545679),
        UINT64_C(17909611376780542444),
    };
    for (int i = 0; i < 4; i++) {
        uint64_t v = splitmix64_next(&s);
        if (v != expected[i]) {
            fprintf(stderr, "FAIL: next()[%d] = %" PRIu64 ", expected %" PRIu64 "\n",
                    i, v, expected[i]);
            exit(1);
        }
    }
    printf("  known values ............ OK\n");
}

int main(void) {
    printf("splitmix64 C99 tests:\n");
    test_deterministic();
    test_different_seeds();
    test_next_double();
    test_random_range();
    test_random_int();
    test_toss();
    test_get_state();
    test_known_doubles();
    test_known_values();
    printf("All tests passed.\n");
    return 0;
}
