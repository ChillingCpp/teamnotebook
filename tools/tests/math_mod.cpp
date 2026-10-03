//@ ids: modpow, euclid, crt, modsqrt, modlog, phi, floorsum, modops, sieve, segsieve
// Test: so khớp từng phép với brute force (RNG có seed).
// Theo doc: crt & modsqrt ASSERT khi không có nghiệm → chỉ test nghiệm tồn tại.
int main() {
    mt19937_64 rng(20261003);
    auto rnd = [&](ll l, ll r) { return (ll)(rng() % (ull)(r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };
    auto slowpow = [&](ll b, ll e, ll m) -> ll {
        __int128 r = 1, x = ((b % m) + m) % m;
        for (; e; e >>= 1) { if (e & 1) r = r * x % m; x = x * x % m; }
        return (ll)r;
    };
    auto naivePrime = [](ll x) {
        if (x < 2) return false;
        for (ll d = 2; d * d <= x; ++d) if (x % d == 0) return false;
        return true;
    };
    auto normref = [](ll v) { ll r = v % MOD; return r < 0 ? r + MOD : r; };

    // ---------- modpow / modinv ----------
    rep(tt, 0, 400) {
        ll m = rnd(2, 1000000007), b = rnd(0, m - 1), e = rnd(0, 1000000000000000000LL);
        check(modpow(b, e, m) == slowpow(b, e, m), "modpow.random");
    }
    rep(tt, 0, 60) {
        ll m = rnd(2, 1000000007), z = 0, o = 1, b = rnd(1, m - 1);
        check(modpow(z, z, m) == 1, "modpow.0^0");
        check(modpow(z, rnd(1, 1000), m) == 0, "modpow.0^e");
        check(modpow(b, z, m) == 1, "modpow.b^0");
        check(modpow(b, o, m) == b, "modpow.b^1");
    }
    rep(tt, 0, 80) {
        ll a = rnd(1, 1000000006), c = rnd(2, 998244351);
        check((ll)((__int128)a * modinv(a, 1000000007) % 1000000007) == 1, "modinv.1e9+7");
        check((ll)((__int128)c * modinv(c, 998244353) % 998244353) == 1, "modinv.998244353");
        check((ll)((__int128)a * modinv(a) % MOD) == 1, "modinv.default-mod");
    }
    check(modinv(0, 1000000007) == 0, "modinv.no-inverse-at-0");  // 0 không có nghịch đảo

    // ---------- euclid ----------
    rep(tt, 0, 1500) {
        ll a = rnd(0, 1000000000000000000LL), b = rnd(0, 1000000000000000000LL);
        ll x, y, g = euclid(a, b, x, y);
        check(g == gcd(a, b), "euclid.gcd");
        check((__int128)a * x + (__int128)b * y == g, "euclid.identity");
    }
    rep(tt, 0, 300) {  // ax + by = c với g | c
        ll a = rnd(0, 100000000), b = rnd(0, 100000000);
        ll x, y, g = euclid(a, b, x, y);
        ll c = g * rnd(0, 1000), k = g ? c / g : 0;
        check((__int128)a * x * k + (__int128)b * y * k == c, "euclid.diophantine");
    }
    rep(tt, 0, 300) {  // nghịch đảo modulo khi gcd == 1
        ll a = rnd(1, 1000000000), m = rnd(2, 1000000000), x, y;
        if (euclid(a, m, x, y) == 1)
            check((ll)((__int128)a * ((x % m + m) % m) % m) == 1, "euclid.inverse");
    }

    // ---------- crt ----------
    rep(tt, 0, 3000) {  // x0 đặt sẵn → nghiệm duy nhất trong [0, lcm) là x0 (tối thiểu)
        ll m = rnd(1, 1500000000), n = rnd(1, 1500000000);
        ll g = gcd(m, n), lcm = m / g * n, x0 = rnd(0, lcm - 1);
        check(crt(x0 % m, m, x0 % n, n) == x0, "crt.coprime-noncoprime");
    }
    rep(tt, 0, 80) {  // gộp 3-4 đồng dư, so với brute tìm nghiệm nhỏ nhất
        vector<ll> ms, rs;
        ll L = 1;
        int cnt = rnd(2, 4);
        bool ok = true;
        rep(i, 0, cnt) {
            ll mi = rnd(2, 30), g2 = gcd(L, mi);
            if (L / g2 * mi > 1000000) { ok = false; break; }
            L = L / g2 * mi;
            ms.push_back(mi);
        }
        if (!ok || sz(ms) != cnt) continue;
        for (ll mi : ms) rs.push_back(rnd(0, mi - 1));
        ll best = -1;
        for (ll x = 0; x < L && best < 0; ++x) {
            bool good = true;
            rep(i, 0, sz(ms)) if (x % ms[i] != rs[i]) { good = false; break; }
            if (good) best = x;
        }
        if (best < 0) continue;  // hệ vô nghiệm → crt assert, bỏ qua
        ll a = rs[0], M = ms[0];
        rep(i, 1, sz(ms)) {
            ll g2 = gcd(M, ms[i]);
            check((a - rs[i]) % g2 == 0, "crt.fold-consistency");
            a = crt(a, M, rs[i], ms[i]);
            M = M / g2 * ms[i];
        }
        check(a == best, "crt.fold-minimal");
    }

    // ---------- modsqrt (chỉ nghiệm tồn tại — vô nghiệm thì assert theo doc) ----------
    int br3 = 0, br1 = 0;
    rep(tt, 0, 1500) {
        ll p = rnd(3, 999999999);
        if (p % 2 == 0) ++p;
        if (!naivePrime(p)) continue;
        if (p % 4 == 3) ++br3; else ++br1;
        ll x = rnd(0, p - 1), n2 = (ll)((__int128)x * x % p);
        ll r = modsqrt(n2, p);
        check(0 <= r && r < p && (__int128)r * r % p == n2, "modsqrt.r2-eq-n");
    }
    check(br3 > 40 && br1 > 40, "modsqrt.both-branches");  // đủ case p%4==3 và p%4==1
    check(modsqrt(0, 7) == 0, "modsqrt.zero");

    // ---------- modlog: exhaustive mod nhỏ vs brute ----------
    for (ll m = 1; m <= 60; ++m)
        for (ll a = 0; a < m; ++a)
            for (ll b = 0; b < m; ++b) {
                ll expect = -1, cur = 1;
                rep(y, 1, (int)m + 1) { cur = cur * a % m; if (cur == b % m) { expect = y; break; } }
                ll got = modLog(a, b, m);
                if (got == -1) check(expect == -1, "modlog.tiny-none");
                else check(expect == got && slowpow(a, got, m) == b % m, "modlog.tiny");
            }
    rep(tt, 0, 500) {  // m lớn: nghiệm trả về phải thật sự là nghiệm
        ll m = rnd(2, 1000000000), a = rnd(0, m - 1), b = rnd(0, m - 1);
        ll y = modLog(a, b, m);
        if (y != -1) check(y >= 1 && slowpow(a, y, m) == b % m, "modlog.random");
    }
    rep(tt, 0, 500) {  // b = a^y0 đặt sẵn → chắc chắn có nghiệm
        ll m = rnd(2, 1000000000), a = rnd(0, m - 1), y0 = rnd(1, 300);
        ll b = slowpow(a, y0, m), y = modLog(a, b, m);
        check(y != -1 && slowpow(a, y, m) == b, "modlog.constructed");
    }

    // ---------- phi & mobius (from sievePhiMu) ----------
    // (bug phi trong notebook đã fix: nhánh nguyên tố set phi[i]=i-1, nhánh p|i ×p)
    sievePhiMu();
    auto naiveMu = [](int n) {
        int res = 1;
        for (int d = 2; d * d <= n; ++d) if (n % d == 0) {
            n /= d;
            if (n % d == 0) return 0;
            res = -res;
        }
        if (n > 1) res = -res;
        return res;
    };
    auto naivePhi = [](int n) {
        int res = n, x = n;
        for (int d = 2; d * d <= x; ++d) if (x % d == 0) {
            while (x % d == 0) x /= d;
            res -= res / d;
        }
        if (x > 1) res -= res / x;
        return res;
    };
    rep(n, 1, 3001) {
        check(mu[n] == naiveMu(n), "phi.mobius");
        check(phi[n] == naivePhi(n), "phi.euler");
    }
    rep(n, 1, 501) {  // Σ_{d|n} φ(d) = n
        ll s = 0;
        for (int d = 1; d <= n; ++d) if (n % d == 0) s += phi[d];
        check(s == n, "phi.divisor-sum");
    }
    {  // reference linear sieve for mu over the whole array
        vector<int> refMu(PHI_LIM + 1, 1), refPhi(PHI_LIM + 1), refPrimes;
        vector<bool> comp(PHI_LIM + 1, false);
        refMu[1] = 1;
        rep(i, 0, PHI_LIM + 1) refPhi[i] = i;
        rep(i, 2, PHI_LIM + 1) {
            if (!comp[i]) {
                refPrimes.push_back(i);
                refMu[i] = -1;
                refPhi[i] = i - 1;
            }
            for (int p : refPrimes) {
                if (i * p > PHI_LIM) break;
                comp[i * p] = true;
                if (i % p == 0) {
                    refMu[i * p] = 0;
                    refPhi[i * p] = refPhi[i] * p;
                    break;
                }
                refMu[i * p] = -refMu[i];
                refPhi[i * p] = refPhi[i] * (p - 1);
            }
        }
        int bad = 0, badPhi = 0;
        rep(n, 1, PHI_LIM + 1) {
            if (mu[n] != refMu[n]) ++bad;
            if (phi[n] != refPhi[n]) ++badPhi;
        }
        check(bad == 0, "phi.mobius-full-array");
        check(badPhi == 0, "phi.euler-full-array");
    }

    // ---------- floorsum ----------
    rep(tt, 0, 2500) {
        ull to = (ull)rnd(0, 3000), c = (ull)rnd(0, 1000000000);
        ull k = (ull)rnd(0, 1000000000), m = (ull)rnd(1, 1000000000);
        ull ex = 0;
        for (ull i = 0; i < to; ++i) ex += (k * i + c) / m;
        check(divsum(to, c, k, m) == ex, "floorsum.divsum");
    }
    rep(tt, 0, 8) {  // to lớn, vẫn brute O(to)
        ull to = (ull)rnd(100000, 1000000), c = (ull)rnd(0, 1000);
        ull k = (ull)rnd(0, 1000), m = (ull)rnd(1, 1000);
        ull ex = 0;
        for (ull i = 0; i < to; ++i) ex += (k * i + c) / m;
        check(divsum(to, c, k, m) == ex, "floorsum.divsum-large");
    }
    rep(tt, 0, 800) {
        ull to = (ull)rnd(0, 3000), ex = 0;
        for (ull i = 0; i < to; ++i) ex += i;
        check(sumsq(to) == ex, "floorsum.sumsq");
    }
    rep(tt, 0, 50) {
        ull to = (ull)rnd(1, 1000000000000000000LL);
        check(sumsq(to) == (ull)((__int128)to * (to - 1) / 2), "floorsum.sumsq-formula");
    }
    rep(tt, 0, 1500) {  // modsum: Σ (k*i+c) % m, c/k có thể âm
        ull to = (ull)rnd(0, 3000);
        ll c = rnd(-1000000000, 1000000000), k = rnd(-1000000000, 1000000000), m = rnd(1, 1000000000);
        ll cc = ((c % m) + m) % m, kk = ((k % m) + m) % m;
        ll ex = 0;
        for (ull i = 0; i < to; ++i) ex += (kk * (ll)i + cc) % m;
        check(modsum(to, c, k, m) == ex, "floorsum.modsum");
    }

    // ---------- modops ----------
    rep(tt, 0, 1500) {
        ll x = rnd(-3000000000LL, 3000000000LL);
        check(norm(x) == normref(x), "modops.norm");
        ll a = rnd(-4000000000000000000LL, 4000000000000000000LL);
        ll b = rnd(-4000000000000000000LL, 4000000000000000000LL);
        check(addmod(a, b) == normref(a + b), "modops.addmod");
        check(submod(a, b) == normref(a - b), "modops.submod");
        ll p = rnd(-1000000000000000000LL, 1000000000000000000LL);
        ll q = rnd(-1000000000000000000LL, 1000000000000000000LL);
        check(mulmod(p, q) == normref((ll)((__int128)p * q % MOD)), "modops.mulmod");
        ll bb = rnd(1, MOD - 1);
        check(mulmod(divmod(p, bb), bb) == norm(p), "modops.divmod");
    }

    // ---------- sieve ----------
    eratosthenes();
    vector<char> np(LIM + 1, 0);
    for (int i = 2; (ll)i * i <= LIM; ++i)
        if (!np[i])
            for (int j = i * i; j <= LIM; j += i) np[j] = 1;
    vi npr;
    rep(i, 2, LIM + 1) if (!np[i]) npr.push_back(i);
    check(primes == npr, "sieve.prime-list");
    bool ipOk = true;
    rep(i, 0, LIM + 1) if ((bool)isPrime[i] != (i >= 2 && !np[i])) { ipOk = false; break; }
    check(ipOk, "sieve.isPrime");

    // ---------- segsieve ----------
    auto trialPrime = [](ll x) {
        if (x < 2) return false;
        for (ll p : primes) { if (p * p > x) break; if (x % p == 0) return false; }
        return true;
    };
    auto checkRange = [&](ll L, ll R, const char* msg) {
        vector<bool> is = segSieve(L, R);
        if (sz(is) != (int)(R - L + 1)) { check(false, msg); return; }
        rep(i, 0, sz(is)) if (is[i] != trialPrime(L + i)) { check(false, msg); return; }
    };
    checkRange(0, 0, "segsieve.[0,0]");
    checkRange(1, 1, "segsieve.[1,1]");
    checkRange(2, 2, "segsieve.[2,2]");
    checkRange(0, 60, "segsieve.[0,60]");
    checkRange(1, 60, "segsieve.[1,60]");
    rep(tt, 0, 12) {  // L nhỏ, R-L <= 5000
        ll L = rnd(0, 1000000), R = L + rnd(0, 5000);
        checkRange(L, R, "segsieve.small-L");
    }
    rep(tt, 0, 6) {  // R tới ~1e11, R-L <= 5000
        ll L = rnd(0, 100000000000LL - 5001), R = L + rnd(0, 5000);
        checkRange(L, R, "segsieve.large-R");
    }

    if (fails) { printf("math_mod: %d loi\n", fails); return 1; }
    printf("math_mod: OK\n");
    return 0;
}
