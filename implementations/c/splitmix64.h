/*  SplitMix64 PRNG — portable C99 implementation.
 *
 *  Original algorithm by Sebastiano Vigna (2015), public domain.
 *  See http://creativecommons.org/publicdomain/zero/1.0/
 *
 *  This is a fixed-increment version of Java 8's SplittableRandom generator.
 *  See http://dx.doi.org/10.1145/2714064.2660195
 */

#ifndef SPLITMIX64_H
#define SPLITMIX64_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t x;
} splitmix64_state_t;

/* Seed the generator. */
void splitmix64_seed(splitmix64_state_t *s, uint64_t seed);

/* Return current internal state (useful for save / restore). */
uint64_t splitmix64_get_state(const splitmix64_state_t *s);

/* Generate the next raw 64-bit value. */
uint64_t splitmix64_next(splitmix64_state_t *s);

/* Random double in [0, 1) with 53 bits of mantissa precision. */
double splitmix64_next_double(splitmix64_state_t *s);

/* Random uint64 in [0, n).  n must be > 0. */
uint64_t splitmix64_random_range(splitmix64_state_t *s, uint64_t n);

/* Random int64 in [low, high] (inclusive).  low <= high required. */
int64_t splitmix64_random_int(splitmix64_state_t *s, int64_t low, int64_t high);

/* Coin-toss: returns 0 or 1. */
int splitmix64_toss(splitmix64_state_t *s);

#ifdef __cplusplus
}
#endif

#endif /* SPLITMIX64_H */
