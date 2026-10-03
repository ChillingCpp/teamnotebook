//@ ids: topo, bellman, floyd, scc, twosat, biconnected, bridgetree, blockcut, eulerwalk, lca, eulertour, eulpath
// Test ngẫu nhiên (seed cố định): mỗi snippet so với brute force tham chiếu.
using WAdj = vector<vector<pair<int, int>>>;  // (kề, trọng số)

// ==================== tham chiếu brute ====================
static vector<vector<char>> closure(const vector<vi>& g, bool reflex) {
    int n = sz(g);
    vector<vector<char>> r(n, vector<char>(n, 0));
    rep(i, 0, n) { if (reflex) r[i][i] = 1; for (int v : g[i]) r[i][v] = 1; }
    rep(k, 0, n) rep(i, 0, n) rep(j, 0, n) if (r[i][k] && r[k][j]) r[i][j] = 1;
    return r;
}
static ll simplePath(const WAdj& g, int i, int j) {  // đường đi đơn giản i→j nhẹ nhất (0 nếu i==j)
    ll best = (i == j) ? 0 : (ll)4e18;
    vector<char> vis(sz(g), 0);
    vis[i] = 1;
    function<void(int, ll)> dfs = [&](int u, ll w) {
        for (auto [v, c] : g[u]) {
            if (v == j) { best = min(best, w + c); continue; }
            if (!vis[v]) vis[v] = 1, dfs(v, w + c), vis[v] = 0;
        }
    };
    dfs(i, 0);
    return best;
}
static bool negCycleThrough(const WAdj& g, int c) {  // có chu trình âm đơn giản qua c?
    vector<char> vis(sz(g), 0);
    vis[c] = 1;
    function<bool(int, ll)> dfs = [&](int u, ll w) {
        for (auto [v, cpt] : g[u]) {
            if (v == c) { if (w + cpt < 0) return true; continue; }
            if (!vis[v]) { vis[v] = 1; if (dfs(v, w + cpt)) return true; vis[v] = 0; }
        }
        return false;
    };
    return dfs(c, 0);
}
static int ccCount(int n, const vector<pii>& el, int skipV, int skipE) {  // số cc (bỏ đỉnh/cạnh)
    vi vis(n, 0);
    int c = 0;
    rep(s, 0, n) if (s != skipV && !vis[s]) {
        ++c; vis[s] = 1;
        vi st = {s};
        while (!st.empty()) {
            int u = st.back(); st.pop_back();
            rep(i, 0, sz(el)) if (i != skipE) {
                auto [a, b] = el[i];
                if (a == u && b != skipV && !vis[b]) vis[b] = 1, st.push_back(b);
                if (b == u && a != skipV && !vis[a]) vis[a] = 1, st.push_back(a);
            }
        }
    }
    return c;
}
static vi bruteBridges(int n, const vector<pii>& el) {
    int base = ccCount(n, el, -1, -1);
    vi br(sz(el), 0);
    rep(i, 0, sz(el)) br[i] = ccCount(n, el, -1, i) > base;
    return br;
}
static vi blockRoots(int n, const vector<pii>& el) {  // gốc thành phần biconnected; -1 = cầu
    int m = sz(el), base = ccCount(n, el, -1, -1);
    vi isBr(m, 0), par(m), res(m, -1);
    rep(i, 0, m) isBr[i] = ccCount(n, el, -1, i) > base, par[i] = i;
    function<int(int)> find = [&](int x) { return par[x] == x ? x : par[x] = find(par[x]); };
    vector<vi> adjIds(n);
    rep(i, 0, m) adjIds[el[i].first].push_back(i), adjIds[el[i].second].push_back(i);
    vi pvis(n, 0), inPath(m, 0), cyc;
    function<void(int, int)> dfs = [&](int s, int u) {  // mọi chu trình đơn giản qua s
        for (int e : adjIds[u]) {
            if (isBr[e]) continue;
            int v = el[e].first ^ el[e].second ^ u;
            if (v == s) {
                if (!inPath[e]) { int r = find(e); for (int x : cyc) if (find(x) != r) par[find(x)] = r; }
            } else if (!pvis[v] && sz(cyc) + 1 < n) {
                pvis[v] = 1; inPath[e] = 1; cyc.push_back(e);
                dfs(s, v);
                cyc.pop_back(); inPath[e] = 0; pvis[v] = 0;
            }
        }
    };
    rep(s, 0, n) { fill(all(pvis), 0); pvis[s] = 1; cyc.clear(); dfs(s, s); }
    vi rid(m, -1);
    int id = 0;
    rep(i, 0, m) if (!isBr[i]) {
        int r = find(i);
        if (rid[r] < 0) rid[r] = id++;
        res[i] = rid[r];
    }
    return res;
}
static int naiveLca(const vi& par, const vi& dep, int a, int b) {
    while (dep[a] > dep[b]) a = par[a];
    while (dep[b] > dep[a]) b = par[b];
    while (a != b) a = par[a], b = par[b];
    return a;
}
static void treePD(const vector<vi>& g, int r, vi& par, vi& dep) {  // cha + độ sâu, gốc r
    par.assign(sz(g), -1); dep.assign(sz(g), 0);
    vi st = {r};
    par[r] = r;
    while (!st.empty()) {
        int u = st.back(); st.pop_back();
        for (int v : g[u]) if (par[v] < 0) par[v] = u, dep[v] = dep[u] + 1, st.push_back(v);
    }
}
// cây random (cha < con), gốc random; gán par (par[r]=r) + dep
static void randTreePD(int n, mt19937_64& rng, vector<vi>& g, int& r, vi& par, vi& dep) {
    g.assign(n, {});
    rep(i, 1, n) { int p = int(rng() % i); g[i].push_back(p), g[p].push_back(i); }
    r = int(rng() % n);
    treePD(g, r, par, dep);
}
static int randUG(int n, int m, mt19937_64& rng, vector<vector<pii>>& adj, vector<pii>& el) {
    adj.assign(n, {}); el.clear();
    int eid = 0;
    rep(e, 0, m) {
        int a = int(rng() % n), b = int(rng() % n);
        if (a == b) continue;  // không tự loop
        adj[a].push_back({b, eid}); adj[b].push_back({a, eid});
        el.push_back({a, b}); ++eid;
    }
    return eid;
}

int main() {
    mt19937_64 rng(20261003);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) { if (!ok) { ++fails; printf("FAIL: %s\n", msg); } };
    auto inSub = [](const vi& par, int w, int v) {  // w trong subtree của v (gốc có par[r]=r)?
        int u = w;
        while (u != v && par[u] != u) u = par[u];
        return u == v;
    };

    // ---------- topo ----------
    rep(tt, 0, 80) {
        int n = rnd(1, 8), m = rnd(0, 16);
        vector<vi> g(n);
        vector<pii> ed;
        rep(e, 0, m) { int u = rnd(0, n - 1), v = rnd(0, n - 1); g[u].push_back(v); ed.push_back({u, v}); }
        vi ord = topoSort(g);
        auto R = closure(g, false);  // đường đi dài ≥ 1
        bool cyc = false;
        rep(i, 0, n) if (R[i][i]) cyc = true;
        if (cyc) { check(sz(ord) < n, "topo.cycle"); continue; }
        check(sz(ord) == n, "topo.size");
        vi pos(n, -1), cnt(n, 0);
        rep(i, 0, sz(ord)) if (ord[i] >= 0 && ord[i] < n) cnt[ord[i]]++, pos[ord[i]] = i;
        rep(v, 0, n) check(cnt[v] == 1, "topo.perm");
        for (auto [u, v] : ed) check(pos[u] < pos[v], "topo.order");
    }

    // ---------- bellman ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 6), m = rnd(0, 12);
        vector<Edge> ed;
        WAdj g(n);
        vector<vi> adj(n);
        rep(e, 0, m) {
            int u = rnd(0, n - 1), v = rnd(0, n - 1), w = rnd(-8, 8);
            ed.push_back({u, v, w}), g[u].push_back({v, w}), adj[u].push_back(v);
        }
        int s = rnd(0, n - 1);
        vector<ll> d = bellmanFord(n, ed, s);
        auto R = closure(adj, true);
        vi negOn(n, 0);
        rep(c, 0, n) negOn[c] = negCycleThrough(g, c);
        rep(v, 0, n) {
            bool bad = false;
            rep(c, 0, n) if (negOn[c] && R[s][c] && R[c][v]) bad = true;
            ll exp = bad ? BF_NEG_INF : (!R[s][v] ? BF_INF : simplePath(g, s, v));
            check(d[v] == exp, "bellman.dist");
        }
    }

    // ---------- floyd ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 6), m = rnd(0, 12);
        vector<vector<ll>> mat(n, vector<ll>(n, FW_INF));
        rep(e, 0, m) { int u = rnd(0, n - 1), v = rnd(0, n - 1); mat[u][v] = min(mat[u][v], (ll)rnd(-8, 8)); }
        WAdj g(n);
        vector<vi> adj(n);
        rep(u, 0, n) rep(v, 0, n) if (mat[u][v] != FW_INF) g[u].push_back({v, (int)mat[u][v]}), adj[u].push_back(v);
        auto R = closure(adj, true);
        vi negOn(n, 0);
        rep(c, 0, n) negOn[c] = negCycleThrough(g, c);
        auto got = mat;
        floydWarshall(got);
        rep(u, 0, n) rep(v, 0, n) {
            bool bad = false;
            rep(c, 0, n) if (negOn[c] && R[u][c] && R[c][v]) bad = true;
            ll exp = bad ? -FW_INF
                   : (u == v ? min(0LL, mat[u][u]) : (!R[u][v] ? FW_INF : simplePath(g, u, v)));
            check(got[u][v] == exp, "floyd.dist");
        }
    }

    // ---------- scc ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 9), m = rnd(0, 16);
        vector<vi> g(n);
        rep(e, 0, m) g[rnd(0, n - 1)].push_back(rnd(0, n - 1));
        vi cb;
        scc(g, [&](int id) { if (id != sz(cb)) check(false, "scc.cb-order"); cb.push_back(id); });
        auto R = closure(g, true);
        vi hasComp(n, 0);
        rep(v, 0, n) if (sccComp[v] >= 0) hasComp[sccComp[v]] = 1;
        check(sz(cb) == accumulate(all(hasComp), 0), "scc.cb-count");
        rep(u, 0, n) rep(v, 0, n) {
            bool same = sccComp[u] == sccComp[v];
            check(same == (R[u][v] && R[v][u]), "scc.class");
            if (!same && R[u][v]) check(sccComp[u] > sccComp[v], "scc.reverse-topo");
        }
    }

    // ---------- twosat ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 14);
        TwoSat ts(n);
        vector<pii> cls;  // (lit, lit), lit = x hoặc -(x+1)
        vector<vi> amo;   // nhóm atMostOne (chỉ biến gốc)
        int nc = rnd(1, 10);
        rep(c, 0, nc) {
            int a = rnd(0, n - 1), b = rnd(0, n - 1);
            int la = rnd(0, 1) ? a : -(a + 1), lb = rnd(0, 1) ? b : -(b + 1);
            if (rnd(0, 3) == 0) ts.setValue(la), cls.push_back({la, la});
            else ts.either(la, lb), cls.push_back({la, lb});
        }
        if (n >= 2 && rnd(0, 1)) {
            vi li;
            rep(i, 0, rnd(2, min(5, n))) li.push_back(rnd(0, n - 1));
            sort(all(li)); li.erase(unique(all(li)), li.end());
            if (sz(li) >= 2) ts.atMostOne(li), amo.push_back(li);
        }
        auto litSat = [](int lit, int msk) { return lit >= 0 ? bool((msk >> lit) & 1) : !bool((msk >> (-lit - 1)) & 1); };
        auto satAll = [&](int msk) {
            for (auto [a, b] : cls) if (!litSat(a, msk) && !litSat(b, msk)) return false;
            for (auto& li : amo) { int c = 0; for (int x : li) c += (msk >> x) & 1; if (c > 1) return false; }
            return true;
        };
        bool sat = ts.solve(), any = false;
        rep(msk, 0, 1 << n) if (satAll(msk)) { any = true; break; }
        check(sat == any, "twosat.decide");
        if (sat) {
            int msk = 0;
            rep(i, 0, n) if (ts.values[i] == 1) msk |= 1 << i;
            check(satAll(msk), "twosat.model");
        }
    }

    // ---------- biconnected ----------
    rep(tt, 0, 50) {
        int n = rnd(1, 6), m = rnd(0, 12);
        vector<pii> el;
        int eid = randUG(n, m, rng, bcEd, el);
        vi roots = blockRoots(n, el), has(eid, 0);
        for (int r : roots) if (r >= 0) has[r] = 1;
        int nclass = accumulate(all(has), 0);
        vector<vi> comps;
        bicomps([&](vi& c) { comps.push_back(move(c)); });  // vi& — đúng convention doc
        vi used(eid, 0), cnt(nclass, 0);
        for (auto& c : comps) {
            if (c.empty()) { check(false, "biconnected.empty"); continue; }
            int r = roots[c[0]];
            for (int e : c) check(roots[e] == r, "biconnected.pure"), used[e]++;
            if (r >= 0) cnt[r]++;
        }
        rep(e, 0, eid) check(used[e] == (roots[e] >= 0), "biconnected.cover");
        rep(r, 0, nclass) check(cnt[r] == 1, "biconnected.once");
        vi inCnt(n, 0);  // >1 component ⇒ điểm cắt (một chiều của hợp đồng)
        for (auto& c : comps) {
            vi mark(n, 0);
            for (int e : c) mark[el[e].first] = 1, mark[el[e].second] = 1;
            rep(v, 0, n) inCnt[v] += mark[v];
        }
        int base = ccCount(n, el, -1, -1);
        rep(v, 0, n) if (inCnt[v] >= 2) check(ccCount(n, el, v, -1) > base, "biconnected.cut");
    }

    // ---------- bridgetree ----------
    rep(tt, 0, 50) {
        int n = rnd(1, 7), m = rnd(0, 12);
        vector<vector<pii>> ed;
        vector<pii> el;
        int eid = randUG(n, m, rng, ed, el);
        BridgeTree bt(n, ed);
        vi isBr = bruteBridges(n, el);
        vi p(n);
        iota(all(p), 0);
        auto find = [](vi& q, int x) { while (q[x] != x) x = q[x]; return x; };
        rep(e, 0, eid) if (!isBr[e]) {  // thành phần sau khi bỏ toàn bộ cầu
            int a = find(p, el[e].first), b = find(p, el[e].second);
            if (a != b) p[a] = b;
        }
        vi rid(n, -1), cbid(n, 0);
        int nc = 0;
        rep(v, 0, n) { int r = find(p, v); if (rid[r] < 0) rid[r] = nc++; cbid[v] = rid[r]; }
        rep(u, 0, n) rep(v, 0, n) check((bt.comp[u] == bt.comp[v]) == (cbid[u] == cbid[v]), "bridgetree.comp");
        int nbr = accumulate(all(isBr), 0);
        check(sz(bt.edges) == nbr, "bridgetree.nedges");
        check(bt.ncomp == nbr + ccCount(n, el, -1, -1), "bridgetree.ncomp");
        multiset<pii> A, B;
        for (auto& e : bt.edges) A.insert({min(e.first, e.second), max(e.first, e.second)});
        rep(e, 0, eid) if (isBr[e]) {
            int a = cbid[el[e].first], b = cbid[el[e].second];
            B.insert({min(a, b), max(a, b)});
        }
        check(A == B, "bridgetree.edges");
    }

    // ---------- blockcut ----------
    rep(tt, 0, 50) {
        int n = rnd(1, 6), m = rnd(0, 12);
        vector<pii> el;
        int eid = randUG(n, m, rng, bcEd, el);
        vector<vi> comps;
        bicomps([&](vi& c) { comps.push_back(move(c)); });  // vi& — đúng convention doc
        vi cutId, vtx;
        vector<vi> bcAdj;
        buildBlockCut(bcEd, comps, cutId, vtx, bcAdj);
        vi isBr = bruteBridges(n, el), used(eid, 0);
        for (auto& c : comps) for (int e : c) used[e]++;
        rep(e, 0, eid) check(used[e] == (isBr[e] ? 0 : 1), "blockcut.cover");
        vector<vi> blocks = comps;  // rebuild: comps rồi từng cầu (thứ tự xuất hiện đầu tiên)
        vi inC(eid, 0), seen(eid, 0);
        for (auto& c : comps) for (int e : c) inC[e] = 1;
        rep(u, 0, n) for (auto [v, e] : bcEd[u]) if (!seen[e]) {
            seen[e] = 1;
            if (!inC[e]) blocks.push_back({e});
        }
        int B = sz(blocks), C = 0, base = ccCount(n, el, -1, -1);
        rep(v, 0, n) {
            check((cutId[v] >= 0) == (ccCount(n, el, v, -1) > base), "blockcut.cut");
            C += cutId[v] >= 0;
        }
        check(sz(bcAdj) == B + C, "blockcut.nnodes");
        rep(v, 0, n) {  // vtx[v] = node chứa v
            vi bl;
            rep(b, 0, B) for (int e : blocks[b])
                if (el[e].first == v || el[e].second == v) { bl.push_back(b); break; }
            if (bl.empty()) check(vtx[v] == -1, "blockcut.vtx-iso");
            else if (cutId[v] >= 0) check(vtx[v] == cutId[v] && sz(bl) >= 2, "blockcut.vtx-cut");
            else check(sz(bl) == 1 && vtx[v] == bl[0], "blockcut.vtx-plain");
        }
        set<pii> exp, act;  // bcAdj đúng bằng quan hệ block – điểm cắt
        rep(b, 0, B) for (int e : blocks[b]) {
            int x = el[e].first, y = el[e].second;
            if (cutId[x] >= 0) exp.insert({b, cutId[x]});
            if (cutId[y] >= 0) exp.insert({b, cutId[y]});
        }
        rep(i, 0, sz(bcAdj)) for (int j : bcAdj[i])
            if (j >= 0 && j < sz(bcAdj)) act.insert({min(i, j), max(i, j)});
            else check(false, "blockcut.adj-range");
        check(exp == act, "blockcut.adj");
        if (base == 1 && B + C >= 1) {  // đồ thị liên thông → block-cut là cây
            int ec = 0;
            for (auto& l : bcAdj) ec += sz(l);
            check(ec == 2 * (B + C - 1), "blockcut.tree-edges");
            vi vis(B + C, 0), st = {0};
            vis[0] = 1;
            int seen2 = 1;
            while (!st.empty()) {
                int u = st.back(); st.pop_back();
                for (int w : bcAdj[u]) if (!vis[w]) vis[w] = 1, st.push_back(w), ++seen2;
            }
            check(seen2 == B + C, "blockcut.tree-connected");
        }
    }

    // ---------- eulerwalk ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 7), m = rnd(0, 12);
        vector<vector<pii>> gr;
        vector<pii> el;
        vi deg(n, 0);
        int eid = randUG(n, m, rng, gr, el);
        rep(e, 0, eid) deg[el[e].first]++, deg[el[e].second]++;
        int src = rnd(0, n - 1), odd = 0, iso = 0;
        for (int d2 : deg) odd += d2 & 1, iso += d2 == 0;
        // các đỉnh bậc>0 liên thông ⇔ cc(đồ thị) − số đỉnh cô lập == 1
        int oddComps = ccCount(n, el, -1, -1) - iso;
        bool exp = eid == 0 || (oddComps == 1 && deg[src] > 0 && (odd == 0 || (odd == 2 && deg[src] % 2 == 1)));
        vi w = eulerWalk(gr, eid, src);
        check((sz(w) > 0) == exp, "eulerwalk.exists");
        if (w.empty()) continue;
        check(sz(w) == eid + 1 && w[0] == src, "eulerwalk.len-src");
        vi usedE(eid, 0);
        bool adjOk = true;
        rep(i, 0, eid) {
            int found = -1;
            for (auto [y, e] : gr[w[i]]) if (y == w[i + 1] && !usedE[e]) { found = e; break; }
            if (found < 0) adjOk = false;
            else usedE[found] = 1;
        }
        check(adjOk, "eulerwalk.step");
        rep(e, 0, eid) check(usedE[e] == 1, "eulerwalk.each-edge");
        if (odd == 2) {  // exp đúng ⇒ src là 1 trong 2 đỉnh lẻ
            int other = -1;
            rep(v, 0, n) if (deg[v] % 2 == 1 && v != src) other = v;
            check(w.back() == other, "eulerwalk.end");
        } else check(w.back() == w[0], "eulerwalk.end");
    }

    // ---------- lca ----------
    rep(tt, 0, 40) {
        int n = rnd(1, 40);
        vi P(n, 0), dep(n, 0);
        rep(i, 1, n) P[i] = rnd(0, i - 1), dep[i] = dep[P[i]] + 1;
        vector<vi> g(n);
        rep(i, 1, n) g[i].push_back(P[i]), g[P[i]].push_back(i);
        auto tbl = treeJump(P);
        rep(a, 0, n) rep(b, 0, n) check(lca(tbl, dep, a, b) == naiveLca(P, dep, a, b), "lca.lca");
        rep(a, 0, n) rep(k, 0, dep[a] + 2) {
            int want = a;
            rep(i, 0, k) want = P[want];
            check(jmpUp(tbl, a, k) == want, "lca.jump");
        }
        rep(q, 0, 30) {  // dist = depth[u]+depth[v]-2*depth[lca] vs BFS
            int a = rnd(0, n - 1), b = rnd(0, n - 1);
            ll d = dep[a] + dep[b] - 2 * dep[lca(tbl, dep, a, b)];
            vi d2(n, -1);
            queue<int> qu;
            d2[a] = 0, qu.push(a);
            while (!qu.empty()) {
                int u = qu.front(); qu.pop();
                for (int v : g[u]) if (d2[v] < 0) d2[v] = d2[u] + 1, qu.push(v);
            }
            check(d == d2[b], "lca.dist");
        }
    }

    // ---------- eulertour ----------
    rep(tt, 0, 40) {
        int n = rnd(1, 40), r;
        vector<vi> g;
        vi par, dep;
        randTreePD(n, rng, g, r, par, dep);
        buildEuler(g, r);
        vi cnt(n, 0);
        for (int x : etOrder) if (x >= 0 && x < n) cnt[x]++;
        rep(v, 0, n) check(cnt[v] == 1, "eulertour.perm");
        rep(v, 0, n) {
            check(etIn[v] < etOut[v] && etOut[v] <= n, "eulertour.range");
            vi got(n, 0);
            rep(i, etIn[v], etOut[v]) if (etOrder[i] >= 0 && etOrder[i] < n) got[etOrder[i]] = 1;
            rep(w, 0, n) check(bool(got[w]) == inSub(par, w, v), "eulertour.subtree");
        }
    }

    // ---------- eulpath ----------
    rep(tt, 0, 40) {
        int n = rnd(1, 40), r;
        vector<vi> g;
        vi par, dep;
        randTreePD(n, rng, g, r, par, dep);
        buildEulerPath(g, r);
        check(sz(epPath) == 2 * n - 1 && epPath[0] == r, "eulpath.len");
        rep(i, 1, sz(epPath)) check(find(all(g[epPath[i - 1]]), epPath[i]) != g[epPath[i - 1]].end(), "eulpath.adj");
        rep(v, 0, n) {
            int f = -1, l = -1;
            rep(i, 0, sz(epPath)) if (epPath[i] == v) { if (f < 0) f = i; l = i; }
            check(epTin[v] == f && epTout[v] == l, "eulpath.tin-tout");
            check(epDep[v] == dep[v], "eulpath.depth");
            vi mark(n, 0);
            rep(i, epTin[v], epTout[v] + 1) if (epPath[i] >= 0 && epPath[i] < n) mark[epPath[i]] = 1;
            rep(w, 0, n) check(bool(mark[w]) == inSub(par, w, v), "eulpath.subtree");
        }
        rep(a, 0, n) rep(b, 0, n) {  // LCA = đỉnh depth nhỏ nhất trong [tin[u], tin[v]]
            int lo = min(epTin[a], epTin[b]), hi = max(epTin[a], epTin[b]), best = lo;
            rep(i, lo, hi + 1) if (epDep[epPath[i]] < epDep[epPath[best]]) best = i;
            check(epPath[best] == naiveLca(par, dep, a, b), "eulpath.lca");
        }
    }

    if (fails) { printf("graph1: %d loi\n", fails); return 1; }
    printf("graph1: OK\n");
    return 0;
}
