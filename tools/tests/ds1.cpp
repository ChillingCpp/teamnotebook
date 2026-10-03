//@ ids: segtree, lazysegtree, sparse, prefix2d, compress, sqrtdecomp, dsu, dsurollback
// Test: so khớp từng phép với brute force (mảng phẳng / DSU có nhãn).
int main() {
    mt19937_64 rng(20261002);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };

    // ---------- segtree ----------
    rep(tt, 0, 50) {
        int n = rnd(1, 50);
        vector<ll> a(n, 0);
        SegTree st(n);
        rep(op, 0, 300) {
            if (rnd(0, 1) == 0) {
                int p = rnd(0, n - 1);
                ll v = rnd(-1000, 1000);
                a[p] = v;
                st.update(p, v);
            } else {
                int l = rnd(0, n - 1), r = rnd(l + 1, n);
                ll s = 0;
                rep(i, l, r) s += a[i];
                check(st.query(l, r) == s, "segtree.query");
            }
        }
    }

    // ---------- lazysegtree (max, set/add) ----------
    rep(tt, 0, 40) {
        int n = rnd(1, 60);
        vector<ll> a(n), br(n);
        rep(i, 0, n) br[i] = a[i] = rnd(-50, 50);
        LazySeg tr(a);
        rep(op, 0, 300) {
            int l = rnd(0, n - 1), r = rnd(l + 1, n);
            int typ = rnd(0, 2);
            if (typ == 0) {
                ll x = rnd(-50, 50);
                rep(i, l, r) br[i] += x;
                tr.add(l, r, x);
            } else if (typ == 1) {
                ll x = rnd(-50, 50);
                rep(i, l, r) br[i] = x;
                tr.set(l, r, x);
            } else {
                ll mx = LLONG_MIN;
                rep(i, l, r) mx = max(mx, br[i]);
                check(tr.query(l, r) == mx, "lazysegtree.query");
            }
        }
    }

    // ---------- sparse table ----------
    rep(tt, 0, 30) {
        int n = rnd(1, 50);
        vi a(n);
        rep(i, 0, n) a[i] = rnd(-1000, 1000);
        RMQ<int> rmq(a);
        rep(q, 0, 200) {
            int x = rnd(0, n - 1), y = rnd(x + 1, n);
            int mn = INT_MAX;
            rep(i, x, y) mn = min(mn, a[i]);
            check(rmq.query(x, y) == mn, "sparse.query");
        }
    }

    // ---------- prefix 2d ----------
    rep(tt, 0, 30) {
        int R = rnd(1, 25), C = rnd(1, 25);
        vector<vector<ll>> v(R, vector<ll>(C));
        rep(i, 0, R) rep(j, 0, C) v[i][j] = rnd(-100, 100);
        SubMatrix sm(v);
        rep(q, 0, 150) {
            int u = rnd(0, R - 1), d = rnd(u + 1, R);
            int l = rnd(0, C - 1), r = rnd(l + 1, C);
            ll s = 0;
            rep(i, u, d) rep(j, l, r) s += v[i][j];
            check(sm.sum(u, l, d, r) == s, "prefix2d.sum");
        }
    }

    // ---------- compress ----------
    rep(tt, 0, 50) {
        int n = rnd(1, 50);
        vector<ll> a(n);
        rep(i, 0, n) a[i] = rnd(-100, 100);
        Compress cm(a);
        vector<ll> u = a;
        sort(all(u));
        u.erase(unique(all(u)), u.end());
        check(cm.v == u && cm.size() == sz(u), "compress.build");
        for (ll x : a) check(0 <= cm.get(x) && cm.get(x) < cm.size() && cm.v[cm.get(x)] == x,
                             "compress.get-existing");
        rep(q, 0, 50) {
            ll x = rnd(-110, 110);
            int i = cm.get(x);
            check(0 <= i && i <= cm.size(), "compress.get-range");
            if (i < cm.size())
                check(cm.v[i] >= x && (i == 0 || cm.v[i - 1] < x), "compress.get-order");
            else
                check(x > u.back(), "compress.get-beyond");
        }
    }

    // ---------- sqrtdecomp ----------
    rep(tt, 0, 30) {
        int n = rnd(1, 60);
        vector<ll> a(n);
        vector<ll> br(n);
        rep(i, 0, n) br[i] = a[i] = rnd(-100, 100);
        SqrtDecomp sd(a);
        rep(op, 0, 400) {
            int l = rnd(0, n - 1), r = rnd(l + 1, n);
            if (rnd(0, 1) == 0) {
                ll x = rnd(-50, 50);
                rep(i, l, r) br[i] += x;
                sd.add(l, r, x);
            } else {
                ll s = 0;
                rep(i, l, r) s += br[i];
                check(sd.query(l, r) == s, "sqrtdecomp");
            }
        }
    }

    // ---------- dsu ----------
    rep(tt, 0, 30) {
        int n = rnd(1, 40);
        vi lab(n);
        rep(i, 0, n) lab[i] = i;
        DSU d(n);
        auto labFind = [&](int x) { while (lab[x] != x) x = lab[x]; return x; };
        rep(op, 0, 300) {
            int x = rnd(0, n - 1), y = rnd(0, n - 1);
            int typ = rnd(0, 2);
            if (typ == 0) {
                bool jd = d.join(x, y);
                int lx = labFind(x), ly = labFind(y);
                bool jb = lx != ly;
                if (jb) rep(i, 0, n) if (labFind(i) == lx) lab[i] = ly;
                check(jd == jb, "dsu.join");
            } else if (typ == 1) {
                check(d.same(x, y) == (labFind(x) == labFind(y)), "dsu.same");
            } else {
                int cnt = 0;
                int rx = labFind(x);
                rep(i, 0, n) if (labFind(i) == rx) ++cnt;
                check(d.size(x) == cnt, "dsu.size");
            }
        }
    }

    // ---------- dsu rollback ----------
    rep(tt, 0, 30) {
        int n = rnd(1, 30);
        RollbackUF rb(n);
        vi lab(n);
        rep(i, 0, n) lab[i] = i;
        vector<vi> snaps;
        vector<int> times;
        auto labFind = [&](int x) { while (lab[x] != x) x = lab[x]; return x; };
        rep(op, 0, 400) {
            int typ = rnd(0, 3);
            if (typ == 0) {
                int x = rnd(0, n - 1), y = rnd(0, n - 1);
                bool jd = rb.join(x, y);
                int lx = labFind(x), ly = labFind(y);
                bool jb = lx != ly;
                if (jb) rep(i, 0, n) if (labFind(i) == lx) lab[i] = ly;
                check(jd == jb, "dsurollback.join");
            } else if (typ == 1) {
                times.push_back(rb.time());
                snaps.push_back(lab);
            } else if (typ == 2 && !times.empty()) {
                int k = rnd(0, sz(times) - 1);
                rb.rollback(times[k]);
                check(rb.time() == times[k], "dsurollback.time");
                lab = snaps[k];
                times.resize(k + 1);
                snaps.resize(k + 1);
            } else {
                int x = rnd(0, n - 1), y = rnd(0, n - 1);
                check((rb.find(x) == rb.find(y)) == (labFind(x) == labFind(y)), "dsurollback.same");
                int cnt = 0;
                int rx = labFind(x);
                rep(i, 0, n) if (labFind(i) == rx) ++cnt;
                check(rb.size(x) == cnt, "dsurollback.size");
            }
        }
    }

    if (fails) { printf("ds1: %d loi\n", fails); return 1; }
    printf("ds1: OK\n");
    return 0;
}
