/**
 * Tests for splitmix64 — mirrors the C99 test suite.
 * Requires ES2020+ (BigInt).
 */

declare const process: { exit(code: number): never };

import { SplitMix64 } from "./splitmix64";

function assert(cond: boolean, msg: string): void {
    if (!cond) {
        console.error(`FAIL: ${msg}`);
        process.exit(1);
    }
}

function testDeterministic(): void {
    const a = new SplitMix64(42n);
    const b = new SplitMix64(42n);
    for (let i = 0; i < 1000; i++) {
        assert(a.next() === b.next(), "same seed must produce identical sequence");
    }
    console.log("  deterministic ........... OK");
}

function testDifferentSeeds(): void {
    const a = new SplitMix64(1n);
    const b = new SplitMix64(2n);
    let differ = 0;
    for (let i = 0; i < 100; i++) {
        if (a.next() !== b.next()) differ++;
    }
    assert(differ > 90, "different seeds should produce different values");
    console.log("  different seeds ......... OK");
}

function testNextDouble(): void {
    const s = new SplitMix64(123n);
    for (let i = 0; i < 10000; i++) {
        const d = s.nextDouble();
        assert(d >= 0.0 && d < 1.0, "nextDouble must be in [0, 1)");
    }
    console.log("  nextDouble range ........ OK");
}

function testRandomRange(): void {
    const s = new SplitMix64(77n);
    for (let i = 0; i < 10000; i++) {
        const r = s.randomRange(10n);
        assert(r >= 0n && r < 10n, "randomRange(10) must be in [0, 10)");
    }
    // range of 1 must always return 0
    for (let i = 0; i < 100; i++) {
        assert(s.randomRange(1n) === 0n, "randomRange(1) must be 0");
    }
    console.log("  randomRange ............. OK");
}

function testRandomInt(): void {
    const s = new SplitMix64(999n);
    for (let i = 0; i < 10000; i++) {
        const v = s.randomInt(-5n, 5n);
        assert(v >= -5n && v <= 5n, "randomInt(-5,5) out of range");
    }
    // single-value range
    for (let i = 0; i < 100; i++) {
        assert(s.randomInt(7n, 7n) === 7n, "randomInt(7,7) must be 7");
    }
    console.log("  randomInt ............... OK");
}

function testToss(): void {
    const s = new SplitMix64(0n);
    let zeros = 0;
    let ones = 0;
    for (let i = 0; i < 10000; i++) {
        const t = s.toss();
        assert(t === 0n || t === 1n, "toss must be 0 or 1");
        if (t === 0n) zeros++;
        else ones++;
    }
    assert(zeros > 4000 && ones > 4000, "toss should be roughly balanced");
    console.log("  toss .................... OK");
}

function testGetState(): void {
    const s = new SplitMix64(12345n);
    assert(s.getState() === 12345n, "getState right after seed");
    s.next();
    assert(s.getState() !== 12345n, "state changes after next()");
    console.log("  getState ................ OK");
}

function testKnownDoubles(): void {
    const s = new SplitMix64(0n);
    const expected = [
        0.88331080821364260647,
        0.43152799704850997031,
        0.02643377159259774345,
        0.97088197815382848432,
        0.10634669156721243688,
    ];
    const EPSILON = 1e-15;
    for (let i = 0; i < expected.length; i++) {
        const v = s.nextDouble();
        if (Math.abs(v - expected[i]) > EPSILON) {
            console.error(`FAIL: nextDouble()[${i}] = ${v}, expected ${expected[i]}`);
            process.exit(1);
        }
    }
    console.log("  known doubles ........... OK");
}

function testKnownValues(): void {
    const s = new SplitMix64(0n);
    const expected = [
        16294208416658607535n,
        7960286522194355700n,
        487617019471545679n,
        17909611376780542444n,
    ];
    for (let i = 0; i < expected.length; i++) {
        const v = s.next();
        if (v !== expected[i]) {
            console.error(`FAIL: next()[${i}] = ${v}, expected ${expected[i]}`);
            process.exit(1);
        }
    }
    console.log("  known values ............ OK");
}

console.log("splitmix64 TypeScript tests:");
testDeterministic();
testDifferentSeeds();
testNextDouble();
testRandomRange();
testRandomInt();
testToss();
testGetState();
testKnownDoubles();
testKnownValues();
console.log("All tests passed.");
