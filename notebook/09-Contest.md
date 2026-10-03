# 09 — Thi đấu (Contest tricks)

## Mod ops nhanh (add/sub/mul/div)

**Mục đích:** Phép toán modulo dùng lại mọi nơi: `addmod`, `submod`, `mulmod`, `divmod` — tránh viết lại mỗi bài.

**Điều kiện sử dụng:**
- `mod` là hằng (khuyên: `const ll MOD`). Với mod bất kỳ (không nguyên tố) → dùng `euclid` (mục Number theory).
- Chỉ chia được khi $\gcd(b, mod) = 1$ (mod nguyên tố → Fermat: `divmod(a,b) = mulmod(a, invmod(b))`).

**Độ phức tạp:**
- Time: $O(1)$ mỗi phép; `invmod` $O(\log mod)$
- Space: $O(1)$

**Dependency:** template.cpp, modpow

```cpp
// id: modops
// MOD lấy từ modpow (dependency) — đổi tại đó nếu bài dùng mod khác
ll norm(ll x) { x %= MOD; return x < 0 ? x + MOD : x; }
ll addmod(ll a, ll b) { return norm(a + b); }
ll submod(ll a, ll b) { return norm(a - b); }
ll mulmod(ll a, ll b) { return norm((__int128)a * b % MOD); }  // mod > 2^31 → __int128
ll divmod(ll a, ll b) { return mulmod(a, modpow(b, MOD - 2, MOD)); }  // mod nguyên tố
// mod không nguyên tố: gcd(b, mod) == 1 mới chia được; euclid tìm nghịch đảo.
```

## Sqrt decomposition (mảng chặn)

**Mục đích:** Chia mảng thành $\sqrt{n}$ block — update đoạn / query tổng nhanh hơn quét hết khi có NHIỀU query trộn lẫn.

**Điều kiện sử dụng:**
- $B = \lceil \sqrt{n} \rceil$; block index = `i / B`; số block = $\frac{n}{B} \approx \sqrt{n}$. Query phức tạp nhất $O(√n)$.
- Nhận ra: "N = Q = 10^5, update đoạn + query tổng bất kỳ" mà không muốn segtree → sqrt decomp (code ngắn, hằng số tốt). "Batch query" offline gộp theo block → Mo (mục 04) — sqrt decomp ở đây là PHIÊN BẢN ONLINE.

**Độ phức tạp:**
- Time: $O(√n)$ mỗi update/query; $O(n)$ build
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: sqrtdecomp
// mỗi block lưu val[block] (lazy) + tổng — add(l, r, x): block đủ thì cộng lazy O(1), block rìa quét O(√n).
struct SqrtDecomp {
    int n, B, nb;
    vector<ll> a, blockSum, blockAdd;
    SqrtDecomp(vector<ll>& v) : n(sz(v)), a(v) {
        B = max(1, (int)sqrt(n));
        nb = (n + B - 1) / B;
        blockSum.assign(nb, 0);
        blockAdd.assign(nb, 0);
        rep(b, 0, nb) rep(i, b * B, min(n, (b + 1) * B)) blockSum[b] += a[i];
    }
    void add(int l, int r, ll x) {  // [l, r)
        int bl = l / B, br = (r - 1) / B;
        if (bl == br) {
            rep(i, l, r) a[i] += x, blockSum[bl] += x;
        } else {
            rep(i, l, (bl + 1) * B) a[i] += x, blockSum[bl] += x;
            rep(b, bl + 1, br) blockSum[b] += x * B, blockAdd[b] += x;
            rep(i, br * B, r) a[i] += x, blockSum[br] += x;
            // KHÔNG reset blockAdd[rìa]: a[i] chưa nhận lazy cũ → query vẫn cộng blockAdd
        }
    }
    ll query(int l, int r) {  // [l, r)
        int bl = l / B, br = (r - 1) / B;
        ll res = 0;
        if (bl == br) {
            rep(i, l, r) res += a[i] + blockAdd[bl];
        } else {
            rep(i, l, (bl + 1) * B) res += a[i] + blockAdd[bl];
            rep(b, bl + 1, br) res += blockSum[b];
            rep(i, br * B, r) res += a[i] + blockAdd[br];
        }
        return res;
    }
};
// Invariant: blockSum[b] = Σ a[i] (toàn block) + blockAdd[b]·số phần tử.
// Update rìa: a[i] += x (a CHƯA nhận blockAdd → query rìa vẫn + blockAdd ✓).
```

## Build một file thi đấu (interactive/ad-hoc checklist)

**Mục đích:** Quy ước làm bài nhanh: đọc kỹ → viết brute → tối ưu; checklist các dạng bài hay gặp.

**Điều kiện sử dụng:**
- Interactive: `cin.tie(0)` để không tie output (flush chủ động thay vì cin tied). Flush sau mỗi output; đọc đến khi judge trả `end`/`-1`; KHÔNG assume thứ tự.
- Dạng bài "đúng/sai nhanh" → xem có thể lùi từ đáp án không (binary search + check).

**Độ phức tạp:**
- — (checklist, không code)

**Dependency:** —

```text
Interactive template:
    int main() {
        ios::sync_with_stdio(false); cin.tie(nullptr);  // KHÔNG tie → tự flush
        int t; cin >> t;
        while (t--) { ... cout << ans << endl; }  // endl = flush
        cout.flush();
    }
```
Ad-hoc workflow:
    1. Đọc kỹ, viết 2-3 case tay
    2. Brute $O(2^n)$ / $O(n!)$ cho n nhỏ → tìm pattern
    3. Suy luận invariant / bất biến / điều kiện đủ
    4. Tính edge cases: n = 0, 1, tất cả bằng nhau, đã sorted
    5. Constructive: thử case nhỏ nhất, xây từng bước (greedy chứng minh bằng trao đổi); bất khả thi → tìm điều kiện duy nhất


## Fractional cascading (tìm kiếm nhiều danh sách)

**Mục đích:** Tìm phần tử trong K danh sách đã sort mỗi lần $O(\log n)$ → $O(\log n + K)$ sau lần đầu — bài "đếm điểm chung / interpolate index".

**Điều kiện sử dụng:**
- Danh sách static (không thêm/xóa). Build $O(Σ|Li|)$, query $O(K + \log)$.
- Nhận ra: "K danh sách, mỗi query tìm trong TẤT CẢ" → fractional cascading (K nhỏ → binary từng list đã đủ).

**Độ phức tạp:**
- Time: $O(Σ|Li|)$ build, $O(K + \log n)$ query
- Space: $O(Σ|Li|)$

**Dependency:** template.cpp

```cpp
// id: fractional
// Li = vector<int> đã sort. mid[i][j] = index trong Li+1 ứng với lower_bound(Li[j], x)
struct FracCascade {
    vector<vi> L;
    vector<vi> mid;  // mid[i][j] = lower_bound index của L[i][j] trong L[i+1]
    FracCascade(vector<vi>& lists) : L(lists) {
        int k = sz(L);
        mid.assign(k - 1, vi());
        rep(i, 0, k - 1) {
            mid[i].resize(sz(L[i]));
            int p = 0;
            rep(j, 0, sz(L[i])) {
                while (p < sz(L[i + 1]) && L[i + 1][p] < L[i][j]) ++p;
                mid[i][j] = p;
            }
        }
    }
    // trả (vị trí x trong L[first], ...) cho mọi list — x đã có mặt (lower_bound)
    vi query(int x, int first = 0) {
        int p = (int)(lower_bound(all(L[first]), x) - L[first].begin());
        vi res(sz(L));
        res[first] = p;
        rep(i, first, sz(L) - 1) {
            p = mid[i][p];
            res[i + 1] = p;
        }
        return res;
    }
};
```
