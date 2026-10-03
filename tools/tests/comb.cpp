//@ ids: ncr, lucas, multinomial, catalan, stirling, burnside, inclusion
// Test: so khớp với Pascal / brute / công thức đóng (RNG có seed).
int main() {
    mt19937_64 rng(20261005);
    auto rnd = [&](ll l, ll r) { return (ll)(rng() % (uint64_t)(r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };
    auto bin128 = [](ll n, ll k) -> __int128 {  // tổ hợp nhị phân chính xác
        if (k < 0 || k > n) return 0;
        k = min(k, n - k);
        __int128 r = 1;
        for (ll i = 1; i <= k; ++i) r = r * (n - k + i) / i;
        return r;
    };

    // ---------- ncr ----------
    buildNcr();
    vector<vector<ll>> pas(301, vector<ll>(301, 0));
    rep(i, 0, 301) {
        pas[i][0] = 1;
        rep(j, 1, i + 1) pas[i][j] = (pas[i - 1][j - 1] + pas[i - 1][j]) % MOD;
    }
    rep(n, 0, 301) rep(k, 0, 301)
        check(C(n, k) == (k > n ? 0 : pas[n][k]), "ncr.pascal");
    check(C(5, -1) == 0, "ncr.negative-k");
    check(C(0, 0) == 1, "ncr.0-0");
    rep(tt, 0, 300) {  // n tới 1e6: công thức tích
        ll n = rnd(0, NCR_MAX), k = rnd(0, 200);
        if (k > n) k = n;
        __int128 r = 1;
        rep(i, 1, k + 1) r = r * (n - k + i) % MOD * modinv((ll)i, MOD) % MOD;
        check((ll)r == C(n, k), "ncr.product-formula");
    }

    // ---------- lucas ----------
    auto buildLF = [](ll p) {
        int P = (int)p;
        vi f(P), g(P);
        f[0] = 1;
        rep(i, 1, P) f[i] = (ll)f[i - 1] * i % p;
        g[P - 1] = (int)modpow((ll)f[P - 1], p - 2, p);
        for (int i = P - 1; i > 0; --i) g[i - 1] = (ll)g[i] * i % p;
        return make_pair(f, g);
    };
    for (ll p : {2, 3, 5, 7, 11, 13, 17, 19}) {
        auto tb = buildLF(p);
        const vi &f = tb.first, &g = tb.second;
        vector<vector<ll>> pp(201, vector<ll>(201, 0));
        rep(i, 0, 201) {
            pp[i][0] = 1;
            rep(j, 1, i + 1) pp[i][j] = (pp[i - 1][j - 1] + pp[i - 1][j]) % p;
        }
        rep(n, 0, 201) rep(k, 0, 201)  // n >= p (chữ số base-p) đều được phủ
            check(lucas(n, k, p, f, g) == (k > n ? 0 : pp[n][k]), "lucas.vs-pascal");
        if (p == 7) {
            check(lucas(10, -1, p, f, g) == 0, "lucas.negative-k");
            check(lucas(10, 11, p, f, g) == 0, "lucas.k-greater-n");
            check(lucas(9, 0, p, f, g) == 1, "lucas.k-zero");
            check(lucas(9, 9, p, f, g) == 1, "lucas.n-eq-k");
        }
    }
    {  // p lớn hơn, n vượt p
        ll p = 10007;
        auto tb = buildLF(p);
        const vi &f = tb.first, &g = tb.second;
        vector<vector<ll>> pp(201, vector<ll>(201, 0));
        rep(i, 0, 201) {
            pp[i][0] = 1;
            rep(j, 1, i + 1) pp[i][j] = (pp[i - 1][j - 1] + pp[i - 1][j]) % p;
        }
        rep(n, 0, 201) rep(k, 0, 201)
            check(lucas(n, k, p, f, g) == (k > n ? 0 : pp[n][k]), "lucas.p10007-vs-pascal");
        rep(tt, 0, 300) {
            ll n = rnd(0, 1000000000), k = rnd(0, 8);
            if (k > n) { check(lucas(n, k, p, f, g) == 0, "lucas.big-k-greater-n"); continue; }
            __int128 r = 1;
            rep(i, 1, k + 1) r = r * (n - k + i) % p * modinv((ll)i, p) % p;
            check((ll)r == lucas(n, k, p, f, g), "lucas.big-product");
        }
    }

    // ---------- multinomial ----------
    auto bruteMulti = [](vi v) {  // đếm hoán vị khác nhau của dãy có lặp
        vi w;
        rep(i, 0, sz(v)) rep(j, 0, v[i]) w.push_back(i);
        sort(all(w));
        ll cnt = 0;
        while (true) { ++cnt; if (!next_permutation(all(w))) break; }
        return cnt;
    };
    rep(tt, 0, 15) {
        int g = rnd(2, 4), total = 0;
        vi v;
        rep(i, 0, g) {
            int x = rnd(0, 6);
            if (total + x > 9) x = 0;
            total += x;
            v.push_back(x);
        }
        check(multinomial(v) == bruteMulti(v), "multinomial.brute");
    }
    vi e0;
    check(multinomial(e0) == 1, "multinomial.empty");
    vi e1 = {7};
    check(multinomial(e1) == 1, "multinomial.single");
    vi e2 = {0, 5};
    check(multinomial(e2) == 1, "multinomial.zero-part");
    rep(tt, 0, 40) {  // độc lập: tích tổ hợp nhị phân (tổng <= 16 để không tràn ll)
        int g = rnd(2, 5), total = 0;
        vi v;
        rep(i, 0, g) {
            int x = rnd(0, 6);
            if (total + x > 16) x = 0;
            total += x;
            v.push_back(x);
        }
        __int128 r = 1;
        int rem = total;
        for (int x : v) { r *= bin128(rem, x); rem -= x; }
        check((__int128)multinomial(v) == r, "multinomial.binom-product");
    }

    // ---------- catalan ----------
    auto bruteCat = [](int n) -> ll {  // duyệt mọi đường ±1 dài 2n không vượt 0
        int len = 2 * n;
        ll cnt = 0;
        for (int mask = 0; mask < (1 << len); ++mask) {
            int bal = 0;
            bool ok = true;
            rep(i, 0, len) {
                bal += (mask >> i) & 1 ? 1 : -1;
                if (bal < 0) { ok = false; break; }
            }
            if (ok && bal == 0) ++cnt;
        }
        return cnt;
    };
    rep(n, 0, 11) check(catalan(n) == bruteCat(n) % MOD, "catalan.brute");
    {
        const int N = 100000;
        vector<ll> dp(N + 1, 0);
        dp[0] = 1;
        rep(i, 1, N + 1) dp[i] = dp[i - 1] * (4 * (ll)i - 2) % MOD * modinv((ll)i + 1, MOD) % MOD;
        rep(n, 0, 1001) check(catalan(n) == dp[n], "catalan.dp");
        rep(tt, 0, 200) {
            ll n = rnd(0, N);
            check(catalan(n) == dp[n], "catalan.dp-random");
        }
    }

    // ---------- stirling (mod MOD: stir1, stir2, bell, euler) ----------
    buildStirling(30);
    {
        const int N = 25;
        vector<vector<__int128>> s1(N + 1, vector<__int128>(N + 1, 0));
        vector<vector<__int128>> s2(N + 1, vector<__int128>(N + 1, 0));
        s1[0][0] = s2[0][0] = 1;
        rep(n, 1, N + 1) rep(k, 1, n + 1) {  // cùng công thức nhưng số học chính xác
            s1[n][k] = s1[n - 1][k - 1] + (n - 1) * s1[n - 1][k];
            s2[n][k] = s2[n - 1][k - 1] + k * s2[n - 1][k];
        }
        rep(n, 0, N + 1) rep(k, 0, N + 1) {
            check(stir1[n][k] == (ll)(s1[n][k] % MOD), "stirling1.exact");
            check(stir2[n][k] == (ll)(s2[n][k] % MOD), "stirling2.exact");
        }
        rep(n, 0, N + 1) {  // Bell = Σ_k S(n,k)
            __int128 s = 0;
            rep(k, 0, N + 1) s += s2[n][k];
            check(bell[n] == (ll)(s % MOD), "stirling.bell");
        }
    }
    check(stir1[7][0] == 0 && stir2[7][0] == 0 && stir1[7][8] == 0 && stir2[5][6] == 0,
          "stirling.out-of-range-zero");
    check(euler[6][6] == 0 && euler[6][7] == 0, "stirling.euler-out-of-range-zero");
    {  // Stirling-1 (số cycle) & Eulerian (điểm tăng) by duyệt mọi hoán vị, n <= 8
        rep(n, 1, 9) {
            vi perm(n);
            rep(i, 0, n) perm[i] = i;
            vi cycCnt(n + 1, 0), ascCnt(n + 1, 0);
            while (true) {
                vi seen(n, 0);
                int c = 0;
                rep(i, 0, n) if (!seen[i]) {
                    ++c;
                    int j = i;
                    while (!seen[j]) { seen[j] = 1; j = perm[j]; }
                }
                ++cycCnt[c];
                int asc = 0;
                rep(i, 0, n - 1) if (perm[i] < perm[i + 1]) ++asc;
                ++ascCnt[asc];
                if (!next_permutation(all(perm))) break;
            }
            rep(k, 1, n + 1) check(stir1[n][k] == cycCnt[k] % MOD, "stirling1.brute-perm");
            rep(k, 0, n) check(euler[n][k] == ascCnt[k] % MOD, "stirling.euler-brute-perm");
        }
    }
    rep(tt, 0, 120) {  // Stirling-2 công thức đóng: S = 1/k! Σ (-1)^{k-j} C(k,j) j^n
        int n = rnd(1, 20), k = rnd(1, 20);
        __int128 sum = 0;
        rep(j, 0, k + 1) {
            __int128 pw = 1;
            rep(z, 0, n) pw *= j;
            __int128 t = bin128(k, j) * pw;
            sum += ((k - j) & 1) ? -t : t;
        }
        __int128 f = 1;
        rep(i, 2, k + 1) f *= i;
        check(stir2[n][k] == (ll)(sum / f % MOD), "stirling2.closed-form");
    }

    // ---------- burnside ----------
    auto bruteNecklace = [](int n, int k) -> ll {
        ll total = 1;
        rep(i, 0, n) total *= k;
        set<vector<int>> orbs;
        for (ll code = 0; code < total; ++code) {
            vi col(n);
            ll x = code;
            rep(i, 0, n) { col[i] = (int)(x % k); x /= k; }
            vi best = col;
            rep(r, 1, n) {
                vi rot(n);
                rep(i, 0, n) rot[i] = col[(i + r) % n];
                if (rot < best) best = rot;
            }
            orbs.insert(best);
        }
        return (ll)orbs.size();
    };
    rep(n, 1, 8) rep(k, 1, 5) {  // nhóm quay Z_n, màu k
        auto f = [&](int d) { ll r = 1; rep(z, 0, d) r *= k; return r; };  // f(d) = k^d
        check(rotateNecklace(n, f) == bruteNecklace(n, k), "burnside.brute");
    }

    // ---------- inclusion ----------
    // Contract: cntSubset(mask) = #x satisfying ALL groups in mask,
    // ieCount = union size = #x satisfying at least one group.
    rep(tt, 0, 300) {
        int g = rnd(1, 6), m = rnd(0, 60);
        vi mem(m);  // mem[i] = bitmask of groups element i satisfies
        rep(i, 0, m) mem[i] = rnd(0, (1 << g) - 1);
        ll expect = 0;
        rep(i, 0, m) if (mem[i] != 0) ++expect;
        ll got = ieCount(g, [&](int mask) {
            ll c = 0;
            rep(i, 0, m) if ((mem[i] & mask) == mask) ++c;
            return c;
        });
        check(got == expect, "inclusion.brute");
        // complement trick: x satisfying ALL groups = m - ieCount(violating)
        ll violAll = ieCount(g, [&](int mask) {
            ll c = 0;
            rep(i, 0, m) if ((mem[i] & mask) == 0) ++c;
            return c;
        });
        ll allGroups = 0;
        rep(i, 0, m) if (mem[i] == (1 << g) - 1) ++allGroups;
        check(m - violAll == allGroups, "inclusion.all-groups");
    }

    if (fails) { printf("comb: %d loi\n", fails); return 1; }
    printf("comb: OK\n");
    return 0;
}
