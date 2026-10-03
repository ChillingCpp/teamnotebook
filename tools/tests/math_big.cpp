//@ ids: modmul, millerrabin, factor
// Test: modmul vs __int128; isPrime vs sàng + chiếu thử; factor vs tích & thừa số nguyên tố.
// Theo doc millerrabin/modmul: giới hạn số < 7.2e18.
int main() {
    mt19937_64 rng(20261004);
    auto rnd = [&](ll l, ll r) { return (ll)(rng() % (ull)(r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };

    auto refPow = [](ull b, ull e, ull m) -> ull {
        __int128 r = 1, x = b % m;
        for (; e; e >>= 1) { if (e & 1) r = r * x % m; x = x * x % m; }
        return (ull)(r % m);
    };
    // Miller–Rabin độc lập: __int128 + 12 base nguyên tố đầu (đủ cho mọi n < 2^64)
    const ull REF_A[12] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    auto refPrime = [&](ull n) {
        if (n < 2) return false;
        for (ull p : REF_A) if (n % p == 0) return n == p;
        ull d = n - 1;
        int s = 0;
        while (d % 2 == 0) d /= 2, ++s;
        for (ull a : REF_A) {
            ull x = refPow(a, d, n);
            if (x == 1 || x == n - 1) continue;
            bool hit = false;
            rep(r, 1, s) { x = (ull)((__int128)x * x % n); if (x == n - 1) { hit = true; break; } }
            if (!hit) return false;
        }
        return true;
    };

    // sàng 0..1e6 (tham chiếu độc lập)
    const int SW = 1000000;
    vector<char> np(SW + 1, 0);
    for (int i = 2; (ll)i * i <= SW; ++i)
        if (!np[i])
            for (int j = i * i; j <= SW; j += i) np[j] = 1;
    vi spr;
    rep(i, 2, SW + 1) if (!np[i]) spr.push_back(i);
    auto trialP = [&](ull x) {  // chắc chắn đúng khi x <= 1e12
        if (x < 2) return false;
        for (int p : spr) { if ((ull)p * p > x) break; if (x % p == 0) return false; }
        return true;
    };
    auto factorPrime = [&](ull x) {
        if (x <= 1000000000000ULL) return trialP(x);
        if (!trialP(x)) return false;
        return refPrime(x);
    };

    // ---------- modmul ----------
    rep(tt, 0, 300000) {
        ull M = (ull)rnd(1, 7200000000000000000LL);
        ull a = (ull)rng() % M, b = (ull)rng() % M;
        check(modmul(a, b, M) == (ull)(((__int128)a * b) % M), "modmul.random");
    }
    {
        ull Ms[] = {1ULL, 2ULL, 3ULL, 4ULL, (1ULL << 62), (1ULL << 62) - 1, 72057594037927935ULL,
                    1000000000000000000ULL, 1000000007ULL, 4611686018427387903ULL,
                    9223372036854775807ULL, 7200000000000000000ULL};
        for (ull M : Ms) {
            ull cand[] = {0ULL, 1ULL, M - 1, M / 2, M / 2 + 1, (ull)rng() % M, (ull)rng() % M,
                          M > 2 ? M - 2 : 0ULL};
            for (ull a : cand) {
                if (a >= M) continue;
                for (ull b : cand) {
                    if (b >= M) continue;
                    check(modmul(a, b, M) == (ull)(((__int128)a * b) % M), "modmul.edge");
                }
            }
        }
    }
    rep(tt, 0, 3000) {  // modpow bản 2^64
        ull M = (ull)rnd(2, 7200000000000000000LL);
        ull b = (ull)rng() % M, e = (ull)rng() % 1000000000000ULL;
        check(modpow(b, e, M) == refPow(b, e, M), "modmul.modpow");
    }

    // ---------- millerrabin ----------
    int bad = 0;
    rep(n, 0, SW + 1)
        if (isPrime((ull)n) != (n >= 2 && !np[n])) { ++bad; if (bad < 5) printf("millerrabin n=%d\n", (int)n); }
    check(bad == 0, "millerrabin.sweep-1e6");
    for (ull p : {1000000007ULL, 1000000009ULL, 1000000033ULL, 998244353ULL, 2147483647ULL,
                  2305843009213693951ULL})
        check(isPrime(p), "millerrabin.known-prime");
    for (ull c : {561ULL, 1105ULL, 1729ULL, 2465ULL, 2821ULL, 6601ULL, 8911ULL, 41041ULL, 62745ULL})
        check(!isPrime(c), "millerrabin.carmichael");
    check(!isPrime(1000000000000000000ULL), "millerrabin.1e18-composite");
    check(!isPrime(1000000007ULL * 1000000009ULL), "millerrabin.semiprime-1e18");
    check(!isPrime(3ULL * 2305843009213693951ULL), "millerrabin.3xmersenne");
    rep(tt, 0, 300) {  // chiếu thử cho n <= 1e12
        ull n = (ull)rnd(0, 1000000000000LL);
        check(isPrime(n) == trialP(n), "millerrabin.trial-1e12");
    }
    rep(tt, 0, 400) {  // n lớn trong khoảng theo doc
        ull n = (ull)rnd(1, 7200000000000000000LL);
        check(isPrime(n) == refPrime(n), "millerrabin.vs-refPrime");
    }

    // ---------- factor ----------
    auto checkFactors = [&](ull n, const char* msg) {
        vector<ull> fs = factor(n);
        __int128 prod = 1;
        bool ok = true;
        for (ull f : fs) { prod *= f; if (!factorPrime(f)) ok = false; }
        check(ok, msg);
        check(prod == (__int128)n, msg);
    };
    rep(tt, 0, 150) checkFactors((ull)rnd(1, 1000000000000LL), "factor.product-1e12");
    rep(tt, 0, 40) checkFactors((ull)rnd(1, 7200000000000000000LL), "factor.product-big");
    for (ull p : {2ULL, 3ULL, 1000000007ULL, 999999937ULL, 2305843009213693951ULL}) {
        vector<ull> fs = factor(p);
        check(sz(fs) == 1 && fs[0] == p, "factor.prime-n-gives-itself");
    }
    check(factor(1).empty(), "factor.one-empty");
    for (ull n : {1000000007ULL * 1000000009ULL, 18446744073709551615ULL,
                  999999937ULL * 999999937ULL, 1000000007ULL * 1000000033ULL,
                  6917529027641081853ULL})
        checkFactors(n, "factor.known-big");

    if (fails) { printf("math_big: %d loi\n", fails); return 1; }
    printf("math_big: OK\n");
    return 0;
}
