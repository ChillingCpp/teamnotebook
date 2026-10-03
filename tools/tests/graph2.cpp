//@ ids: hld, kruskal, pushrelabel, mcmf, hopcroft, hungarian, mincut
// Test ngẫu nhiên (seed cố định): mỗi snippet so với brute force tham chiếu.

// ==================== tham chiếu brute ====================
static ll refMaxFlow(vector<vector<ll>> cap, int s, int t) {  // Edmonds-Karp
    int n = sz(cap);
    ll f = 0;
    for (;;) {
        vi par(n, -1);
        par[s] = s;
        queue<int> q;
        q.push(s);
        while (!q.empty()) {  // BFS đường tăng cường trên lưới dư
            int u = q.front(); q.pop();
            rep(v, 0, n) if (par[v] < 0 && cap[u][v] > 0) par[v] = u, q.push(v);
        }
        if (par[t] < 0) return f;
        ll fl = LLONG_MAX;
        for (int v = t; v != s; v = par[v]) fl = min(fl, cap[par[v]][v]);
        for (int v = t; v != s; v = par[v]) cap[par[v]][v] -= fl, cap[v][par[v]] += fl;
        f += fl;
    }
}
static ll refMinCut(const vector<array<ll, 3>>& arcs, int n, int s, int t) {  // quét mọi tập S
    ll best = LLONG_MAX;
    rep(mask, 0, 1 << n) if ((mask >> s & 1) && !(mask >> t & 1)) {
        ll c = 0;
        for (auto [u, v, w] : arcs) if ((mask >> u & 1) && !((mask >> v) & 1)) c += w;
        best = min(best, c);
    }
    return best;
}
struct RefMCMF {  // min-cost max-flow: SPFA tăng dần theo đường ngắn nhất (tham chiếu)
    struct E { int to, rev; ll cap, cost; };
    vector<vector<E>> g;
    int n;
    RefMCMF(int n) : g(n), n(n) {}
    void add(int u, int v, ll cap, ll cost) {
        g[u].push_back({v, sz(g[v]), cap, cost});
        g[v].push_back({u, sz(g[u]) - 1, 0, -cost});
    }
    pair<ll, ll> run(int s, int t) {
        ll F = 0, C = 0;
        for (;;) {
            vector<ll> d(n, (ll)4e18);
            vi pv(n, -1), pe(n, -1);
            vector<char> inq(n, 0);
            queue<int> q;
            d[s] = 0, q.push(s), inq[s] = 1;
            while (!q.empty()) {
                int u = q.front(); q.pop(); inq[u] = 0;
                rep(i, 0, sz(g[u])) {
                    E& e = g[u][i];
                    if (e.cap && d[u] + e.cost < d[e.to]) {
                        d[e.to] = d[u] + e.cost, pv[e.to] = u, pe[e.to] = i;
                        if (!inq[e.to]) inq[e.to] = 1, q.push(e.to);
                    }
                }
            }
            if (d[t] > (ll)3e18) return {F, C};
            ll fl = (ll)4e18;
            for (int v = t; v != s; v = pv[v]) fl = min(fl, g[pv[v]][pe[v]].cap);
            for (int v = t; v != s; v = pv[v]) { E& e = g[pv[v]][pe[v]]; e.cap -= fl, g[v][e.rev].cap += fl; }
            F += fl, C += fl * d[t];
        }
    }
};
static int bruteMatch(const vector<vi>& g, int nr) {  // quét mọi matching (nén bên trái ≤ 6)
    vi used(nr, 0);
    int best = 0;
    function<void(int, int)> rec = [&](int i, int cnt) {
        if (i == sz(g)) { best = max(best, cnt); return; }
        rec(i + 1, cnt);  // bỏ đỉnh trái i
        for (int b : g[i]) if (!used[b]) used[b] = 1, rec(i + 1, cnt + 1), used[b] = 0;
    };
    rec(0, 0);
    return best;
}
static ll bruteAssign(const vector<vi>& a) {  // min cost gán n người → m việc (n ≤ m)
    vi used(sz(a[0]), 0);
    ll best = LLONG_MAX;
    function<void(int, ll)> rec = [&](int i, ll cur) {
        if (i == sz(a)) { best = min(best, cur); return; }
        rep(j, 0, sz(a[0])) if (!used[j]) used[j] = 1, rec(i + 1, cur + a[i][j]), used[j] = 0;
    };
    rec(0, 0);
    return best;
}
static void randNet(int n, int m, mt19937_64& rng, PushRelabel& pr, vector<array<ll, 3>>& arcs,
                    vector<vector<ll>>& cap) {  // mạng ngẫu nhiên: addEdge + arcs + ma trận cap
    rep(e, 0, m) {
        int u = int(rng() % n), v = int(rng() % n);
        if (u == v) continue;
        ll c = int(rng() % 13);
        int typ = int(rng() % 3);
        if (typ == 0) {  // có hướng
            pr.addEdge(u, v, c);
            cap[u][v] += c;
            arcs.push_back({u, v, c});
        } else if (typ == 1) {  // vô hướng
            pr.addEdge(u, v, c, c);
            cap[u][v] += c, cap[v][u] += c;
            arcs.push_back({u, v, c}), arcs.push_back({v, u, c});
        } else {  // 2 chiều không cân xứng
            ll r = int(rng() % 9);
            pr.addEdge(u, v, c, r);
            cap[u][v] += c, cap[v][u] += r;
            arcs.push_back({u, v, c}), arcs.push_back({v, u, r});
        }
    }
}

int main() {
    mt19937_64 rng(20261003);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    int fails = 0;
    auto check = [&](bool ok, const char* msg) { if (!ok) { ++fails; printf("FAIL: %s\n", msg); } };

    // ---------- hld ----------
    rep(tt, 0, 25) {
        int n = rnd(1, 40);
        vector<vi> adj(n);
        vi par(n, -1), dep(n, 0);
        rep(i, 1, n) {  // cây random, cha có chỉ số nhỏ hơn → par[i] = cha thật
            int p = rnd(0, i - 1);
            par[i] = p, dep[i] = dep[p] + 1;
            adj[i].push_back(p), adj[p].push_back(i);
        }
        HLD hld(adj);  // HLD tự copy adj (rồi tự sửa danh sách kề)
        vi val(n, 0);  // giá trị đỉnh (tham chiếu)
        auto nlca = [&](int a, int b) {
            while (dep[a] > dep[b]) a = par[a];
            while (dep[b] > dep[a]) b = par[b];
            while (a != b) a = par[a], b = par[b];
            return a;
        };
        auto pathSet = [&](int a, int b) {  // các đỉnh trên đường a→b
            int c = nlca(a, b);
            vi r;
            for (int u = a; u != c; u = par[u]) r.push_back(u);
            for (int u = b; u != c; u = par[u]) r.push_back(u);
            r.push_back(c);
            return r;
        };
        auto inSub = [&](int w, int v) { while (w >= 0 && w != v) w = par[w]; return w == v; };
        rep(op, 0, 250) {
            int typ = rnd(0, 3);
            if (typ <= 1) {
                int a = rnd(0, n - 1), b = rnd(0, n - 1);
                ll x = rnd(-20, 20);
                if (typ == 0) {
                    hld.modifyPath(a, b, x);
                    for (int u : pathSet(a, b)) val[u] += x;
                } else {
                    ll exp = LZY_NINF;
                    for (int u : pathSet(a, b)) exp = max(exp, (ll)val[u]);
                    check(hld.queryPath(a, b) == exp, "hld.queryPath");
                }
            } else if (typ == 2) {
                int v = rnd(0, n - 1);
                ll exp = LZY_NINF;
                rep(w, 0, n) if (inSub(w, v)) exp = max(exp, (ll)val[w]);
                check(hld.querySubtree(v) == exp, "hld.querySubtree");
            } else {  // modifySubtree theo doc: tree.add(pos[v], pos[v]+siz[v], x)
                int v = rnd(0, n - 1);
                ll x = rnd(-20, 20);
                hld.tree.add(hld.pos[v], hld.pos[v] + hld.siz[v], x);
                rep(w, 0, n) if (inSub(w, v)) val[w] += x;
            }
        }
    }

    // ---------- kruskal ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 7), m = rnd(0, 16);
        vector<array<ll, 3>> ed;
        rep(e, 0, m) ed.push_back({rnd(-30, 30), rnd(0, n - 1), rnd(0, n - 1)});
        // brute: quét mọi tập n-1 cạnh, lấy trọng số cây khung nhỏ nhất
        ll best = LLONG_MAX;
        rep(mask, 0, 1 << m) if (__builtin_popcount(mask) == n - 1) {
            vi p(n);
            iota(all(p), 0);
            auto find = [](vi& q, int x) { while (q[x] != x) x = q[x]; return x; };
            ll w = 0;
            rep(i, 0, m) if (mask >> i & 1) {
                w += ed[i][0];
                int a = find(p, (int)ed[i][1]), b = find(p, (int)ed[i][2]);
                if (a != b) p[a] = b;
            }
            vi root(n);
            rep(v, 0, n) root[v] = find(p, v);
            if (count(all(root), root[0]) == n) best = min(best, w);
        }
        ll exp = best == LLONG_MAX ? -1 : best;
        vi used;
        ll got = kruskal(n, ed, &used);
        check(got == exp, "kruskal.mst");
        if (got != -1) {  // used = chỉ số trong dãy ĐÃ sắp xếp → chúng tạo cây khung
            auto sed = ed;
            sort(all(sed));
            check(sz(used) == n - 1, "kruskal.used-size");
            vi p(n);
            iota(all(p), 0);
            auto find = [](vi& q, int x) { while (q[x] != x) x = q[x]; return x; };
            ll sum = 0;
            for (int i : used) {
                sum += sed[i][0];
                int a = find(p, (int)sed[i][1]), b = find(p, (int)sed[i][2]);
                if (a != b) p[a] = b;
            }
            vi root(n);
            rep(v, 0, n) root[v] = find(p, v);
            check(sum == got && count(all(root), root[0]) == n, "kruskal.used-tree");
        }
    }

    // ---------- pushrelabel ----------
    rep(tt, 0, 60) {
        int n = rnd(2, 8), s = 0, t = n - 1;
        PushRelabel pr(n);
        vector<array<ll, 3>> arcs;
        vector<vector<ll>> cap(n, vector<ll>(n, 0));
        randNet(n, rnd(0, 18), rng, pr, arcs, cap);
        ll F = pr.calc(s, t);
        check(F == refMaxFlow(cap, s, t), "pushrelabel.maxflow");
        check(F == refMinCut(arcs, n, s, t), "pushrelabel.mincut");
    }

    // ---------- mcmf ----------
    rep(tt, 0, 40) {
        int n = rnd(2, 6), s = 0, t = n - 1;
        bool dag = rnd(0, 1);  // DAG → chi phí âm không thể tạo chu trình âm
        MCMF mf(n);
        RefMCMF rf(n);
        bool neg = false;
        rep(e, 0, rnd(0, 14)) {
            int u, v;
            if (dag) u = rnd(0, n - 2), v = rnd(u + 1, n - 1);
            else {
                u = rnd(0, n - 1), v = rnd(0, n - 1);
                if (u == v) continue;
            }
            ll c = rnd(1, 4), w = dag ? rnd(-6, 10) : rnd(0, 10);
            neg |= w < 0;
            mf.addEdge(u, v, c, w);
            rf.add(u, v, c, w);
        }
        if (neg) mf.setpi(s);  // theo doc: chạy TRƯỚC maxflow khi có cạnh âm
        auto [F, C] = mf.maxflow(s, t);
        auto [rF, rC] = rf.run(s, t);
        check(F == rF, "mcmf.flow");
        check(C == rC, "mcmf.cost");
        // đọc luồng thực từ e.flow: hợp lệ (0<=flow<=cap), bảo toàn, chi phí khớp
        // (cạnh ngược có cap=0, flow=-f nên chỉ tính flow>0, tránh nhân đôi)
        vector<ll> net(n, 0);
        ll cost = 0;
        rep(u, 0, n) for (auto& e : mf.ed[u]) {
            if (e.cap == 0) {
                if (e.flow > 0) check(false, "mcmf.rev-flow");
            } else if (e.flow < 0 || e.flow > e.cap)
                check(false, "mcmf.cap-range");
            if (e.flow > 0) {
                net[u] += e.flow, net[e.to] -= e.flow;
                cost += e.flow * e.cost;
            }
        }
        check(net[s] == F && net[t] == -F, "mcmf.net-endpoints");
        rep(v, 0, n) if (v != s && v != t) check(net[v] == 0, "mcmf.conservation");
        check(cost == C, "mcmf.flowcost");
    }

    // ---------- hopcroft ----------
    rep(tt, 0, 60) {
        int nl = rnd(1, 6), nr = rnd(1, 6);
        vector<vi> g(nl);
        rep(i, 0, nl) rep(j, 0, nr) if (rnd(0, 2) == 0) g[i].push_back(j);
        vi btoa(nr, -1);
        int mt = hopcroftKarp(g, btoa);
        check(mt == bruteMatch(g, nr), "hopcroft.max");
        vi seenL(nl, 0);
        int cnt = 0;
        bool ok = true;
        rep(b, 0, nr) if (btoa[b] != -1) {
            int a = btoa[b];
            if (a < 0 || a >= nl || seenL[a] || find(all(g[a]), b) == g[a].end()) ok = false;
            else seenL[a] = 1;
            cnt++;
        }
        check(ok && cnt == mt, "hopcroft.match");
    }

    // ---------- hungarian ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 6), m = rnd(n, 6);  // yêu cầu n ≤ m
        vector<vi> a(n, vi(m));
        rep(i, 0, n) rep(j, 0, m) a[i][j] = rnd(-40, 40);
        auto [cost, match] = hungarian(a);
        vi usedc(m, 0);
        ll sum = 0;
        bool ok = sz(match) == n;
        rep(i, 0, n) {
            int j = match[i];
            if (j < 0 || j >= m || usedc[j]) ok = false;
            else usedc[j] = 1, sum += a[i][j];
        }
        check(ok, "hungarian.match");
        check(cost == sum, "hungarian.cost");
        check(cost == bruteAssign(a), "hungarian.min");
    }

    // ---------- mincut ----------
    rep(tt, 0, 60) {
        int n = rnd(2, 8), s = 0, t = n - 1;
        PushRelabel pr(n);
        vector<array<ll, 3>> arcs;
        vector<vector<ll>> cap(n, vector<ll>(n, 0));
        randNet(n, rnd(1, 18), rng, pr, arcs, cap);
        ll F = pr.calc(s, t);
        vi side = minCutSide(pr);
        check(side[s] == 1 && side[t] == 0, "mincut.side-endpoints");
        ll cut = 0;
        for (auto [u, v, c] : arcs) if (side[u] && !side[v]) cut += c;  // cap(S→T)
        check(cut == F, "mincut.value");
        check(F == refMaxFlow(cap, s, t), "mincut.maxflow");
    }

    if (fails) { printf("graph2: %d loi\n", fails); return 1; }
    printf("graph2: OK\n");
    return 0;
}
