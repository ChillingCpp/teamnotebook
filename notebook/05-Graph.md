# 05 — Đồ thị (Graph)

## Topological sort

**Mục đích:** Sắp xếp đỉnh theo chiều cạnh; phát hiện chu trình (kết quả < n phần tử).

**Ý tưởng / Observation:**
- Kahn (BFS đếm indegree): kết quả có `< n` đỉnh → có chu trình (các đỉnh còn lại reachable từ chu trình).
- DP trên DAG topo: duyệt theo thứ tự topo, relax mọi cạnh — 1 pass.
- Bài "thứ tự thỏa điều kiện phụ thuộc" → topo; "số đường đi trên DAG" → topo + DP.

**Điều kiện sử dụng:**
- Đồ thị có hướng. Multi-edge OK.

**Độ phức tạp:**
- Time: $O(V + E)$
- Space: $O(V)$

**Dependency:** template.cpp

```cpp
// id: topo
vi topoSort(const vector<vi>& gr) {  // gr[u] = danh sách kề đi ra
    vi indeg(sz(gr)), q;
    for (auto& li : gr) for (int x : li) indeg[x]++;
    rep(i, 0, sz(gr)) if (indeg[i] == 0) q.push_back(i);
    rep(j, 0, sz(q)) for (int x : gr[q[j]]) if (--indeg[x] == 0) q.push_back(x);
    return q;  // sz < n → có chu trình
}
```

## Bellman-Ford

**Mục đích:** Shortest path với cạnh âm; phát hiện chu trình âm (dist = -INF cho đỉnh bị ảnh hưởng).

**Ý tưởng / Observation:**
- Relax $V-1$ lần; lần thứ V còn relax → đỉnh đó nằm trên đường qua chu trình âm → lan `-inf` ra mọi nơi reach được.
- $V^2 \cdot \max|w| < 2^{63}$ để tránh tràn (hoặc check khi cộng).

**Điều kiện sử dụng:**
- Đồ thị có hướng/bất kỳ; `inf = LLONG_MAX/2` (cộng không tràn).

**Độ phức tạp:**
- Time: $O(V\cdot E)$
- Space: $O(V + E)$

**Dependency:** template.cpp

```cpp
// id: bellman
const ll BF_INF = LLONG_MAX / 2;
struct Edge {
    int u, v, w;
};
// dist[v] = BF_INF (không tới), BF_NEG_INF (đi qua chu trình âm)
const ll BF_NEG_INF = LLONG_MIN / 2;
vector<ll> bellmanFord(int n, vector<Edge>& ed, int s) {
    vector<ll> d(n, BF_INF);
    d[s] = 0;
    rep(i, 0, n - 1) {
        bool ch = false;
        for (auto& e : ed)
            if (d[e.u] != BF_INF && d[e.u] + e.w < d[e.v]) d[e.v] = d[e.u] + e.w, ch = true;
        if (!ch) break;
    }
    // lần V: đỉnh nào còn relax được thì nằm sau chu trình âm
    vi bad(n, 0);
    for (auto& e : ed)
        if (d[e.u] != BF_INF && d[e.u] + e.w < d[e.v]) bad[e.v] = 1;
    rep(cnt, 0, n) rep(i, 0, n) if (bad[i])  // lan ra mọi đỉnh reach được từ bad
        for (auto& e : ed) if (e.u == i) bad[e.v] = 1;
    rep(i, 0, n) if (bad[i]) d[i] = BF_NEG_INF;
    return d;
}
```

## Floyd-Warshall

**Mục đích:** Tất cả cặp đường đi ngắn nhất (APSP), gồm đường đi âm và chu trình âm.

**Ý tưởng / Observation:**
- `m[i][j] = inf` nếu kề; `m[i][i] = min(m[i][i], 0)` để tự loop không âm.
- Sau vòng k: `m[k][k] < 0` → mọi `i,j` reach qua k thành `-inf`.
- Check `m[i][k] != inf && m[k][j] != inf` trước khi cộng để tránh tràn/`inf + x`.

**Điều kiện sử dụng:**
- $N \le 400\text{-}1000$; `inf = 1LL<<62` (cộng 2 inf không tràn). Dùng cho "shortest path + còn lại" — thêm vòng k ở giữa.

**Độ phức tạp:**
- Time: $O(N^3)$
- Space: $O(N^2)$

**Dependency:** template.cpp

```cpp
// id: floyd
const ll FW_INF = 1LL << 62;
void floydWarshall(vector<vector<ll>>& m) {
    int n = sz(m);
    rep(i, 0, n) m[i][i] = min(m[i][i], 0LL);
    rep(k, 0, n) rep(i, 0, n) rep(j, 0, n)
        if (m[i][k] != FW_INF && m[k][j] != FW_INF) {
            ll nd = max(m[i][k] + m[k][j], -FW_INF);
            m[i][j] = min(m[i][j], nd);
        }
    rep(k, 0, n) if (m[k][k] < 0)
        rep(i, 0, n) rep(j, 0, n) if (m[i][k] != FW_INF && m[k][j] != FW_INF) m[i][j] = -FW_INF;
}
```

## SCC (Tarjan)

**Mục đích:** Thành phần liên thông mạnh của đồ thị có hướng — co-condensation graph, topo trên SCC.

**Ý tưởng / Observation:**
- `scc(g, callback)` duyệt component theo thứ tự **ngược topo** (component ra trước chỉ có cạnh vào từ sau) — trực tiếp dùng cho DP trên condensation.
- `comp[u] < comp[v]` → từ u KHÔNG tới được v (vì thứ tự ngược); co lại thành DAG.
- Bài "gộp strongly connected rồi DP" / "kiểm tra 2 đỉnh song song reach" → SCC.

**Điều kiện sử dụng:**
- `comp` gán trước khi callback (khi `low == val`). Mảng global `val, comp, z, Time, ncomps` — reset khi gọi lại.

**Độ phức tạp:**
- Time: $O(V + E)$
- Space: $O(V)$

**Dependency:** template.cpp

```cpp
// id: scc
vi sccVal, sccComp, sccZ;
int sccTime, sccCnt;
template <class G, class F>
int sccDfs(int j, G& g, F& f) {
    int low = sccVal[j] = ++sccTime, x;
    sccZ.push_back(j);
    for (auto e : g[j])
        if (sccComp[e] < 0) low = min(low, sccVal[e] ?: sccDfs(e, g, f));
    if (low == sccVal[j]) {
        while (true) {
            x = sccZ.back();
            sccZ.pop_back();
            sccComp[x] = sccCnt;
            if (x == j) break;
        }
        f(sccCnt);  // callback component id (xét theo thứ tự ngược topo)
        sccCnt++;
    }
    return sccVal[j] = low;
}
template <class G, class F>
void scc(G& g, F f) {
    int n = sz(g);
    sccVal.assign(n, 0);
    sccComp.assign(n, -1);
    sccTime = sccCnt = 0;
    rep(i, 0, n) if (sccComp[i] < 0) sccDfs(i, g, f);
}
// usage: scc(g, [&](int compId) { ... }); // compId tăng = thứ tự ngược topo
```

## 2-SAT

**Mục đích:** Tìm nghiệm cho hệ mệnh đề $(a \lor b) \land (\lnot a \lor c) \land \dots$ — biến đổi thành đồ thị IMP.

**Ý tưởng / Observation:**
- $a \lor b$ → $\lnot a \to b$ và $\lnot b \to a$; chạy SCC, nếu $x$ và $\lnot x$ cùng component → UNSAT.
- Gán giá trị theo thứ tự topo của condensation: component ra trước = false… (bản này gán trực tiếp trong DFS).
- `atMostOne({a,b,c})` — thêm biến auxiliary, chain: $(\lnot cur \lor \lnot b_i)$, $(\lnot cur \lor next)$…

**Điều kiện sử dụng:**
- Biến `x` ↔ node `2x` (true) / `2x+1` (false); `~x = x^1` (bit inverse). Số biến = N, node = 2N.

**Độ phức tạp:**
- Time: $O(N + E)$ (E = số mệnh đề)
- Space: $O(N + E)$

**Dependency:** template.cpp

```cpp
// id: twosat
struct TwoSat {
    int N;
    vector<vi> gr;
    vi values;  // 0 = false, 1 = true
    TwoSat(int n = 0) : N(n), gr(2 * n) {}
    int addVar() {
        gr.emplace_back();
        gr.emplace_back();
        return N++;
    }
    void either(int f, int j) {  // f ∨ j
        f = max(2 * f, -1 - 2 * f);
        j = max(2 * j, -1 - 2 * j);
        gr[f].push_back(j ^ 1);
        gr[j].push_back(f ^ 1);
    }
    void setValue(int x) { either(x, x); }
    void atMostOne(const vi& li) {  // tối đa 1 trong li là true
        if (sz(li) <= 1) return;
        int cur = ~li[0];
        rep(i, 2, sz(li)) {
            int next = addVar();
            either(cur, ~li[i]);
            either(cur, next);
            either(~li[i], next);
            cur = ~next;
        }
        either(cur, ~li[1]);
    }
    vi val, comp, z;
    int time = 0;
    int dfs(int i) {
        int low = val[i] = ++time, x;
        z.push_back(i);
        for (int e : gr[i])
            if (!comp[e]) low = min(low, val[e] ?: dfs(e));
        if (low == val[i])
            while (true) {
                x = z.back();
                z.pop_back();
                comp[x] = low;
                if (values[x >> 1] == -1) values[x >> 1] = x & 1;
                if (x == i) break;
            }
        return val[i] = low;
    }
    bool solve() {
        values.assign(N, -1);
        val.assign(2 * N, 0);
        comp = val;
        rep(i, 0, 2 * N) if (!comp[i]) dfs(i);
        rep(i, 0, N) if (comp[2 * i] == comp[2 * i + 1]) return false;
        return true;
    }
};
// usage: TwoSat ts(n); ts.either(0, ~3); ts.setValue(2); ts.solve(); ts.values[i];
```

## Cầu & biconnected components

**Mục đích:** Tìm cầu (edge thuộc mọi đường đi), điểm bậc 3, thành phần 2-vertex-connected — rebuild graph bỏ cầu/ điểm cắt.

**Ý tưởng / Observation:**
- DFS lowlink: `low[child] > num[u]` → cạnh `(u, child)` LÀ CẦU (không vòng quay).
- Node thuộc NHIỀU component → đó là điểm cắt; component callback nhận list edge-id.
- Bài "loại bỏ cầu → phân rã" / "2 bộ lọc 2 điểm" → biconnected.

**Điều kiện sử dụng:**
- `ed[a] = {b, edgeId}` — mỗi cạnh có id RIÊNG (với vô hướng: 2 hướng cùng id).

**Độ phức tạp:**
- Time: $O(V + E)$
- Space: $O(V + E)$

**Dependency:** template.cpp

```cpp
// id: biconnected
vi bcNum, bcSt;
vector<vector<pii>> bcEd;  // bcEd[u] = {v, edgeId}
int bcTime;
template <class F>
int bcDfs(int at, int par, F& f) {
    int me = bcNum[at] = ++bcTime, top = me;
    for (auto [y, e] : bcEd[at])
        if (e != par) {
            if (bcNum[y]) {
                top = min(top, bcNum[y]);
                if (bcNum[y] < me) bcSt.push_back(e);
            } else {
                int si = sz(bcSt);
                int up = bcDfs(y, e, f);
                top = min(top, up);
                if (up == me) {
                    bcSt.push_back(e);
                    vi comp(bcSt.begin() + si, bcSt.end());  // 1 component: list edge-id
                    f(comp);  // lvalue → lambda nhận vi&, const vi& hoặc vi đều được
                    bcSt.resize(si);
                } else if (up < me)
                    bcSt.push_back(e);
                // up > me → cạnh (at,y) LÀ CẦU
            }
        }
    return top;
}
template <class F>
void bicomps(F f) {
    bcNum.assign(sz(bcEd), 0);
    bcTime = 0;
    rep(i, 0, sz(bcEd)) if (!bcNum[i]) bcDfs(i, -1, f);
}
// usage: bcEd.assign(n, {}); int eid = 0;
//   bcEd[a].push_back({b, eid}); bcEd[b].push_back({a, eid++});
//   bicomps([&](vi& edges) { ... });   // điểm cắt = đỉnh xuất hiện >1 lần
// → dựng cây: xem section "Bridge tree & Block-cut tree" ngay sau.
```

## Bridge tree & Block-cut tree

**Mục đích:** Co đồ thị vô hướng về CÂY để trả lời "2 đỉnh còn nối sau khi xóa cầu / xóa đỉnh?" bằng so node trên cây — gộp 2 loại vào 1 chỗ (code tách rõ phần A / phần B).

**Ý tưởng / Observation:**
- **PHẦN A — BRIDGE TREE** (`bridgetree`): DFS lowlink tìm cầu (`low[v] > num[u]`), gộp 2 đầu qua cạnh KHÔNG phải cầu (DSU) → mỗi thành phần 1 node, mỗi cầu 1 cạnh của cây. `comp[u] == comp[v]` ⇔ đường u–v KHÔNG qua cầu nào.
- **PHẦN B — BLOCK-CUT TREE** (`blockcut`): gọi SAU `bicomps()` (mục Cầu & biconnected). Cầu không thuộc component nào (quy ước KACTL) → mỗi cầu tự là 1 block riêng. Node `[0, B)` = block, `[B, B+C)` = điểm cắt; đỉnh thuộc $\ge 2$ block ⇔ điểm cắt. `vtx[v]` = node đại diện của v (`cutId[v]` nếu là cut, không thì block chứa v).
- Dùng: xóa cut `x` → u, v còn nối ⇔ đường đi `vtx[u]` → `vtx[v]` trên cây KHÔNG đi qua node `cutId[x]` (LCA/DFS trên cây); xóa toàn bộ cầu → thành phần = node cây cầu.

**Điều kiện sử dụng:**
- `bcEd` dùng chung với `biconnected` (mỗi cạnh 1 id, 2 hướng cùng id). Đỉnh không có cạnh → `vtx = -1` (không thuộc block nào).

**Độ phức tạp:**
- Time: $O(V + E)$ cho cả 2 phần.
- Space: $O(V + E)$

**Dependency:** template.cpp, dsu

```cpp
// id: bridgetree
// ====== PHẦN A — BRIDGE TREE (cây cầu) ======
// ed[u] = {v, edgeId}; mỗi cạnh vô hướng có MỘT id dùng chung cho 2 hướng.
struct BridgeTree {
    vi comp;            // comp[u] = node trong cây cầu [0, ncomp)
    int ncomp = 0;
    vector<pii> edges;  // cạnh cây cầu (mỗi cầu 1 cạnh)
    BridgeTree(int n, vector<vector<pii>>& ed) {
        int m = 0;
        rep(u, 0, n) for (auto& p : ed[u]) m = max(m, p.second + 1);
        vector<char> isBr(m, 0);
        vector<pii> ends(m, pii(-1, -1));
        vi num(n, 0), low(n, 0);
        int tm = 0;
        function<void(int, int)> dfs = [&](int u, int pe) {
            num[u] = low[u] = ++tm;
            for (auto& [v, e] : ed[u]) {
                ends[e] = {u, v};
                if (e == pe) continue;
                if (!num[v]) {
                    dfs(v, e);
                    low[u] = min(low[u], low[v]);
                    if (low[v] > num[u]) isBr[e] = 1;  // cạnh (u, v) LÀ CẦU
                } else
                    low[u] = min(low[u], num[v]);
            }
        };
        rep(u, 0, n) if (!num[u]) dfs(u, -1);
        DSU d(n);
        rep(u, 0, n) for (auto& p : ed[u]) if (!isBr[p.second]) d.join(u, p.first);
        vi rid(n, -1);
        comp.assign(n, -1);
        rep(u, 0, n) {
            int r = d.find(u);
            if (rid[r] < 0) rid[r] = ncomp++;
            comp[u] = rid[r];
        }
        rep(e, 0, m) if (isBr[e]) edges.push_back({comp[ends[e].first], comp[ends[e].second]});
    }
};
```

**Dependency:** template.cpp

```cpp
// id: blockcut
// ====== PHẦN B — BLOCK-CUT TREE ======
// Gọi SAU bicomps() (id: biconnected). Cầu (không thuộc component nào) → 1 block riêng.
// Node [0, B) = block, [B, B+C) = điểm cắt; cutId[v] ≥ 0 nếu v là cut; vtx[v] = node của v.
void buildBlockCut(const vector<vector<pii>>& bcEd, const vector<vi>& comps, vi& cutId, vi& vtx,
                   vector<vi>& bcAdj) {
    int n = sz(bcEd), m = 0;
    rep(u, 0, n) for (auto& p : bcEd[u]) m = max(m, p.second + 1);
    vector<pii> ends(m, pii(-1, -1));
    vector<char> inComp(m, 0), seenE(m, 0);
    rep(b, 0, sz(comps)) for (int e : comps[b]) inComp[e] = 1;
    vector<vi> blocks = comps;
    rep(u, 0, n) for (auto& [v, e] : bcEd[u]) {
        ends[e] = {u, v};
        if (!seenE[e]) {
            seenE[e] = 1;
            if (!inComp[e]) blocks.push_back({e});  // cầu → block riêng
        }
    }
    int B = sz(blocks);
    vi seen(n, -1), nblk(n, 0);
    rep(b, 0, B) for (int e : blocks[b])
        for (int x : {ends[e].first, ends[e].second})
            if (x >= 0 && seen[x] != b) seen[x] = b, nblk[x]++;
    cutId.assign(n, -1);
    int C = 0;
    rep(v, 0, n) if (nblk[v] >= 2) cutId[v] = B + C++;  // ≥ 2 block → điểm cắt
    bcAdj.assign(B + C, {});
    vtx.assign(n, -1);
    fill(all(seen), -1);
    rep(b, 0, B) for (int e : blocks[b])
        for (int x : {ends[e].first, ends[e].second})
            if (x >= 0 && seen[x] != b) {
                seen[x] = b;
                if (cutId[x] >= 0)
                    vtx[x] = cutId[x], bcAdj[b].push_back(cutId[x]),
                        bcAdj[cutId[x]].push_back(b);
                else
                    vtx[x] = b;
            }
}
// usage: vector<vi> comps; bicomps([&](vi& c) { comps.push_back(c); });
//   vi cutId, vtx; vector<vi> bcAdj; buildBlockCut(bcEd, comps, cutId, vtx, bcAdj);
```

## Euler walk

**Mục đích:** Đường đi Euler (đi qua mọi cạnh đúng 1 lần) — tồn tại khi và chỉ khi liên thông + số đỉnh lẻ $\le 2$.

**Ý tưởng / Observation:**
- Hierholzer iterative: stack, khi không còn cạnh → đẩy vào kết quả (đảo ngược = thứ tự).
- `D[src]++` ban đầu cho phép PATH (không chỉ cycle); check cuối: `sz(ret) == nedges + 1`.
- Bài "xếp hình/domino thành chuỗi" → Euler path trên đồ thị cạnh.

**Điều kiện sử dụng:**
- Input: `gr[u] = {v, edgeId}`; vô hướng: 2 hướng cùng id (đánh dấu `eu[e]` đã dùng). Đồ thị vô hướng/có hướng tùy biến `D`.

**Độ phức tạp:**
- Time: $O(V + E)$
- Space: $O(V + E)$

**Dependency:** template.cpp

```cpp
// id: eulerwalk
vi eulerWalk(vector<vector<pii>>& gr, int nedges, int src = 0) {
    int n = sz(gr);
    vi D(n), its(n), eu(nedges), ret, s = {src};
    D[src]++;  // cho phép path (không cần quay về)
    while (!s.empty()) {
        int x = s.back(), y, e, &it = its[x], end = sz(gr[x]);
        if (it == end) {
            ret.push_back(x);
            s.pop_back();
            continue;
        }
        tie(y, e) = gr[x][it++];
        if (!eu[e]) {
            D[x]--;
            D[y]++;
            eu[e] = 1;
            s.push_back(y);
        }
    }
    for (int x : D)
        if (x < 0 || sz(ret) != nedges + 1) return {};  // không tồn tại
    return {ret.rbegin(), ret.rend()};
}
// đỉnh đầu/cuối = src (nếu cycle). muôn đường đi: chọn src, check D khác 0 ở ≤2 đỉnh.
```

## Binary lifting / LCA

**Mục đích:** LCA, khoảng cách 2 đỉnh, nhảy tổ tiên k bước, kiểm tra ancestor — $O(\log)$ mỗi query.

**Ý tưởng / Observation:**
- `jmp[i][v]` = tổ tiên $2^i$; root trỏ vào chính nó → code ngắn.
- $dist(u,v) = depth[u] + depth[v] - 2\cdot depth[lca]$; depth từ DFS (BFS cũng được).
- Bài "query trên đường đi u→v" (aggregate) → dùng LCA tách thành 2 nhánh đi lên + HLD (khi cần update).
- LCA $O(1)$ không log (chỉ LCA, không update): RMQ depth trên euler tour LOẠI ĐƯỜNG ĐI — xem "Euler tour trên cây — 3 loại".

**Điều kiện sử dụng:**
- Cây n (0-based); `P[root] = root`. `log` $\le 60$ với $n \le 10^{18}$? → `lg = 63 - clz`.

**Độ phức tạp:**
- Time: $O(n \log n)$ build, $O(\log n)$ query.
- Space: $O(n \log n)$

**Dependency:** template.cpp

```cpp
// id: lca
vector<vi> treeJump(vi& P) {  // P[v] = cha (root: P[root]=root)
    int on = 1, d = 1;
    while (on < sz(P)) on *= 2, d++;
    vector<vi> jmp(d, P);
    rep(i, 1, d) rep(j, 0, sz(P)) jmp[i][j] = jmp[i - 1][jmp[i - 1][j]];
    return jmp;
}
int jmpUp(vector<vi>& tbl, int nod, int steps) {
    rep(i, 0, sz(tbl)) if (steps & (1 << i)) nod = tbl[i][nod];
    return nod;
}
int lca(vector<vi>& tbl, vi& depth, int a, int b) {
    if (depth[a] < depth[b]) swap(a, b);
    a = jmpUp(tbl, a, depth[a] - depth[b]);
    if (a == b) return a;
    for (int i = sz(tbl); i--;) {
        int c = tbl[i][a], d = tbl[i][b];
        if (c != d) a = c, b = d;
    }
    return tbl[0][a];
}
// depth: BFS/DFS từ root. dist(a,b) = depth[a]+depth[b]-2*depth[lca(a,b)];
```

## Euler tour trên cây — 3 loại

**Mục đích:** Phẳng hóa cây thành mảng — CHỌN LOẠI THEO MỤC ĐÍCH: subtree, LCA/đường đi, hay Mo trên cây.

**Ý tưởng / Observation:**
- **Loại 1 — preorder (mảng n)** `eulertour`: `in[v]`/`out[v]` = thời gian vào/ra → subtree = `[in[v], out[v])`, mỗi đỉnh đúng 1 lần. Dùng cho subtree query/update bằng segtree, thứ tự DFS bottom-up (không cần lật trạng thái).
- **Loại 2 — đường đi (mảng 2n−1)** `eulpath`: ghi đỉnh khi vào + SAU MỖI con; `tin/tout` = lần xuất hiện đầu/cuối → subtree = `[tin, tout]`. **LCA(u, v) = đỉnh depth nhỏ nhất trên `[tin[u], tin[v]]`** → RMQ (sparse table mục 04) cho LCA $O(1)$ không log. Dùng khi cần LCA thật nhiều hoặc làm việc trực tiếp trên mảng đường đi.
- **Loại 3 — entry/exit (mảng 2n)** `motree`: mỗi đỉnh đúng 2 lần (vào + ra) → đặt tại **04 — Mo's algorithm** (đúng chỗ dùng): đường đi u→v = 1 đoạn, đỉnh NGOÀI đường xuất hiện chẵn lần → tự hủy khi flip. KHÔNG dùng cho DP cây/subtree (mỗi đỉnh 2 lần).
- Chọn: subtree thuần → loại 1; LCA $O(1)$ → loại 2; Mo query đường đi → loại 3; update/query đường đi có lazy → HLD (mục dưới).

**Điều kiện sử dụng:**
- DFS đệ quy với $n \le 2\cdot 10^5$ OK; sâu hơn → iterative DFS.

**Độ phức tạp:**
- Time: $O(n)$ build mỗi loại; lấy subtree/path = $O(1)$ đoạn, LCA qua RMQ $O(1)$ sau $O(n \log n)$ build.
- Space: $O(n)$ (loại 3: 2n)

**Dependency:** template.cpp

```cpp
// id: eulertour
int etTimer = 0;
vi etIn, etOut, etOrder;
void etDfs(int u, int p, vector<vi>& g) {
    etIn[u] = etTimer;
    etOrder[etTimer++] = u;
    for (int v : g[u])
        if (v != p) etDfs(v, u, g);
    etOut[u] = etTimer;  // subtree u = [etIn[u], etOut[u])
}
void buildEuler(vector<vi>& g, int root = 0) {
    int n = sz(g);
    etIn.assign(n, 0), etOut.assign(n, 0), etOrder.assign(n, 0);
    etTimer = 0;
    etDfs(root, -1, g);
}
```

**Dependency:** template.cpp

```cpp
// id: eulpath
// Loại 2 — ĐƯỜNG ĐI (mảng 2n−1): path[] ghi đỉnh khi vào + SAU MỖI CON;
// epTin/epTout = vị trí lần xuất hiện đầu/cuối trong epPath.
vi epTin, epTout, epPath, epDep;  // epDep[u] = depth (gốc depth 0)
void epDfs(int u, int p, vector<vi>& g, int d) {
    epDep[u] = d;
    epTin[u] = sz(epPath);
    epPath.push_back(u);
    for (int v : g[u])
        if (v != p) {
            epDfs(v, u, g, d + 1);
            epPath.push_back(u);
        }
    epTout[u] = sz(epPath) - 1;
}
void buildEulerPath(vector<vi>& g, int root = 0) {
    int n = sz(g);
    epTin.assign(n, 0), epTout.assign(n, 0), epDep.assign(n, 0);
    epPath.clear();
    epPath.reserve(2 * n - 1);
    epDfs(root, -1, g, 0);
}
// subtree v   = [epTin[v], epTout[v]] (liên tục trong epPath)
// LCA(u, v)   = epPath[ argmin depth trên epTin[u]..epTin[v] ]
//   → RMQ trên vector<pii> { -epDep[epPath[i]], i } (mục Sparse table):
//   RMQ<pair<ll,int>> rmq(v); int pos = rmq.query(tin, tout+1).second; LCA = epPath[pos];
```

## Heavy-Light Decomposition (HLD)

**Mục đích:** Phân rã cây thành các heavy path — query/update `max`/`sum`/`add` trên ĐƯỜNG ĐI u→v và subtree trong $O(\log^2 n)$.

**Ý tưởng / Observation:**
- Cây n thành $\le \log n$ light edge trên đường root→node → mỗi query chạm $\le \log n$ đoạn liên tục trong mảng `pos`.
- `process(u, v, op)` gộp các đoạn `[l, r)` theo thứ tự → gắn với segtree (tư duy: aggregate trên mảng `pos`).
- `VALS_EDGES = true` khi giá trị nằm ở CẠNH (offset `pos[u]+1`); `false` khi ở ĐỈNH.
- Bài "query max/add trên đường đi 2 đỉnh + update subtree" → HLD + segtree (KHÔNG thể chỉ LCA nếu có update).

**Điều kiện sử dụng:**
- Root = 0; adj nhận vào (bản này tự xóa parent khỏi danh sách kề → adj bị sửa). Dùng với `LazySeg` (lazy segtree mục 04) — dependency `lazysegtree`.

**Độ phức tạp:**
- Time: $O(n \log n)$ build, $O(\log^2 n)$ mỗi path query/update.
- Space: $O(n)$

**Dependency:** template.cpp, lazysegtree

```cpp
// id: hld
struct HLD {
    int N, tim = 0;
    vector<vi> adj;
    vi par, siz, rt, pos;
    LazySeg tree;
    HLD(vector<vi> adj_) : N(sz(adj_)), adj(adj_), par(N, -1), siz(N, 1), rt(N), pos(N) {
        vector<ll> init(N, 0);  // giá trị đỉnh ban đầu = 0 (đổi theo bài)
        tree = LazySeg(init);
        dfsSz(0);
        dfsHld(0);
    }
    void dfsSz(int v) {
        if (par[v] != -1) adj[v].erase(find(all(adj[v]), par[v]));
        for (int& u : adj[v]) {
            par[u] = v;
            dfsSz(u);
            siz[v] += siz[u];
            if (siz[u] > siz[adj[v][0]]) swap(u, adj[v][0]);  // heavy child đầu tiên
        }
    }
    void dfsHld(int v) {
        pos[v] = tim++;
        for (int u : adj[v]) {
            rt[u] = (u == adj[v][0] ? rt[v] : u);
            dfsHld(u);
        }
    }
    template <class B>
    void process(int u, int v, B op) {  // op(l, r) nửa mở trên pos
        for (; rt[u] != rt[v]; v = par[rt[v]]) {
            if (pos[rt[u]] > pos[rt[v]]) swap(u, v);
            op(pos[rt[v]], pos[v] + 1);
        }
        if (pos[u] > pos[v]) swap(u, v);
        op(pos[u], pos[v] + 1);  // VALS_EDGES: đổi pos[u] → pos[u] + 1
    }
    void modifyPath(int u, int v, ll val) {
        process(u, v, [&](int l, int r) { tree.add(l, r, val); });
    }
    ll queryPath(int u, int v) {  // max trên đường đi — đổi query theo bài
        ll res = LZY_NINF;
        process(u, v, [&](int l, int r) { res = max(res, tree.query(l, r)); });
        return res;
    }
    ll querySubtree(int v) {  // modifySubtree tương tự: tree.add(pos[v], pos[v]+siz[v], val)
        return tree.query(pos[v], pos[v] + siz[v]);
    }
};
```

## MST (Kruskal)

**Mục đích:** Cây khung nhỏ nhất (hoặc lớn nhất, đổi dấu trọng số) đồ thị vô hướng; build MST rồi trả lời truy vấn trên cây khung.

**Ý tưởng / Observation:**
- Sort cạnh + DSU → $O(E \log E)$; stop khi gộp được $n-1$ cạnh.
- Không liên thông → MST không tồn tại (kiểm tra số component = 1).
- **Tính chất cắt (cut property):** cạnh nhẹ nhất cắt qua bất kỳ tập đỉnh nào LUÔN thuộc MST. **Tính chất chu trình (cycle property):** cạnh nặng nhất trong chu trình KHÔNG thuộc MST.
- **Truy vấn trên MST** (bài xây MST rồi query):
  - Với cạnh ngoài MST $(u, v, w)$: $w = \max_{e \in P(u,v)} \mathrm{wt}(e)$ — mọi cạnh trên đường $P(u,v)$ trong MST nhẹ hơn. → Query "cạnh nặng nhất / nhẹ nhất trên đường 2 đỉnh" sau khi build MST → HLD hoặc binary lifting (mục HLD / Binary lifting).
  - Second-best MST: $\mathrm{best} = \min_{(u,v,w) \notin \mathrm{MST}} \left(w - \max_{e \in P(u,v)} \mathrm{wt}(e)\right)$ (kể cả cạnh trùng trong input).
  - MST duy nhất ⇔ mọi cạnh ngoài MST có $w > \max_{e \in P(u,v)} \mathrm{wt}(e)$ (bất đẳng nghiêm ngặt).
  - Nhiều query offline "2 đỉnh còn nối sau khi bỏ cạnh nặng nhất?" → sắp theo trọng số rồi Kruskal + DSU (gộp query vào lần gộp tương ứng).
- Đếm SỐ cây khung nhỏ nhất → Kirchhoff (matrix tree) — xem mục Determinant mod.

**Điều kiện sử dụng:**
- Đồ thị vô hướng; cạnh trùng OK. Prim ($O(E \log V)$) nhanh hơn với đồ thị dày. Query trên MST → dựng adjacency từ danh sách cạnh MST trả về.

**Độ phức tạp:**
- Time: $O(E \log E)$ build; query trên cây $O(\log n)$ (LCA/binary lifting) hoặc $O(\log^2 n)$ (HLD).
- Space: $O(V + E)$

**Dependency:** template.cpp, dsu

```cpp
// id: kruskal
// edges = {w, u, v} — trả về trọng số MST, -1 nếu không liên thông.
// used ≠ nullptr → nhận index các cạnh được chọn (dựng adjacency cây cho query).
ll kruskal(int n, vector<array<ll, 3>> edges, vi* used = nullptr) {
    sort(all(edges));
    DSU d(n);
    ll res = 0;
    int cnt = 0;
    rep(i, 0, sz(edges)) {
        auto [w, u, v] = edges[i];
        if (d.join(u, v)) {
            res += w;
            if (used) used->push_back(i);
            if (++cnt == n - 1) break;
        }
    }
    return cnt == n - 1 ? res : -1;
}
// Prim (đồ thị dày): priority queue trên cạnh nhỏ nhất ra khỏi tập đã chọn, O(E log V).
// Query trên MST: kruskal(n, e, &used) → adj[u].push_back({v, w}) rồi HLD / binary lifting.
```

## Push-relabel (max flow)

**Mục đích:** Luồng cực đại nhanh; sau khi chạy, min-cut đọc từ nhãn (H).

**Ý tưởng / Observation:**
- Highest-label + gap heuristic → thực tế rất nhanh ($n \le 5 \cdot 10^3$, $m \le 10^5$).
- **Min-cut:** `leftOfMinCut(v)` = $H[v] \ge V$ — tập S = đỉnh này, cắt = cạnh capacity >0 từ S sang T.
- Lấy flow thực: nhìn `e.f` (flow đi qua) hoặc $cap - residual$ với cạnh ngược.

**Điều kiện sử dụng:**
- `addEdge(s, t, cap, rcap=0)`; vô hướng: `addEdge(u,v,c,c)`. Self-loop bị bỏ.

**Độ phức tạp:**
- Time: $O(V^2\cdot √E)$ (worst), thực tế tốt hơn.
- Space: $O(V + E)$

**Dependency:** template.cpp

```cpp
// id: pushrelabel
struct PushRelabel {
    struct Edge {
        int dest, back;
        ll f, c;
    };
    vector<vector<Edge>> g;
    vector<ll> ec;
    vector<Edge*> cur;
    vector<vi> hs;
    vi H;
    PushRelabel(int n) : g(n), ec(n), cur(n), hs(2 * n), H(n) {}
    void addEdge(int s, int t, ll cap, ll rcap = 0) {
        if (s == t) return;
        g[s].push_back({t, sz(g[t]), 0, cap});
        g[t].push_back({s, sz(g[s]) - 1, 0, rcap});
    }
    void addFlow(Edge& e, ll f) {
        Edge& back = g[e.dest][e.back];
        if (!ec[e.dest] && f) hs[H[e.dest]].push_back(e.dest);
        e.f += f;
        e.c -= f;
        ec[e.dest] += f;
        back.f -= f;
        back.c += f;
        ec[back.dest] -= f;
    }
    ll calc(int s, int t) {
        int v = sz(g);
        H[s] = v;
        ec[t] = 1;
        vi co(2 * v);
        co[0] = v - 1;
        rep(i, 0, v) cur[i] = g[i].data();
        for (Edge& e : g[s]) addFlow(e, e.c);
        for (int hi = 0;;) {
            while (hs[hi].empty())
                if (!hi--) return -ec[s];
            int u = hs[hi].back();
            hs[hi].pop_back();
            while (ec[u] > 0)  // discharge u
                if (cur[u] == g[u].data() + sz(g[u])) {
                    H[u] = 1e9;
                    for (Edge& e : g[u])
                        if (e.c && H[u] > H[e.dest] + 1) H[u] = H[e.dest] + 1, cur[u] = &e;
                    if (++co[H[u]], !--co[hi] && hi < v)
                        rep(i, 0, v) if (hi < H[i] && H[i] < v) --co[H[i]], H[i] = v + 1;
                    hi = H[u];
                } else if (cur[u]->c && H[u] == H[cur[u]->dest] + 1)
                    addFlow(*cur[u], min(ec[u], cur[u]->c));
                else ++cur[u];
        }
    }
    bool leftOfMinCut(int a) { return H[a] >= sz(g); }
};
```

## MCMF (min-cost max-flow)

**Mục đích:** Luồng cực đại với chi phí cực tiểu — assignment, flow có trọng số.

**Ý tưởng / Observation:**
- Shortest path s→t với potential `pi` (giá trị dist thực = $dist[v] + pi[v] - pi[t]$) → xử lý cạnh âm (KHÔNG có chu trình âm).
- `setpi(s)` chạy Bellman-Ford trước nếu có cạnh âm (mục đích: potential hợp lệ).
- Lấy flow: nhìn `e.flow > 0` (bản này lưu `flow` riêng).

**Điều kiện sử dụng:**
- Chi phí âm OK (có setpi), nhưng không có chu trình âm. `INF = max/4` tránh tràn.

**Độ phức tạp:**
- Time: $O(F\cdot E \log V)$ (F = max flow), setpi $O(V\cdot E)$.
- Space: $O(V + E)$

**Dependency:** template.cpp

```cpp
// id: mcmf
const ll MCMF_INF = numeric_limits<ll>::max() / 4;
struct MCMF {
    struct edge {
        int from, to, rev;
        ll cap, cost, flow;
    };
    int N;
    vector<vector<edge>> ed;
    vi seen;
    vector<ll> dist, pi;
    vector<edge*> par;
    MCMF(int N) : N(N), ed(N), seen(N), dist(N), pi(N), par(N) {}
    void addEdge(int from, int to, ll cap, ll cost) {
        if (from == to) return;
        ed[from].push_back({from, to, sz(ed[to]), cap, cost, 0});
        ed[to].push_back({to, from, sz(ed[from]) - 1, 0, -cost, 0});
    }
    pair<ll, ll> maxflow(int s, int t) {
        fill(all(pi), 0);
        ll totflow = 0, totcost = 0;
        while (true) {
            // shortest path s→t bằng potential
            fill(all(seen), 0);
            fill(all(dist), MCMF_INF);
            dist[s] = 0;
            priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> q;
            q.push({0, s});
            while (!q.empty()) {
                auto [d, u] = q.top();
                q.pop();
                if (seen[u]) continue;
                seen[u] = 1;
                for (edge& e : ed[u])
                    if (e.cap - e.flow > 0 && !seen[e.to] &&
                        d + e.cost + pi[u] - pi[e.to] < dist[e.to]) {
                        dist[e.to] = d + e.cost + pi[u] - pi[e.to];
                        par[e.to] = &e;
                        q.push({dist[e.to], e.to});
                    }
            }
            if (!seen[t]) break;
            rep(i, 0, N) if (dist[i] < MCMF_INF) pi[i] += dist[i];
            ll fl = MCMF_INF;
            for (edge* x = par[t]; x; x = par[x->from]) fl = min(fl, x->cap - x->flow);
            for (edge* x = par[t]; x; x = par[x->from]) {
                x->flow += fl;
                ed[x->to][x->rev].flow -= fl;
            }
            totflow += fl;
            totcost += fl * pi[t];  // pi[t] = cost đường hiện tại
        }
        return {totflow, totcost};
    }
    void setpi(int s) {  // gọi TRƯỚC maxflow nếu có cạnh âm
        fill(all(pi), MCMF_INF);
        pi[s] = 0;
        int it = N, ch = 1;
        ll v;
        while (ch-- && it--)
            rep(i, 0, N) if (pi[i] != MCMF_INF)
                for (edge& e : ed[i])
                    if (e.cap && (v = pi[i] + e.cost) < pi[e.to]) pi[e.to] = v, ch = 1;
        assert(it >= 0);  // có chu trình âm
    }
};
```

## Bipartite matching (Hopcroft–Karp)

**Mục đích:** Matching cực đại đồ thị nhị phân — nén bài "chọn cặp", 2-SAT biến… nhanh hơn DFS matching.

**Ý tưởng / Observation:**
- BFS layering + DFS augmenting theo layer → $O(√V\cdot E)$.
- `btoa[u]` = đỉnh phải được ghép với u (−1 nếu không); `g[x]` = kề của đỉnh trái x.
- $V \le 10^3$ → DFS matching ($O(VE)$) đủ; lớn hơn → HK.

**Điều kiện sử dụng:**
- Đồ thị nhị phân, `g` chỉ chứa đỉnh trái, `btoa` size = số đỉnh phải, khởi tạo −1.

**Độ phức tạp:**
- Time: $O(√V \cdot E)$
- Space: $O(V + E)$

**Dependency:** template.cpp

```cpp
// id: hopcroft
bool hkDfs(int a, int L, vector<vi>& g, vi& btoa, vi& A, vi& B) {
    if (A[a] != L) return false;
    A[a] = -1;
    for (int b : g[a])
        if (B[b] == L + 1) {
            B[b] = 0;
            if (btoa[b] == -1 || hkDfs(btoa[b], L + 1, g, btoa, A, B)) return btoa[b] = a, true;
        }
    return false;
}
int hopcroftKarp(vector<vi>& g, vi& btoa) {  // g[x] = kề đỉnh trái x; btoa = -1
    int res = 0;
    vi A(sz(g)), B(sz(btoa)), cur, next;
    for (;;) {
        fill(all(A), 0);
        fill(all(B), 0);
        cur.clear();
        for (int a : btoa)
            if (a != -1) A[a] = -1;
        rep(a, 0, sz(g)) if (A[a] == 0) cur.push_back(a);
        for (int lay = 1;; lay++) {
            bool islast = false;
            next.clear();
            for (int a : cur)
                for (int b : g[a]) {
                    if (btoa[b] == -1) {
                        B[b] = lay;
                        islast = true;
                    } else if (btoa[b] != a && !B[b]) {
                        B[b] = lay;
                        next.push_back(btoa[b]);
                    }
                }
            if (islast) break;
            if (next.empty()) return res;
            for (int a : next) A[a] = lay;
            cur.swap(next);
        }
        rep(a, 0, sz(g)) res += hkDfs(a, 0, g, btoa, A, B);
    }
}
// usage: vi btoa(m, -1); int mt = hopcroftKarp(g, btoa); // btoa[j] = đỉnh trái match với j
// DFS matching (đơn giản, V ≤ 1e3): for each trái try find augmenting → O(V·E).
```

## Hungarian (weighted matching)

**Mục đích:** Matching cực tiểu chi phí (hoặc cực đại, đổi dấu chi phí) — gán N việc cho M người.

**Ý tưởng / Observation:**
- `hungarian(a)` trả `(minCost, match)` với `match[i]` = việc gán cho người i. Đòi hỏi $N \le M$.
- Dùng potential rút ngắn bước lặp → $O(N^2M)$; với $N, M \le 500$ OK.

**Điều kiện sử dụng:**
- `a[i][j]` = chi phí; đổi dấu để maximize. $N \le M$ (không đủ việc → bổ sung cột 0).

**Độ phức tạp:**
- Time: $O(N^2M)$
- Space: $O(NM)$

**Dependency:** template.cpp

```cpp
// id: hungarian
pair<ll, vi> hungarian(const vector<vi>& a) {  // trả (min cost, match): L[i] → R[match[i]]
    if (a.empty()) return {0, {}};
    int n = sz(a) + 1, m = sz(a[0]) + 1;
    vi u(n), v(m), p(m), ans(n - 1);
    rep(i, 1, n) {
        p[0] = i;
        int j0 = 0;
        vi dist(m, INT_MAX), pre(m, -1);
        vector<bool> done(m + 1);
        while (true) {
            done[j0] = true;
            int i0 = p[j0], j1, delta = INT_MAX;
            rep(j, 1, m)
                if (!done[j]) {
                    auto cur = a[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < dist[j]) dist[j] = cur, pre[j] = j0;
                    if (dist[j] < delta) delta = dist[j], j1 = j;
                }
            rep(j, 0, m) {
                if (done[j]) u[p[j]] += delta, v[j] -= delta;
                else dist[j] -= delta;
            }
            j0 = j1;
            if (!p[j0]) break;
        }
        while (j0) {
            int j1 = pre[j0];
            p[j0] = p[j1], j0 = j1;
        }
    }
    rep(j, 1, m) if (p[j]) ans[p[j] - 1] = j - 1;
    return {-v[0], ans};
}
```

## Min cut (sau max-flow)

**Mục đích:** Cắt nhỏ nhất giữa s,t = giá trị max-flow (max-flow min-cut) — đếm/ liệt kê cạnh cắt.

**Ý tưởng / Observation:**
- Chạy max-flow xong → BFS/DFS từ s trên cạnh `residual > 0`; tập reach = S (bên trái).
- Cạnh cắt = cạnh `capacity > 0` từ S sang `T`. Số đỉnh S = thuật toán sẵn (`leftOfMinCut` của Push-relabel).
- "Cắt nhỏ nhất rồi tối ưu cái khác" → max-flow, đọc S/T rồi gán.

**Điều kiện sử dụng:**
- Đồ thị sau khi có flow; direction của cạnh: $u \in S, v \in T$.

**Độ phức tạp:**
- Time: $O(V + E)$ BFS thêm (sau flow).
- Space: $O(V)$

**Dependency:** template.cpp, pushrelabel

```cpp
// id: mincut — tìm tập S sau max-flow (Push-relabel)
vi minCutSide(PushRelabel& pr) {
    int n = sz(pr.g);
    vi side(n, 0);
    rep(i, 0, n) side[i] = pr.leftOfMinCut(i);
    return side;  // side[v]=1 → v ở phía T (hoặc S tùy convention của leftOfMinCut)
}
// Cạnh cắt: mọi cạnh (u,v) với side[u] != side[v] && capacity > 0.
```
