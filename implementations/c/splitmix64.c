#include "splitmix64.h"

#define MT64_MUL (1.0 / 9007199254740992.0) /* 1 / 2^53 */

void splitmix64_seed(splitmix64_state_t *s, uint64_t seed) {
    s->x = seed;
}

uint64_t splitmix64_get_state(const splitmix64_state_t *s) {
    return s->x;
}

uint64_t splitmix64_next(splitmix64_state_t *s) {
    uint64_t z = (s->x += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

double splitmix64_next_double(splitmix64_state_t *s) {
    return (double)(splitmix64_next(s) >> 11) * MT64_MUL;
}

uint64_t splitmix64_random_range(splitmix64_state_t *s, uint64_t n) {
    return splitmix64_next(s) % n;
}

int64_t splitmix64_random_int(splitmix64_state_t *s, int64_t low, int64_t high) {
    uint64_t n = (uint64_t)(high - low) + 1;
    return (int64_t)splitmix64_random_range(s, n) + low;
}

int splitmix64_toss(splitmix64_state_t *s) {
    return (int)splitmix64_random_range(s, 2);
}
