/**
 * SplitMix64 PRNG — pure TypeScript implementation.
 * Requires ES2020+ (uses BigInt for exact 64-bit arithmetic).
 *
 * Original algorithm by Sebastiano Vigna (2015), public domain.
 * See http://creativecommons.org/publicdomain/zero/1.0/
 *
 * This is a fixed-increment version of Java 8's SplittableRandom generator.
 * See http://dx.doi.org/10.1145/2714064.2660195
 */

const MASK64 = 0xFFFFFFFFFFFFFFFFn;
const MT64_MUL = 1.0 / 9007199254740992.0; // 1 / 2^53

export class SplitMix64 {
    private x: bigint;

    constructor(seed: bigint = 0n) {
        this.x = seed & MASK64;
    }

    /** Seed the generator. */
    seed(seed: bigint): void {
        this.x = seed & MASK64;
    }

    /** Return current internal state (useful for save / restore). */
    getState(): bigint {
        return this.x;
    }

    /** Generate the next raw 64-bit value. */
    next(): bigint {
        this.x = (this.x + 0x9E3779B97F4A7C15n) & MASK64;
        let z = this.x;
        z = ((z ^ (z >> 30n)) * 0xBF58476D1CE4E5B9n) & MASK64;
        z = ((z ^ (z >> 27n)) * 0x94D049BB133111EBn) & MASK64;
        return z ^ (z >> 31n);
    }

    /** Random double in [0, 1) with 53 bits of mantissa precision. */
    nextDouble(): number {
        return Number(this.next() >> 11n) * MT64_MUL;
    }

    /** Random bigint in [0, n).  n must be > 0. */
    randomRange(n: bigint): bigint {
        return this.next() % n;
    }

    /** Random bigint in [low, high] (inclusive).  low <= high required. */
    randomInt(low: bigint, high: bigint): bigint {
        const n = high - low + 1n;
        return this.randomRange(n) + low;
    }

    /** Coin-toss: returns 0n or 1n. */
    toss(): bigint {
        return this.randomRange(2n);
    }
}
