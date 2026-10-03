//@ ids: binsearch, ternary, cht, dcdp, knuth, sos, lis, bitmask, pbs
// Test: brute force / đối chiếu với O(n^2)-O(n^3) reference.
int main() {
    mt19937_64 rng(20261005);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };

    // ---------- binsearch: l nhỏ nhất với check(l) == true ----------
    rep(tt, 0, 200) {
        int N = rnd(1, 60);
        int first = rnd(0, N);  // check(x) = (x >= first) trên [0, N], luôn có true
        ll got = binSearch(0, N, [&](ll x) { return x >= first; });
        check(got == first, "binsearch.first");
    }

    // ---------- ternary / golden: cực tiểu của hàm unimodal ----------
    rep(tt, 0, 30) {
        double c = rnd(0, 1000) / 10.0;  // điểm cực tiểu trong [0, 100]
        auto f = [&](double x) { return (x - c) * (x - c) + 1.0; };
        double x1 = ternarySearch(0.0, 100.0, f);
        double x2 = golden(0.0, 100.0, f);
        check(fabs(x1 - c) < 1e-3, "ternary.min");
        check(fabs(x2 - c) < 1e-3, "golden.min");
    }

    // ---------- cht: max (k*x + m) vs quét hết ----------
    rep(tt, 0, 30) {
        LineContainer lc;
        int nl = rnd(1, 30);
        vector<pair<ll, ll>> lines;
        rep(i, 0, nl) {
            ll k = rnd(-50, 50), m = rnd(-1000, 1000);
            lines.push_back({k, m});
            lc.add(k, m);
        }
        ll x = -1000;
        rep(q, 0, 50) {
            x += rnd(0, 60);  // x không giảm (theo doc)
            ll exp = LLONG_MIN;
            for (auto& e : lines) exp = max(exp, e.first * x + e.second);
            check(lc.query(x) == exp, "cht.query");
        }
    }

    // ---------- dcdp: opt đơn điệu với cost (k-i)^2 ----------
    rep(tt, 0, 15) {
        int n = rnd(2, 40);
        vector<ll> pv(n), dp(n);
        rep(i, 0, n) pv[i] = rnd(-100, 100);
        dp[0] = LLONG_MAX;  // [lo(0), hi(0)) rỗng
        rep(i, 1, n) {
            dp[i] = LLONG_MAX;
            rep(k, 0, i) dp[i] = min(dp[i], pv[k] + (ll)(k - i) * (k - i));
        }
        DCDP d(n, [](int) { return 0; }, [](int i) { return i; },
               [&](int i, int k) { return pv[k] + (ll)(k - i) * (k - i); });
        d.solve();
        rep(i, 0, n) check(d.dp[i] == dp[i], "dcdp.dp");
    }

    // ---------- knuth: w = tổng đoạn (đ thỏa quadrangle) vs O(n^3) ----------
    rep(tt, 0, 15) {
        int n = rnd(1, 30);
        vector<ll> wv(n), pref(n + 1, 0);
        rep(i, 0, n) wv[i] = rnd(1, 50), pref[i + 1] = pref[i] + wv[i];
        auto W = [&](int i, int j) { return pref[j + 1] - pref[i]; };
        vector<vector<ll>> dp(n, vector<ll>(n, 0));
        rep(len, 1, n) rep(i, 0, n - len) {
            int j = i + len;
            ll best = LLONG_MAX;
            rep(k, i, j) best = min(best, dp[i][k] + dp[k + 1][j]);
            dp[i][j] = best + W(i, j);
        }
        KnuthDP kd(n, W);
        kd.solve();
        check(kd.dp[0][n - 1] == dp[0][n - 1], "knuth.dp");
    }

    // ---------- sos: F[i] = sum_{j subset i} f[j] ----------
    rep(tt, 0, 30) {
        int k = rnd(1, 8), n = 1 << k;
        vector<ll> f(n), F;
        rep(i, 0, n) f[i] = rnd(-100, 100);
        F = f;
        sos(F);
        rep(i, 0, n) {
            ll exp = 0;
            for (int j = i;; j = (j - 1) & i) {
                exp += f[j];
                if (j == 0) break;
            }
            check(F[i] == exp, "sos.subset");
        }
    }

    // ---------- lis: chỉ số tăng & độ dài = LIS brute ----------
    rep(tt, 0, 60) {
        int n = rnd(0, 50);
        vector<ll> S(n);
        rep(i, 0, n) S[i] = rnd(0, 20);
        vi got = lis(S);
        bool ok = true;
        rep(i, 0, sz(got)) if (got[i] < 0 || got[i] >= n) ok = false;
        rep(i, 1, sz(got))
            if (!(got[i] > got[i - 1] && S[got[i]] > S[got[i - 1]])) ok = false;
        check(ok, "lis.valid");
        vi bl(n, 1);
        rep(i, 0, n) rep(j, 0, i) if (S[j] < S[i]) bl[i] = max(bl[i], bl[j] + 1);
        int exp = n ? *max_element(all(bl)) : 0;
        check(sz(got) == exp, "lis.length");
    }

    // ---------- pbs: batch one-pass, kq = (ans > mid) ----------
    rep(tt, 0, 50) {
        int Q = rnd(1, 40), hi0 = rnd(1, 60);
        vi ans(Q);
        rep(i, 0, Q) ans[i] = rnd(0, hi0 - 1);  // ans ∈ [0, hi0)
        PBS p(Q, hi0, [&](const vi& ids, const vi& mids) {
            vi ord(sz(ids));
            iota(all(ord), 0);
            sort(all(ord), [&](int x, int y) { return mids[x] < mids[y]; });
            vector<bool> kq(sz(ids));
            for (int x : ord) kq[x] = ans[ids[x]] > mids[x];  // strict (theo doc)
            return kq;
        });
        vi got = p.solve();
        rep(i, 0, Q) check(got[i] == ans[i], "pbs.ans");
    }

    printf(fails ? "dp: %d FAIL\n" : "dp: OK\n", fails);
    return fails ? 1 : 0;
}
