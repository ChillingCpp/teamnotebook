# 04 — Cấu trúc dữ liệu (Data structures)

## Segment tree (iterative)

**Mục đích:** Query tổng/max/min đoạn, update điểm — $O(\log n)$ mỗi thao tác, đủ cho hầu hết bài cần gộp kết hợp (sum/max/min/xor).

**Điều kiện sử dụng:**
- `n` không cần là lũy thừa 2. Hàm gộp phải kết hợp (associative).

**Độ phức tạp:**
- Time: $O(\log n)$ mỗi thao tác.
- Space: $O(2n)$

**Dependency:** template.cpp

```cpp
// id: segtree
// iterative bottom-up: b += n, e += n rồi đi lên — không đệ quy. Đổi T, f, unit để chuyển mục đích. Query [b, e) nửa mở; update ghi đè val.
struct SegTree {
    typedef ll T;
    static constexpr T unit = 0;
    T f(T a, T b) { return a + b; }  // đổi thành max(a,b) nếu cần
    vector<T> s;
    int n;
    SegTree(int n = 0, T def = unit) : s(2 * n, def), n(n) {}
    void update(int pos, T val) {
        for (s[pos += n] = val; pos /= 2;) s[pos] = f(s[pos * 2], s[pos * 2 + 1]);
    }
    T query(int b, int e) {  // [b, e)
        T ra = unit, rb = unit;
        for (b += n, e += n; b < e; b /= 2, e /= 2) {
            if (b % 2) ra = f(ra, s[b++]);
            if (e % 2) rb = f(s[--e], rb);
        }
        return f(ra, rb);
    }
};
```

## Lazy segment tree

**Mục đích:** Update đoạn (set/add) + query max (đổi được thành sum/min) trên khoảng — $O(\log n)$.

**Điều kiện sử dụng:**
- `LazySeg tr(v);` rồi `tr.add(L,R,x)` / `tr.set(L,R,x)` / `tr.query(L,R)` — nửa mở `[L, R)`. Ảnh hưởng chồng lên nhau OK (set áp lên add).

**Độ phức tạp:**
- Time: $O(\log n)$ mỗi thao tác.
- Space: $O(4n)$ mảng tĩnh (không new/delete)

**Dependency:** template.cpp

```cpp
// id: lazysegtree
const ll LZY_NINF = -(ll)4e18;  // đơn vị trung lập của max
struct LazySeg {  // mảng 4n: set/add đoạn + query max
    int n = 0;
    vector<ll> mx, ad, st;
    vector<char> hs;  // hs[p] = node p có set đang chờ (áp trước add)
    LazySeg(int n = 0) { init(n); }
    LazySeg(vector<ll>& a) {
        init(sz(a));
        if (n) build(1, 0, n, a);
    }
    void init(int n_) {
        n = n_;
        int m = 4 * max(1, n);
        mx.assign(m, LZY_NINF);
        ad.assign(m, 0);
        st.assign(m, 0);
        hs.assign(m, 0);
    }
    void build(int p, int l, int r, vector<ll>& a) {
        if (l + 1 == r) {
            mx[p] = a[l];
            return;
        }
        int m = (l + r) / 2;
        build(2 * p, l, m, a);
        build(2 * p + 1, m, r, a);
        mx[p] = max(mx[2 * p], mx[2 * p + 1]);
    }
    void setNode(int p, ll x) { mx[p] = st[p] = x; hs[p] = 1; ad[p] = 0; }
    void addNode(int p, ll x) {
        mx[p] += x;
        if (hs[p]) st[p] += x;
        else ad[p] += x;
    }
    void push(int p) {
        if (hs[p])
            setNode(2 * p, st[p]), setNode(2 * p + 1, st[p]), hs[p] = 0;
        else if (ad[p])
            addNode(2 * p, ad[p]), addNode(2 * p + 1, ad[p]), ad[p] = 0;
    }
    void set(int L, int R, ll x) { setRec(1, 0, n, L, R, x); }
    void add(int L, int R, ll x) { addRec(1, 0, n, L, R, x); }
    ll query(int L, int R) { return queryRec(1, 0, n, L, R); }
    void setRec(int p, int l, int r, int L, int R, ll x) {
        if (R <= l || r <= L) return;
        if (L <= l && r <= R) {
            setNode(p, x);
            return;
        }
        push(p);
        int m = (l + r) / 2;
        setRec(2 * p, l, m, L, R, x);
        setRec(2 * p + 1, m, r, L, R, x);
        mx[p] = max(mx[2 * p], mx[2 * p + 1]);
    }
    void addRec(int p, int l, int r, int L, int R, ll x) {
        if (R <= l || r <= L) return;
        if (L <= l && r <= R) {
            addNode(p, x);
            return;
        }
        push(p);
        int m = (l + r) / 2;
        addRec(2 * p, l, m, L, R, x);
        addRec(2 * p + 1, m, r, L, R, x);
        mx[p] = max(mx[2 * p], mx[2 * p + 1]);
    }
    ll queryRec(int p, int l, int r, int L, int R) {
        if (R <= l || r <= L) return LZY_NINF;
        if (L <= l && r <= R) return mx[p];
        push(p);
        int m = (l + r) / 2;
        return max(queryRec(2 * p, l, m, L, R), queryRec(2 * p + 1, m, r, L, R));
    }
};
// Dùng: LazySeg tr(v); tr.add(0, n, 5); tr.set(0, n, 7); tr.query(0, n);
```

## Segment tree 2D nén toạ độ

**Mục đích:** Truy vấn sum rectangle `(x1..x2, y1..y2)` + update điểm trên N điểm rời rạc — KHÔNG cần lưới đầy.

**Điều kiện sử dụng:**
- Mọi điểm biết trước (offline). `x` index `[0, n)`; `y` là tọa độ tùy ý (nén trong node). Query nửa mở theo x, `[y1, y2]` closed theo y (đổi `get` nếu cần).
- `prepare()` phải chạy TRƯỚC khi update/query (không thêm điểm sau khi build).

**Độ phức tạp:**
- Time: $O(\log^2 n)$ mỗi update/query.
- Space: $O(n \log n)$

**Dependency:** template.cpp

```cpp
// id: seg2d
// mỗi node x chứa danh sách y của các điểm treo trên nó → sort + unique + build segtree 1D (coordinate compression theo node). Điểm (x,y) đi vào O(log n) node → space O(n log n).
struct Seg1D {  // segtree sum trên mảng y đã nén
    vi ys;
    vector<ll> t;
    int get(int y) const { return (int)(lower_bound(all(ys), y) - ys.begin()); }
    void build() { t.assign(2 * sz(ys), 0); }
    void upd(int y, ll d) {  // y PHẢI có trong ys (mọi điểm đã qua prepare)
        for (int p = get(y) + sz(ys); p > 0; p >>= 1) t[p] += d;
    }
    ll query(int y1, int y2) {  // [y1, y2] closed — y1, y2 tùy ý, không cần thuộc ys
        int m = sz(ys);
        int l = (int)(lower_bound(all(ys), y1) - ys.begin());
        int r = (int)(upper_bound(all(ys), y2) - ys.begin());
        ll res = 0;
        for (l += m, r += m; l < r; l >>= 1, r >>= 1) {
            if (l & 1) res += t[l++];
            if (r & 1) res += t[--r];
        }
        return res;
    }
};
struct Seg2D {
    int n;
    vector<Seg1D> tree;
    Seg2D(int n) : n(n), tree(2 * n) {}
    void prepare(const vector<pii>& points) {  // GỌI TRƯỚC mọi update/query
        for (auto [x, y] : points)
            for (int p = x + n; p > 0; p >>= 1) tree[p].ys.push_back(y);
        for (auto& nd : tree) {
            sort(all(nd.ys));
            nd.ys.erase(unique(all(nd.ys)), nd.ys.end());
            nd.build();
        }
    }
    void update(int x, int y, ll delta) {
        for (int p = x + n; p > 0; p >>= 1) tree[p].upd(y, delta);
    }
    ll query(int x1, int x2, int y1, int y2) {  // x: [x1,x2) , y: [y1,y2]
        ll ans = 0;
        for (int l = x1 + n, r = x2 + n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) ans += tree[l++].query(y1, y2);
            if (r & 1) ans += tree[--r].query(y1, y2);
        }
        return ans;
    }
};
// Dùng: Seg2D st(n); st.prepare(pts); st.update(x,y,v); st.query(x1,x2,y1,y2);
```

## Persistent segment tree

**Mục đích:** Giữ LẠI mọi phiên bản (version) của mảng: query trên version bất kỳ, query "trong đoạn [l,r] phần tử nhỏ nhất thế nào" (offline).

**Điều kiện sử dụng:**
- 0-based, mảng giá trị index `[0, n)`. Chưa có point update "set" (cộng delta vẫn làm được qua node mới).
- Bài "query k-th nhỏ nhất trong đoạn [l,r]" → build version theo prefix, `kth(root[l-1], root[r], k)`.

**Độ phức tạp:**
- Time: $O(\log n)$ mỗi update/query; kth cũng $O(\log n)$.
- Space: $O((n + q) \log n)$

**Dependency:** template.cpp

```cpp
// id: persistent
// mỗi update tạo O(log n) node mới, CHỈ sửa đường đi gốc → version t = gốc của version t-1 + nhánh mới.
struct PST {
    struct Node {
        int l = 0, r = 0;
        int cnt = 0;
    };
    vector<Node> t;
    int n;
    PST(int n) : n(n) { t.reserve(n * 40); }
    int build(int l, int r) {  // mảng toàn 0
        int id = sz(t);
        t.push_back({});
        if (l + 1 < r) {
            int m = (l + r) / 2;
            t[id].l = build(l, m);
            t[id].r = build(m, r);
        }
        return id;
    }
    int upd(int prev, int l, int r, int pos) {  // +1 tại pos
        int id = sz(t);
        t.push_back(t[prev]);
        t[id].cnt++;
        if (l + 1 < r) {
            int m = (l + r) / 2;
            if (pos < m) t[id].l = upd(t[prev].l, l, m, pos);
            else t[id].r = upd(t[prev].r, m, r, pos);
        }
        return id;
    }
    int kth(int vl, int vr, int l, int r, int k) {  // k-th (1-based) trong diff vr - vl
        if (l + 1 == r) return l;
        int m = (l + r) / 2;
        int cntL = t[t[vr].l].cnt - t[t[vl].l].cnt;
        if (k <= cntL) return kth(t[vl].l, t[vr].l, l, m, k);
        return kth(t[vl].r, t[vr].r, m, r, k - cntL);
    }
};
// k-th nhỏ nhất trong a[l..r]: build root[0]; rep(i,0,n) root[i+1]=upd(root[i],...);
// int x = pst.kth(root[l], root[r+1], 0, sz, k);
```

## Prefix sum 2D

**Mục đích:** Sum rectangle bất kỳ trên lưới tĩnh — $O(1)$ query sau $O(RC)$ build.

**Điều kiện sử dụng:**
- Grid tĩnh (không update). Dùng `ll` khi tổng $> 2 \cdot 10^9$. Bài có update 1 điểm → chuyển sang segtree 2D (mục `seg2d`).

**Độ phức tạp:**
- Time: $O(RC)$ build, $O(1)$ query.
- Space: $O(RC)$

**Dependency:** template.cpp

```cpp
// id: prefix2d
// p[r+1][c+1] = a[r][c] + p[r][c+1] + p[r+1][c] - p[r][c]; rectangle [u,d)×[l,r) = 4 điểm.
struct SubMatrix {
    vector<vector<ll>> p;
    SubMatrix(vector<vector<ll>>& v) {
        int R = sz(v), C = sz(v[0]);
        p.assign(R + 1, vector<ll>(C + 1, 0));
        rep(r, 0, R) rep(c, 0, C) p[r + 1][c + 1] = v[r][c] + p[r][c + 1] + p[r + 1][c] - p[r][c];
    }
    ll sum(int u, int l, int d, int r) {  // [u,d) x [l,r)
        return p[d][r] - p[d][l] - p[u][r] + p[u][l];
    }
};
```

## DSU (Union-Find)

**Mục đích:** Gộp tập, tìm thành phần — kiểm tra cùng tập / gộp thành phần $O(α(n))$.

**Điều kiện sử dụng:**
- 0-based; undo → dùng bản rollback. Dùng khi cần "gộp 2 thứ lại và duy trì thông tin tập hợp" — MST Kruskal, offline query gộp, connected components.

**Độ phức tạp:**
- Time: $O(α(n))$ gần $O(1)$ mỗi thao tác.
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: dsu
// path compression + union by size → gần như O(1). Lưu extra info theo thành phần: mảng info[leader], cập nhật khi union.
struct DSU {
    vi e;
    DSU(int n) : e(n, -1) {}
    int size(int x) { return -e[find(x)]; }
    int find(int x) { return e[x] < 0 ? x : e[x] = find(e[x]); }
    bool join(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (e[a] > e[b]) swap(a, b);
        e[a] += e[b];
        e[b] = a;
        return true;
    }
    bool same(int a, int b) { return find(a) == find(b); }
};
```

## Rollback DSU

**Mục đích:** DSU có undo — dùng khi duyệt backtrack/Phân hoạch / cần quay lại trạng thái trước (Mo trên cây, offline 2D).

**Điều kiện sử dụng:**
- KHÔNG có path compression (chỉ union by size) — nếu không không undo được. `find` $O(\log n)$.

**Độ phức tạp:**
- Time: $O(\log n)$ find/join, $O(1)$ amortized rollback (đúng bằng số lần join).
- Space: $O(n + số join)$

**Dependency:** template.cpp

```cpp
// id: dsurollback
// lưu st = {(node, e[node] cũ)} trước mỗi lần gộp; rollback(t) trả về trạng thái tại time() == t. time() = kích thước stack = checkpoint.
struct RollbackUF {
    vi e;
    vector<pii> st;
    RollbackUF(int n) : e(n, -1) {}
    int size(int x) { return -e[find(x)]; }
    int find(int x) { return e[x] < 0 ? x : find(e[x]); }
    int time() { return sz(st); }
    void rollback(int t) {
        for (int i = time(); i-- > t;) e[st[i].first] = st[i].second;
        st.resize(t);
    }
    bool join(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (e[a] > e[b]) swap(a, b);
        st.push_back({a, e[a]});
        st.push_back({b, e[b]});
        e[a] += e[b];
        e[b] = a;
        return true;
    }
};
```

## Sparse table / RMQ

**Mục đích:** Query min/max/idempotent trên mảng tĩnh $O(1)$ — thay segtree khi KHÔNG có update.

**Điều kiện sử dụng:**
- Hàm gộp idempotent (min/max/gcd), KHÔNG dùng cho sum. `[a, b)` với `a < b` (assert).
- GHÉP TRÊN CÂY (LCA bằng euler tour + RMQ) cũng dùng bản này.

**Độ phức tạp:**
- Time: $O(n \log n)$ build, $O(1)$ query.
- Space: $O(n \log n)$

**Dependency:** template.cpp

```cpp
// id: sparse
// jmp[k][j] = min(jmp[k-1][j], jmp[k-1][j + 2^(k-1)]); query [a,b) lấy 2 đoạn chồng lấp 2^⌊log(b-a)⌋ — idempotent nên OK. dep = 31 - __builtin_clz(b - a).
template <class T>
struct RMQ {
    vector<vector<T>> jmp;
    RMQ(const vector<T>& V) : jmp(1, V) {
        for (int pw = 1, k = 1; pw * 2 <= sz(V); pw *= 2, ++k) {
            jmp.emplace_back(sz(V) - pw * 2 + 1);
            rep(j, 0, sz(jmp[k])) jmp[k][j] = min(jmp[k - 1][j], jmp[k - 1][j + pw]);
        }
    }
    T query(int a, int b) {  // [a, b), a < b
        assert(a < b);
        int dep = 31 - __builtin_clz(b - a);
        return min(jmp[dep][a], jmp[dep][b - (1 << dep)]);
    }
};
// Precompute log2: vi lg(n+1); rep(i,2,n+1) lg[i] = lg[i/2] + 1; → dep = lg[b-a];
```

## Treap (implicit)

**Mục đích:** Chuỗi/hàng đợi hỗ trợ split/merge theo vị trí: chèn, xóa, reverse đoạn, move đoạn, query tổng đoạn.

**Điều kiện sử dụng:**
- `rand()` đủ cho thi; đổi thành `mt19937` nếu WA lặp lại (rất hiếm).

**Độ phức tạp:**
- Time: $O(\log n)$ mỗi thao tác.
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: treap
// split(t, k) → (0..k-1, k..); merge(a, b) theo priority y ngẫu nhiên → cân bằng kỳ vọng O(log n). Augment: thêm c (size) để split theo index; thêm sum để query.
struct TNode {
    TNode *l = 0, *r = 0;
    int val, y, c = 1;
    TNode(int val) : val(val), y(rand()) {}
    void recalc();
};
int cnt(TNode* n) { return n ? n->c : 0; }
void TNode::recalc() { c = cnt(l) + cnt(r) + 1; }
pair<TNode*, TNode*> split(TNode* n, int k) {  // n => first k phần tử
    if (!n) return {};
    if (cnt(n->l) >= k) {
        auto pa = split(n->l, k);
        n->l = pa.second;
        n->recalc();
        return {pa.first, n};
    }
    auto pa = split(n->r, k - cnt(n->l) - 1);
    n->r = pa.first;
    n->recalc();
    return {n, pa.second};
}
TNode* merge(TNode* l, TNode* r) {
    if (!l) return r;
    if (!r) return l;
    if (l->y > r->y) {
        l->r = merge(l->r, r);
        l->recalc();
        return l;
    }
    r->l = merge(l, r->l);
    r->recalc();
    return r;
}
// ví dụ: chèn nút mới vào vị trí pos trong chuỗi:
TNode* ins(TNode* t, TNode* n, int pos) {
    auto pa = split(t, pos);
    return merge(merge(pa.first, n), pa.second);
}
// ví dụ: move đoạn [l, r) đến vị trí k (index mới trong chuỗi):
void moveRange(TNode*& t, int l, int r, int k) {
    TNode *a, *b, *c;
    tie(a, b) = split(t, l);
    tie(b, c) = split(b, r - l);  // a = [0,l) , b = [l,r) , c = [r,..)
    if (k <= l) t = merge(ins(a, b, k), c);
    else t = merge(a, ins(c, b, k - r));
}
```

## Order statistic tree (pbds)

**Mục đích:** Set có tìm phần tử thứ k & index của phần tử — "k-th nhỏ nhất trong tập động".

**Điều kiện sử dụng:**
- `#include <ext/pb_ds/tree_policy.hpp>`; namespace `__gnu_pbds`. `join` cần 2 tree rời (không giao).
- Set thường không có multiset → đổi `null_type` thành `int` (hoặc pair) để cho trùng.

**Độ phức tạp:**
- Time: $O(\log n)$ mỗi thao tác.
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: ost
// order_of_key(x) = số phần tử < x; find_by_order(k) = phần tử thứ k (0-based).
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;
template <class T>
using Tree = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;
// Dùng:
// Tree<int> t; t.insert(8); t.insert(10);
// t.order_of_key(9)  -> 1   (số phần tử < 9)
// *t.find_by_order(0) -> 8  (phần tử nhỏ nhất thứ 0)
```

## Mo's algorithm

**Mục đích:** Trả lời Q truy vấn đoạn offline khi có add/del 1 phần tử $O(1)$ — gộp answer mà không cần segment tree.

**Điều kiện sử dụng:**
- OFFLINE. `add/del(ind, end)` phải $O(1)$; `calc()` trả answer hiện tại. Mảng static giữa các truy vấn.
- Truy vấn nửa mở `[L, R)` — `Q = {L, R}` với R exclusive (chuyển `R+1` nếu input closed).
- Bài trên CÂY: query ĐƯỜNG ĐI → euler tour entry/exit (mỗi đỉnh 2 lần, code `motree` dưới đây) + flip; query SUBTREE → dùng thẳng mảng preorder (Euler tour mục 05) với add/del, không cần flip.

**Độ phức tạp:**
- Time: $O((N + Q)\cdot √N)$
- Space: $O(N + Q)$

**Dependency:** template.cpp

```cpp
// id: mo
// add(a[ind]) / del(a[ind]) / calc() — viết theo bài.
// sắp truy vấn theo block L/sqrt(Q) rồi R Ziczac (xor -(L/snk & 1)) → tổng chuyển O((N+Q)√N). blk = N/sqrt(Q) tối ưu; N=Q=1e5 → ~300-350.
vi mo(vector<pii> Q, vector<int>& a) {  // ví dụ: tính distinct count
    int n = sz(a), L = 0, R = 0, blk = max(1, (int)(n / sqrt(max(1, sz(Q)))));
    vi cnt(n + 1, 0), ans(sz(Q));
    ll cur = 0;
    auto add = [&](int i, int) {
        if (cnt[a[i]]++ == 0) ++cur;
    };
    auto del = [&](int i, int) {
        if (--cnt[a[i]] == 0) --cur;
    };
    vi s(sz(Q));
    iota(all(s), 0);
    auto K = [&](pii x) { return pii(x.first / blk, x.second ^ -(x.first / blk & 1)); };
    sort(all(s), [&](int x, int y) { return K(Q[x]) < K(Q[y]); });
    for (int qi : s) {
        pii q = Q[qi];
        while (L > q.first) add(--L, 0);
        while (R < q.second) add(R++, 1);
        while (L < q.first) del(L++, 1);
        while (R > q.second) del(--R, 0);
        ans[qi] = cur;
    }
    return ans;
}
```

```cpp
// id: motree
// Mo trên CÂY — Euler tour loại entry/exit (mỗi đỉnh xuất hiện ĐÚNG 2 lần):
// mảng cỡ 2n. Subtree(v) = [tin[v], tout[v]] liên tục. Đường đi u→v (tin[u] ≤ tin[v]):
//   • u là tổ tiên của v → đoạn [tin[u], tin[v]];
//   • ngược lại          → đoạn [tout[u], tin[v]] và CỘNG thêm LCA thủ công.
// Đỉnh NGOÀI đường xuất hiện chẵn lần → tự hủy khi flip (add/del = lật trạng thái).
vi motTin, motTout, motEuler;
int motTimer = 0;
void motDfs(int u, int p, vector<vi>& g) {
    motTin[u] = motTimer;
    motEuler[motTimer++] = u;
    for (int v : g[u])
        if (v != p) motDfs(v, u, g);
    motTout[u] = motTimer;
    motEuler[motTimer++] = u;
}
void motBuild(vector<vi>& g, int root = 0) {  // gọi trước mọi query
    int n = sz(g);
    motTin.assign(n, 0), motTout.assign(n, 0), motEuler.assign(2 * n, 0);
    motTimer = 0;
    motDfs(root, -1, g);
}
// Query (u,v) trong Mo: if (motTin[u] > motTin[v]) swap(u, v);
//   if (motTout[u] > motTin[v]) { L = motTin[u], R = motTin[v], lca = u; }   // u tổ tiên
//   else                       { L = motTout[u], R = motTin[v], lca = -1; }  // lca: +1 ở calc()
```

## Coordinate compression

**Mục đích:** Ánh xạ giá trị lớn/rời rạc ($10^9$) về index `[0, n)` nhỏ — cần cho segtree theo giá trị.

**Điều kiện sử dụng:**
- Nén sau khi biết toàn bộ tập giá trị (offline) — hoặc dùng `map` nếu cần online. Bài "k-th nhỏ nhất theo giá trị" → nén giá trị rồi làm trên index (kết hợp persistent segment tree mục persistent).

**Độ phức tạp:**
- Time: $O(n \log n)$ build, $O(\log n)$ query.
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: compress
// sort(unique) rồi lower_bound — sau nén, mọi so sánh thứ tự GIỮ NGUYÊN.
struct Compress {
    vector<ll> v;
    Compress(vector<ll>& a) : v(a) {
        sort(all(v));
        v.erase(unique(all(v)), v.end());
    }
    int get(ll x) const { return (int)(lower_bound(all(v), x) - v.begin()); }  // first ≥ x; x > max → size()
    int size() const { return sz(v); }
};
```
