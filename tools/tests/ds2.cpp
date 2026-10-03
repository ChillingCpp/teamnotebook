//@ ids: seg2d, persistent, treap, ost, mo, motree
// Test: so khớp từng phép với brute force (lưới /暴力 / tập std::set).
int main() {
    mt19937_64 rng(20261004);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) {
        if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
    };

    // ---------- seg2d ----------
    rep(tt, 0, 25) {
        int n = rnd(1, 25), m = rnd(1, 15);
        set<pii> used;
        vector<pii> pts;
        int np = rnd(1, 50);
        rep(i, 0, np) {  // điểm (x, y) distinct — chỉ update tại điểm đã prepare
            pii p{rnd(0, n - 1), rnd(0, m)};
            if (used.insert(p).second) pts.push_back(p);
        }
        Seg2D st(n);
        st.prepare(pts);
        map<pii, ll> val;
        rep(op, 0, 200) {
            if (rnd(0, 1) == 0) {
                pii p = pts[rnd(0, sz(pts) - 1)];
                ll d = rnd(-20, 20);
                val[p] += d;
                st.update(p.first, p.second, d);
            } else {
                int x1 = rnd(0, n - 1), x2 = rnd(x1, n - 1);
                int y1 = rnd(0, m), y2 = rnd(y1, m);
                ll s = 0;
                for (auto& e : val)
                    if (e.first.first >= x1 && e.first.first <= x2 &&
                        e.first.second >= y1 && e.first.second <= y2) s += e.second;
                check(st.query(x1, x2 + 1, y1, y2) == s, "seg2d.query");
            }
        }
    }

    // ---------- persistent (k-th trong a[l..r]) ----------
    rep(tt, 0, 15) {
        int n = rnd(1, 40), szv = rnd(2, 200);
        vector<ll> a(n);
        rep(i, 0, n) a[i] = rnd(0, szv - 1);
        PST pst(szv);
        vi roots;
        roots.push_back(pst.build(0, szv));
        rep(i, 0, n) roots.push_back(pst.upd(roots.back(), 0, szv, a[i]));
        rep(qi, 0, 25) {
            int l = rnd(0, n - 1), r = rnd(l, n - 1), k = rnd(1, r - l + 1);
            vector<ll> sub(a.begin() + l, a.begin() + r + 1);
            sort(all(sub));
            check(pst.kth(roots[l], roots[r + 1], 0, szv, k) == sub[k - 1],
                  "persistent.kth");
        }
    }

    // ---------- treap (insert / moveRange) ----------
    rep(tt, 0, 30) {
        int n = rnd(1, 25);
        vector<ll> seq;
        TNode* t = nullptr;
        rep(i, 0, n) {
            ll v = rnd(-50, 50);
            seq.push_back(v);
            t = ins(t, new TNode(v), sz(seq) - 1);
        }
        rep(op, 0, 40) {
            if (rnd(0, 1) == 0) {  // chèn nút mới tại pos
                int pos = rnd(0, sz(seq));
                ll v = rnd(-50, 50);
                seq.insert(seq.begin() + pos, v);
                t = ins(t, new TNode(v), pos);
            } else {  // moveRange [l, r) — semantics theo code: k<=l → k, else → l+max(0,k-r)
                int l = rnd(0, sz(seq) - 1);
                int r = rnd(l + 1, sz(seq));
                int k = rnd(0, sz(seq));
                moveRange(t, l, r, k);
                vector<ll> seg(seq.begin() + l, seq.begin() + r);
                vector<ll> base;
                rep(i, 0, sz(seq)) if (i < l || i >= r) base.push_back(seq[i]);
                int p = (k <= l) ? k : l + max(0, k - r);
                seq.assign(base.begin(), base.begin() + p);
                seq.insert(seq.end(), seg.begin(), seg.end());
                seq.insert(seq.end(), base.begin() + p, base.end());
            }
        }
        vi flat;
        function<void(TNode*)> dfs = [&](TNode* nd) {
            if (!nd) return;
            dfs(nd->l);
            flat.push_back(nd->val);
            dfs(nd->r);
        };
        dfs(t);
        bool ok = sz(flat) == sz(seq);
        rep(i, 0, min(sz(flat), sz(seq))) if (flat[i] != seq[i]) ok = false;
        check(ok, "treap.content");
    }

    // ---------- ost (order statistic tree) ----------
    {
        Tree<int> t;
        set<int> ref;
        rep(op, 0, 600) {
            int typ = rnd(0, 2);
            if (typ == 0) {
                int x = rnd(0, 200);
                if (ref.insert(x).second) t.insert(x);
            } else if (typ == 1 && !ref.empty()) {
                int idx = rnd(0, sz(ref) - 1);
                auto it = next(ref.begin(), idx);
                t.erase(*it);
                ref.erase(it);
            } else if (!ref.empty()) {
                int k = rnd(0, sz(ref) - 1);
                auto it = next(ref.begin(), k);
                auto got = t.find_by_order(k);
                check(got != t.end() && *got == *it, "ost.find_by_order");
                int x = rnd(-5, 205);
                int exp = 0;
                for (int v : ref) if (v < x) ++exp;
                check(t.order_of_key(x) == exp, "ost.order_of_key");
            }
        }
        check(sz(t) == sz(ref), "ost.size");
    }

    // ---------- mo (distinct count, [L, R)) ----------
    rep(tt, 0, 30) {
        int n = rnd(1, 60);
        vi a(n);
        rep(i, 0, n) a[i] = rnd(0, n);  // cnt có n+1 ô → giá trị 0..n hợp lệ
        int q = rnd(1, 30);
        vector<pii> Q(q);
        rep(i, 0, q) {
            int l = rnd(0, n - 1);
            Q[i] = {l, rnd(l + 1, n)};
        }
        vi got = mo(Q, a);
        rep(i, 0, q) {
            set<int> s;
            rep(j, Q[i].first, Q[i].second) s.insert(a[j]);
            check(got[i] == sz(s), "mo.distinct");
        }
    }

    // ---------- motree (euler entry/exit — subtree & đường đi) ----------
    rep(tt, 0, 25) {
        int n = rnd(1, 25);
        vector<vi> g(n);
        vi par(n, -1), dep(n, 0);
        rep(v, 1, n) {
            int p = rnd(0, v - 1);
            g[p].push_back(v);
            par[v] = p;
            dep[v] = dep[p] + 1;
        }
        motBuild(g, 0);
        check(sz(motEuler) == 2 * n, "motree.eulerSize");
        auto isAnc = [&](int u, int v) {
            for (int x = v; x != -1; x = par[x])
                if (x == u) return true;
            return false;
        };
        rep(u, 0, n) {
            bool ok = motTin[u] < motTout[u] && motTout[u] < 2 * n &&
                      motEuler[motTin[u]] == u && motEuler[motTout[u]] == u;
            check(ok, "motree.tinTout");
            rep(v, 0, n) {  // subtree(v) = [tin, tout] liên tục
                bool inSub = motTin[u] <= motTin[v] && motTout[v] <= motTout[u];
                check(inSub == isAnc(u, v), "motree.subtree");
            }
        }
        auto onPath = [&](int u, int v) {
            set<int> s;
            int a = u, b = v;
            while (a != b) {
                if (dep[a] >= dep[b]) { s.insert(a); a = par[a]; }
                else { s.insert(b); b = par[b]; }
            }
            s.insert(a);
            return s;
        };
        auto lcaOf = [&](int u, int v) {
            int a = u, b = v;
            while (a != b) {
                if (dep[a] >= dep[b]) a = par[a];
                else b = par[b];
            }
            return a;
        };
        rep(q, 0, 50) {  // quy ước Mo (comment trong snippet)
            int u = rnd(0, n - 1), v = rnd(0, n - 1);
            int a1 = u, a2 = v;
            if (motTin[a1] > motTin[a2]) swap(a1, a2);
            bool anc = motTout[a1] > motTin[a2];
            int L = anc ? motTin[a1] : motTout[a1], R = motTin[a2];
            set<int> odd;
            rep(i, L, R + 1) {
                int x = motEuler[i];
                if (odd.count(x)) odd.erase(x); else odd.insert(x);
            }
            set<int> exp = onPath(u, v);
            if (!anc) exp.erase(lcaOf(u, v));  // không tổ tiên → cộng LCA thủ công
            check(odd == exp, "motree.path");
        }
    }

    printf(fails ? "ds2: %d FAIL\n" : "ds2: OK\n", fails);
    return fails ? 1 : 0;
}
