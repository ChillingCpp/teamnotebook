//@ ids: sqrtdecomp, fractional
// Test: brute force từng phép.
int main() {
    mt19937_64 rng(20261006);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };

    // ---------- sqrtdecomp: add/query [l, r) ----------
    rep(tt, 0, 40) {
        int n = rnd(1, 60);
        vector<ll> a(n), br(n);
        rep(i, 0, n) br[i] = a[i] = rnd(-50, 50);
        SqrtDecomp sd(a);
        rep(op, 0, 300) {
            int l = rnd(0, n - 1), r = rnd(l + 1, n);
            if (rnd(0, 1) == 0) {
                ll x = rnd(-30, 30);
                rep(i, l, r) br[i] += x;
                sd.add(l, r, x);
            } else {
                ll s = 0;
                rep(i, l, r) s += br[i];
                check(sd.query(l, r) == s, "sqrtdecomp.query");
            }
        }
        rep(i, 0, n) check(sd.query(i, i + 1) == br[i], "sqrtdecomp.point");
    }

    // ---------- fractional cascading: x có mặt trong MỌI danh sách ----------
    rep(tt, 0, 40) {
        int k = rnd(1, 6);  // số danh sách
        int nc = rnd(1, 8);
        vi common(nc);
        rep(i, 0, nc) common[i] = rnd(0, 1000);
        sort(all(common));
        common.erase(unique(all(common)), common.end());
        set<int> cs(all(common));
        vector<vi> L(k);
        rep(i, 0, k) {
            set<int> s = cs;  // mọi list chứa mọi giá trị chung
            int extra = rnd(0, 10);
            rep(j, 0, extra) s.insert(rnd(0, 1000));
            L[i] = vi(all(s));
        }
        FracCascade fc(L);
        rep(q, 0, 30) {
            int x = common[rnd(0, sz(common) - 1)];
            int first = rnd(0, k - 1);
            vi got = fc.query(x, first);
            rep(i, first, k) {
                int exp = (int)(lower_bound(all(L[i]), x) - L[i].begin());
                check(got[i] == exp, "fractional.query");
            }
        }
    }

    printf(fails ? "contest: %d FAIL\n" : "contest: OK\n", fails);
    return fails ? 1 : 0;
}
