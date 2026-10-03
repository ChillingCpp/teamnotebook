# 06 — Tìm kiếm & tối ưu hóa DP

## Binary search trên kết quả

**Mục đích:** Tìm min/max giá trị thỏa điều kiện đơn điệu (`check(x)` đúng/fail liên tục) — bài "tìm nhỏ nhất sao cho luôn kịp".

**Ý tưởng / Observation:**
- Định dạng chuẩn: `check(mid)` đúng → thử nhỏ hơn (đưa `r = mid`), sai → `l = mid+1`. Kiểm tra 2 trường hợp biên `l`, `r+1` nếu không chắc.
- Thấy "đáp án nằm trong [A, B] và có tính chất: nếu x làm được thì y cũng làm được (đơn điệu)" → TKNP.
- Số thực: lặp 80-100 lần (không dùng `while(l<r)` vì sai số) — hoặc `while (r-l > 1e-7)`.

**Điều kiện sử dụng:**
- `check` PHẢI đơn điệu theo x. Không đơn điệu → xem ternary search / DP.

**Độ phức tạp:**
- Time: $O(\log(range) \cdot cost(check))$
- Space: $O(1)$

**Dependency:** template.cpp

```cpp
// id: binsearch
// trả về l nhỏ nhất có check(l) == true (kiểm tra cả biên nếu cần)
ll binSearch(ll l, ll r, auto check) {
    while (l < r) {
        ll m = l + (r - l) / 2;
        check(m) ? r = m : l = m + 1;
    }
    return l;
}
// double: for (int it = 0; it < 100; ++it) { mid = (l+r)/2; check(mid) ? r = mid : l = mid; }
// Khi r-l < range/2^80 → sai số double ~ range*1e-24, đủ cho mọi bài.
```

## Ternary search / Golden section

**Mục đích:** Tìm cực trị của hàm đơn điệu đỉnh (unimodal): 1 đỉnh max/min trên đoạn.

**Ý tưởng / Observation:**
- Đơn điệu đỉnh: tăng rồi giảm (max) hoặc giảm rồi tăng (min) — KHÔNG có nhiều đỉnh cục bộ.
- Số nguyên: `mid1 = l + (r-l)/3`, `mid2 = r - (r-l)/3`; so `f(mid1)` vs `f(mid2)` → cắt 1/3 đoạn mỗi bước.
- Số thực: golden section (tỉ lệ vàng) tiết kiệm 1 phép hàm/so sánh; hoặc bắn thẳng binary search trên đạo hàm dấu (nếu tính được).

**Điều kiện sử dụng:**
- Hàm CHỈ có 1 cực trị trên đoạn. Đa đỉnh → binary search theo đạo hàm / D&C / DP.
- Sai số: dừng khi `r - l < eps` (số thực), hoặc đến khi `l == r` (số nguyên, dùng `long long`).

**Độ phức tạp:**
- Time: $O(\log((b-a)/eps))$ với eps sai số mong muốn
- Space: $O(1)$

**Dependency:** template.cpp

```cpp
// id: ternary
// hàm số thực unimodal, trả về x cực trị (đổi < thành > để tìm max)
double ternarySearch(double l, double r, auto f) {
    while (r - l > 1e-7) {
        double m1 = l + (r - l) / 3, m2 = r - (r - l) / 3;
        f(m1) < f(m2) ? r = m2 : l = m1;  // đổi thành > nếu tìm max
    }
    return l;
}
// golden section (tiết kiệm 1 phép f mỗi vòng):
double golden(double a, double b, auto f) {
    const double r = (sqrt(5.0) - 1) / 2, eps = 1e-7;
    double x1 = b - r * (b - a), x2 = a + r * (b - a);
    double f1 = f(x1), f2 = f(x2);
    while (b - a > eps) {
        if (f1 < f2) {  // đổi > để tìm max
            b = x2; x2 = x1; f2 = f1;
            x1 = b - r * (b - a); f1 = f(x1);
        } else {
            a = x1; x1 = x2; f1 = f2;
            x2 = a + r * (b - a); f2 = f(x2);
        }
    }
    return a;
}
// int (đơn điệu): while (l < r) { m = l + (r-l)/2; check(m) ? r = m : l = m+1; }
```

## Convex hull trick (LineContainer)

**Mục đích:** Truy vấn $\min/\max (a \cdot x + b)$ nhanh — DP với hàm tuyến tính $dp[i] = \min(dp[j] + a[i] \cdot b[j] + c[i])$.

**Ý tưởng / Observation:**
- `LineContainer` insert + query: $O(\log n)$ / $O(\log n)$ — slope tăng dần + query x tăng dần → $O(1)$ deque (xem ghi chú).
- **Nhận ra bài:** DP có dạng $cost(i,j) = a_i \cdot b_j + c_i$ với $a_i$ đơn điệu → CHT.
- Nếu insert theo slope NGẪU NHIÊN → dùng multiset / Li Chao tree (đúng $O(\log^2)$ / $O(\log)$).
- Sản phẩm $a_i \cdot b_j$ với `ll` tràn → check cận ($\le 10^{18}$).

**Điều kiện sử dụng:**
- `m.x` tăng dần (query) → `LineContainer` hoạt động $O(\log n)$ bình thường; khi insert slope giảm dần, `long double` so sánh vẫn ổn.

**Độ phức tạp:**
- Time: $O(\log n)$ insert, $O(\log n)$ query (thêm dòng / truy vấn)
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: cht
struct Line {
    mutable ll k, m, p;  // y = k*x + m, p = giao điểm với dòng trước
    bool operator<(const Line& o) const { return k < o.k; }
    bool operator<(ll x) const { return p < x; }
};
struct LineContainer : multiset<Line, less<>> {
    static const ll inf = LLONG_MAX;
    ll div(ll a, ll b) {  // floor chia (xử lý cả b âm)
        return a / b - ((a ^ b) < 0 && a % b);
    }
    bool isect(iterator x, iterator y) {  // cập nhật giao điểm x∩y, true nếu x vô dụng
        if (y == end()) return x->p = inf, 0;
        if (x->k == y->k) x->p = x->m > y->m ? inf : -inf;
        else x->p = div(y->m - x->m, x->k - y->k);
        return x->p >= y->p;
    }
    void add(ll k, ll m) {  // thêm dòng y = k*x + m (max)
        auto z = insert({k, m, 0}), y = z++, x = y;
        while (isect(y, z)) z = erase(z);
        if (x != begin() && isect(--x, y)) isect(x, y = erase(y));
        while ((y = x) != begin() && (--x)->p >= y->p) isect(x, erase(y));
    }
    ll query(ll x) {  // max tại x — yêu cầu x không giảm nếu muốn O(log n) amortized
        assert(!empty());
        auto l = *lower_bound(x);
        return l.k * x + l.m;
    }
};
// tìm MIN: add(-k, -m) rồi query(x) = -(kết quả).  Hoặc đổi mọi so sánh > thành <.
// slope/query đơn điệu (tăng dần) → thay multiset bằng deque: thêm O(1) amortized.
```

## Divide & Conquer DP

**Mục đích:** Tính $dp[i] = \min_{lo(i) \le k < hi(i)} (f(i, k))$ khi điểm tối ưu `k` đơn điệu TĂNG theo i — gộp $O(N^2)$ → $O((N + range) \log N)$.

**Ý tưởng / Observation:**
- Điều kiện: `k*` (argmin) không giảm khi i tăng → D&C chia đôi, mỗi tầng duyệt tổng $O(range)$.
- **Nhận ra bài:** DP `dp[i][k]` với `k` đơn điệu (opt monotone) mà không thuộc dạng Knuth/CHT → thử D&C.
- `rec(L, R, LO, HI)` — tìm `mid` trong `[LO, HI)` rồi đệ quy trái/phải với biên cắt theo `best.k`.

**Điều kiện sử dụng:**
- `lo(i), hi(i)` — hàm giới hạn (thường `lo(i) = 0`, `hi(i) = n`). `f(i,k)` $O(1)$.
- Bài nhiều layer: mỗi layer là một `DCDP` mới — `f` capture mảng layer trước (đã tính xong toàn bộ).

**Độ phức tạp:**
- Time: $O((N + (hi-lo)) \log N)$
- Space: $O(N)$

**Dependency:** template.cpp

```cpp
// id: dcdp
// dp[i] = min_{lo(i) ≤ k < hi(i)} f(i, k) — k* đơn điệu TĂNG theo i
struct DCDP {
    vector<ll> dp;
    vi bestK;
    function<int(int)> lo, hi;   // miền k hợp lệ của mỗi i
    function<ll(int, int)> f;    // f(i, k) = ứng cử viên — gán theo bài
    DCDP(int n, function<int(int)> lo, function<int(int)> hi, function<ll(int, int)> f)
        : dp(n, LLONG_MAX), bestK(n), lo(lo), hi(hi), f(f) {}
    void rec(int L, int R, int LO, int HI) {
        if (L >= R) return;
        int mid = (L + R) >> 1;
        pair<ll, int> best(LLONG_MAX, LO);
        rep(k, max(LO, lo(mid)), min(HI, hi(mid))) {
            ll cur = f(mid, k);
            if (cur < best.first) best = {cur, k};
        }
        dp[mid] = best.first;
        bestK[mid] = best.second;
        rec(L, mid, LO, best.second + 1);
        rec(mid + 1, R, best.second, HI);
    }
    void solve() { rec(0, sz(dp), 0, sz(dp)); }
    // Dùng: DCDP d(n, [&](int i){ return 0; }, [&](int i){ return i; },
    //              [&](int i, int k){ return prev[k] + C(k, i); });
    //       d.solve();  → d.dp[i], d.bestK[i]
};
```

## Knuth optimization

**Mục đích:** Rút gọn phạm vi k của DP $dp[i][j] = \min_{i<k<j}(dp[i][k]+dp[k][j]) + C[i][j]$ — $O(N^2)$ thay vì $O(N^3)$.

**Ý tưởng / Observation:**
- Điều kiện: **quadrangle inequality** + `C` đơn điệu → $opt[i][j-1] \le opt[i][j] \le opt[i+1][j]$.
- Thường gặp: $C[i][j] = C[i][j-1] + C[i+1][j] + w[i][j]$, hoặc tổng chi phí chuỗi (matrix chain) — xem ghi chú DP.
- `dp[i][j]` với $j-i \ge 2$; $dp[i][i] = 0$, `dp[i][i+1]` = base.

**Điều kiện sử dụng:**
- Phải kiểm tra 2 bất đẳng thức (quadrangle): $f(b,c) \le f(a,d)$ và $f(a,c)+f(b,d) \le f(a,d)+f(b,c)$ với $a \le b \le c \le d$.

**Độ phức tạp:**
- Time: $O(N^2)$
- Space: $O(N^2)$

**Dependency:** template.cpp

```cpp
// id: knuth
// dp[i][j] = min_{i <= k < j} dp[i][k] + dp[k+1][j] + w(i, j)
// Đòi hỏi: w thỏa quadrangle inequality (xem mục Điều kiện), opt đơn điệu.
struct KnuthDP {
    int n;
    vector<vector<ll>> dp;
    vector<vector<int>> opt;
    function<ll(int, int)> w;  // w(i, j): chi phí gộp đoạn [i..j] — gán theo bài
    KnuthDP(int n, function<ll(int, int)> w) : n(n), w(w) {}
    void solve() {
        dp.assign(n, vector<ll>(n, 0));
        opt.assign(n, vector<int>(n, 0));
        rep(i, 0, n) opt[i][i] = i;  // biên: opt[i][i] = i (dp[i][i] = 0)
        rep(len, 1, n)
            rep(i, 0, n - len) {
                int j = i + len;
                int lo = opt[i][j - 1], hi = opt[i + 1][j];
                pair<ll, int> best(LLONG_MAX, lo);
                rep(k, lo, hi + 1) {
                    if (k >= j) break;  // k < j
                    ll cur = dp[i][k] + dp[k + 1][j];
                    if (cur < best.first) best = {cur, k};
                }
                opt[i][j] = best.second;
                dp[i][j] = best.first + w(i, j);
            }
    }
};
// Dùng: KnuthDP k(n, [&](int i, int j){ return sum(i, j); }); k.solve(); → k.dp[0][n-1]
```

## SOS DP (Sum over subsets)

**Mục đích:** Tính $F[i] = \sum_{j \subseteq i} f[j]$ cho mọi mask (sum over subsets) — $O(N \log N)$ thay vì $O(3^N)$.

**Ý tưởng / Observation:**
- `for b, for i, if i có bit b: F[i] += F[i ^ (1<<b)]` — duyệt bit ngoài, i tăng dần → tích lũy qua bit.
- Bài "tổng các f[j] với j là subset của i" → SOS. Bài "xor transform" → dùng FST (FWHT) — xem mục FST.
- $F[i] = \sum_{i \subseteq j} f[j]$ (superset): đổi hướng `if (!(i>>b & 1)) F[i] += F[i | 1<<b]`.

**Điều kiện sử dụng:**
- $N = 2^k$, `f` là mảng `ll`. $k \le 20$ → OK (mảng 10^6 phần tử).

**Độ phức tạp:**
- Time: $O(N \log N)$
- Space: $O(N)$

**Dependency:** template.cpp

```cpp
// id: sos
void sos(vector<ll>& F) {  // F[i] = Σ_{j ⊆ i} f[j] — F ban đầu = f
    int n = sz(F);
    for (int b = 0; (1 << b) < n; ++b)
        rep(i, 0, n) if (i & (1 << b)) F[i] += F[i ^ (1 << b)];
}
// superset: rep(i,0,n) if (!(i & (1<<b))) F[i] += F[i | (1<<b)];
// XOR transform (FWHT): for b, for i, if (!(i>>b&1)) tie(u, v) = pii(u+v, v-u); (xem FST)
```

## Longest Increasing Subsequence

**Mục đích:** LIS độ dài / dãy con đơn điệu (tăng chặt hoặc không giảm) — $O(n \log n)$.

**Ý tưởng / Observation:**
- `res` là dãy kết thúc min của LIS length tương ứng → `lower_bound` thay `upper_bound` cho tăng chặt (đổi để không giảm).
- Truy vết: `prev[i]` lưu chỉ số trước → đảo ngược từ cuối. Không cần truy vết → chỉ cần `sz(res)`.
- Bài "số dãy con đơn điệu" → $dp[i] = \sum dp[j]$ (sum trên segtree theo giá trị), không phải LIS.

**Điều kiện sử dụng:**
- `S` là mảng `vector<I>` (I có `operator<`). `S` rỗng → trả `{}`.

**Độ phức tạp:**
- Time: $O(n \log n)$
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: lis
vi lis(const vector<ll>& S) {  // trả về CHỈ SỐ của LIS (tăng chặt)
    if (S.empty()) return {};
    vi prev(sz(S));
    typedef pair<ll, int> p;
    vector<p> res;
    rep(i, 0, sz(S)) {
        auto it = lower_bound(all(res), p{S[i], 0});
        if (it == res.end()) res.emplace_back(), it = res.end() - 1;
        *it = {S[i], i};
        prev[i] = it == res.begin() ? 0 : (it - 1)->second;
    }
    vi ans(sz(res));
    int L = sz(res), cur = res.back().second;
    while (L--) ans[L] = cur, cur = prev[cur];
    return ans;
}
// không giảm (≤): đổi lower_bound → upper_bound (res = dãy kết thúc max length).
// không cần dãy → ans = sz(res) ngay.
```

## Bitmask DP tricks

**Mục đích:** DP trên tập con (TSP, matching trên mask, đếm) — các pattern vòng lặp chuẩn.

**Ý tưởng / Observation:**
- `for mask = 0 .. (1<<n)-1` rồi `for (int sub = mask; sub; sub = (sub-1) & mask)` duyệt mọi subset.
- TSP: $dp[mask][v] = \min(dp[mask \oplus (1 \ll v)][u] + c[u][v])$ — $O(2^n \cdot n^2)$, $n \le 20$.
- "Gán N việc cho N người" trên mask: `dp[mask] = giá trị tốt nhất khi đã gán các bit trong mask`.

**Điều kiện sử dụng:**
- $n \le 20$ (2^n), $n \le 25$ nếu DP nhanh. Đếm subset con → $O(3^n)$.

**Độ phức tạp:**
- Time: $O(2^n \cdot n)$ hoặc $O(3^n)$
- Space: $O(2^n)$

**Dependency:** template.cpp

```cpp
// id: bitmask
// duyệt mọi subset khác rỗng của mask:
//   for (int sub = mask; sub; sub = (sub - 1) & mask) { ... }
// duyệt mọi mask 0..2^n-1:
//   rep(mask, 0, 1 << n) { ... }
// TSP DP:
//   dp[1 << v0][v0] = 0;
//   for (mask) for (v in mask) for (u not in mask)
//       if (dp[mask][v] + c[v][u] < dp[mask | (1<<u)][u]) dp[mask | (1<<u)][u] = ...;
```

## Parallel binary search

**Mục đích:** $Q$ truy vấn cùng kiểu, mỗi truy vấn cần binary search trên đáp án $[0, N)$ mà hàm kiểm tra chỉ chạy được OFFLINE (1 lượt quét DS) — gộp $Q \cdot \log N$ lần chạy DS còn $\log N$ lần.

**Ý tưởng / Observation:**
- Mỗi vòng: mọi truy vấn chưa chốt lấy mid = $\lfloor(lo+hi)/2\rfloor$, gom tất cả $(mid, id)$ rồi chạy DS MỘT LẦN cho tất cả (thay vì mỗi query một lần) — đó là toàn bộ lợi ích của kĩ thuật.
- `batch(ids, mids)` thường viết: gom task → sort theo mid → quét DS 1 lượt → trả kết quả từng task.
- Duy trì bất biến $ans[i] \in [lo_i, hi_i]$ (kín 2 biên): $check = true$ → $lo = mid + 1$, ngược lại $hi = mid$; dừng khi mọi $lo = hi$ = đáp án.
- Nhận ra: "mỗi query 1 binary search mà check chỉ làm được theo lô/offline" → parallel binary search (giống offline nhưng theo chiều binary search).

**Điều kiện sử dụng:**
- Kiểm tra $(i, mid)$ phải OFFLINE / gộp lô được (DS clear() mỗi vòng là đủ). Đáp án trong $[0, hi0)$.

**Độ phức tạp:**
- Time: $O\big(\log N \cdot T_{batch}\big)$ — $T_{batch}$ = cost 1 vòng quét DS cho mọi query (thường $O((n + q) \log n)$).
- Space: $O(Q)$

**Dependency:** template.cpp

```cpp
// id: pbs
// Parallel Binary Search — mỗi vòng gọi batch() MỘT lần cho TẤT CẢ query còn chờ.
// batch(ids, mids) trả kq[j] = true ⇔ đáp án của ids[j] > mids[j]  (STRICT —
// code tính "mid đầu tiên mà kq = false": với >= thì kết quả lệch +1).
struct PBS {
    vi lo, hi;  // miền đáp án [lo, hi) — duy trì: ans ∈ [lo, hi)
    function<vector<bool>(const vi&, const vi&)> batch;
    PBS(int Q, int hi0, function<vector<bool>(const vi&, const vi&)> batch)
        : lo(Q, 0), hi(Q, hi0), batch(batch) {}
    vi solve() {  // kết quả: lo[i] == hi[i] == ans[i]
        while (true) {
            vi ids;
            rep(i, 0, sz(lo)) if (lo[i] < hi[i]) ids.push_back(i);
            if (ids.empty()) break;
            vi mids(sz(ids));
            rep(j, 0, sz(ids)) mids[j] = (lo[ids[j]] + hi[ids[j]]) / 2;
            vector<bool> kq = batch(ids, mids);
            rep(j, 0, sz(ids)) {
                int i = ids[j];
                if (kq[j]) lo[i] = mids[j] + 1;  // ans > mid
                else hi[i] = mids[j];
            }
        }
        return lo;
    }
};
// usage: PBS pbs(Q, N, [&](const vi& ids, const vi& mids) {
//     // gom task {mids[j], ids[j]} → sort → quét DS MỘT lần → kq[j] = (ans > mids[j])
//     vector<bool> kq(sz(ids));
//     ... return kq;
// });
// vi ans = pbs.solve();
```
