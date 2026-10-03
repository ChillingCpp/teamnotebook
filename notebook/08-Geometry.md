# 08 — Hình học (Geometry)

## Point (điểm & vector 2D)

**Mục đích:** Cấu trúc nền cho mọi hình học 2D: phép toán vector, cross/dot, xoay, khoảng cách.

**Ý tưởng / Observation:**
- `cross(a, b)` > 0 → b quay NGƯỢC kim đồng hồ so với a (left turn) — nền của orient test, hull.
- `dot` > 0 → góc nhọn; `dist2()` = bình phương khoảng cách — so sánh bằng `dist2` khi nào có thể (tránh `sqrt`).
- `perp()` = xoay +90°, `unit()` = vector đơn vị — dựng đường trung tuyến, pháp tuyến.

**Điều kiện sử dụng:**
- `T = double` (phép chia/nhân có `dist()`), `T = long long` (chỉ dùng cross/dot int — cẩn thận tràn với tọa độ $\le 10^9$ → cross ~$10^{18}$ OK borderline).

**Độ phức tạp:**
- Time: $O(1)$ mỗi phép
- Space: $O(1)$

**Dependency:** template.cpp

```cpp
// id: point
typedef long double D;  // hoặc ll nếu chỉ dùng cross/dot
template <class T>
struct Point {
    typedef Point P;
    T x, y;
    explicit Point(T x = 0, T y = 0) : x(x), y(y) {}
    bool operator<(P p) const { return tie(x, y) < tie(p.x, p.y); }
    bool operator==(P p) const { return tie(x, y) == tie(p.x, p.y); }
    bool operator!=(P p) const { return !(*this == p); }
    P operator+(P p) const { return P(x + p.x, y + p.y); }
    P operator-(P p) const { return P(x - p.x, y - p.y); }
    P operator*(T d) const { return P(x * d, y * d); }
    P operator/(T d) const { return P(x / d, y / d); }
    T dot(P p) const { return x * p.x + y * p.y; }
    T cross(P p) const { return x * p.y - y * p.x; }
    T cross(P a, P b) const { return (a - *this).cross(b - *this); }
    T dist2() const { return x * x + y * y; }
    D dist() const { return sqrt((D)dist2()); }
    D angle() const { return atan2((D)y, (D)x); }  // [-pi, pi]
    P unit() const { return *this / (T)dist(); }
    P perp() const { return P(-y, x); }  // xoay +90 độ
    P rotate(D a) const {  // xoay a rad ngược kim đồng hồ
        return P(x * cos(a) - y * sin(a), x * sin(a) + y * cos(a));
    }
    friend ostream& operator<<(ostream& os, P p) { return os << "(" << p.x << "," << p.y << ")"; }
};
typedef Point<D> Pd;
typedef Point<ll> Pl;
```

## Định hướng & điểm trên đoạn

**Mục đích:** Kiểm tra 3 điểm quay theo chiều nào (`sideOf`), điểm có nằm trên đoạn (`onSegment`) — nền của giao đoạn, point-in-polygon.

**Ý tưởng / Observation:**
- $\operatorname{cross}(b-a, c-a)$: > 0 → trái (ccw), < 0 → phải, == 0 → thẳng hàng (collinear).
- `onSegment`: thẳng hàng + `min(a.x,b.x) <= c.x <= max(...)` với cả x, y — không dùng epsilon nếu tọa độ nguyên.

**Điều kiện sử dụng:**
- `D` double → epsilon (`> 1e-12`); `ll` → so sánh chính xác (cross với |tọa độ| $\le 10^9$).

**Độ phức tạp:**
- Time: $O(1)$
- Space: $O(1)$

**Dependency:** template.cpp, point

```cpp
// id: onsegment
int sideOf(Pd a, Pd b, Pd c) {  // c so với đoạn ab: 1 = trái, -1 = phải, 0 = thẳng hàng
    D v = (b - a).cross(c - a);
    return (v > 1e-12) - (v < -1e-12);
}
bool onSegment(Pd s, Pd e, Pd p) {  // p trên đoạn [s,e] (kể cả đầu mút)
    if (sideOf(s, e, p) != 0) return false;
    return min(s.x, e.x) <= p.x && p.x <= max(s.x, e.x) &&
           min(s.y, e.y) <= p.y && p.y <= max(s.y, e.y);
}
// ll version: sideOf dùng cross > 0 / < 0 tuyệt đối (không epsilon).
```

## Khoảng cách điểm–đường thẳng / đoạn

**Mục đích:** Khoảng cách từ điểm p đến đường thẳng qua a,b (`lineDist`), đến đoạn [s,e] (`segDist`).

**Ý tưởng / Observation:**
- `lineDist` = $\frac{|\operatorname{cross}(b-a, p-a)|}{|b-a|}$ — tuyệt đối của diện tích / đáy.
- `segDist`: chiếu p lên đoạn, `t = clamp(dot/|e-s|^2, 0, 1)` — nếu chiếu nằm trong đoạn → khoảng cách vuông góc, không thì khoảng cách đến đầu mút gần nhất.
- Bài "tìm cặp điểm gần nhất" → dùng sweep line (mục Closest pair) thay vì $O(n^2)$.

**Điều kiện sử dụng:**
- `a == b` → nan (đường thẳng suy biến) — check trước nếu input không chắc chắn.

**Độ phức tạp:**
- Time: $O(1)$
- Space: $O(1)$

**Dependency:** template.cpp, point

```cpp
// id: linedist
D lineDist(const Pd& a, const Pd& b, const Pd& p) {
    return fabs((D)(b - a).cross(p - a) / (b - a).dist());  // khoảng cách ≥ 0 (khớp doc)
}
D segDist(Pd& s, Pd& e, Pd& p) {  // khoảng cách p đến đoạn [s,e]
    if (s == e) return (p - s).dist();
    auto d = (e - s).dist2();
    auto t = min(d, max((D)0, (p - s).dot(e - s)));
    return ((p - s) * d - (e - s) * t).dist() / d;
}
```

## Giao điểm đường thẳng / đoạn

**Mục đích:** Tìm giao điểm 2 đường thẳng vô hạn (`lineInter`), 2 đoạn thẳng (`segInter`) — kiểm tra chồng lấp.

**Ý tưởng / Observation:**
- `lineInter`: diện tích tam giác / diện tích → tọa độ giao qua Cramer (2 phương trình 2 ẩn).
- `segInter`: 4 phép `sideOf` — 2 đoạn chéo nhau nếu mỗi đoạn phân biệt 2 đầu mút đoạn kia; + check `onSegment` cho trường hợp song song trùng.
- Parallel (`cross == 0`) → không giao hoặc trùng (trả `nullopt` / cần check thủ công).

**Điều kiện sử dụng:**
- `D = double` → epsilon; `ll` → dùng determinant trực tiếp (không chia).

**Độ phức tạp:**
- Time: $O(1)$
- Space: $O(1)$

**Dependency:** template.cpp, point, onsegment

```cpp
// id: seginter
// giao 2 đường thẳng vô hạn — nullopt nếu song song
optional<Pd> lineInter(Pd a, Pd b, Pd c, Pd d) {
    D v = (b - a).cross(d - c);
    if (fabs(v) < 1e-12) return nullopt;
    return a + (b - a) * ((c - a).cross(d - c) / v);
}
// kiểm tra 2 đoạn có giao (kể cả chồng/chạm):
bool segInter(Pd s1, Pd e1, Pd s2, Pd e2) {
    D a = (e2 - s2).cross(s1 - s2), b = (e2 - s2).cross(e1 - s2);
    D c = (e1 - s1).cross(s2 - s1), d = (e1 - s1).cross(e2 - s1);
    return ((a > 0) != (b > 0)) && ((c > 0) != (d > 0)) ||
           (onSegment(s1, e1, s2) || onSegment(s1, e1, e2) ||
            onSegment(s2, e2, s1) || onSegment(s2, e2, e1));
}
// giao điểm (nếu có): auto p = lineInter(s1,e1,s2,e2); if (p && onSegment(s1,e1,*p) && onSegment(s2,e2,*p)) ...
```

## Convex hull (Andrew monotone chain)

**Mục đích:** Lớp bao lồi của tập điểm — tính chu vi/diện tích hull, bài "điểm nằm ngoài"/"bao tất cả".

**Ý tưởng / Observation:**
- Sort theo (x, y) rồi build lower + upper — $O(n \log n)$, ngắn, ổn định (so với Graham scan).
- **Hull gồm tất cả điểm** (kể cả thẳng hàng) → dùng `<= 0` trong check (bỏ collinear); **hull strictly convex** (bỏ collinear) → `< 0`.
- Hull cần `>= 3` điểm để tính diện tích/đường chéo; input có thể tất cả thẳng hàng → check `sz(hull) < 3`.

**Điều kiện sử dụng:**
- `T` là `ll` hoặc `double`; điểm trùng → `unique` trước. Hull trả về **ngược kim đồng hồ** (ccw) với `cross <= 0`.

**Độ phức tạp:**
- Time: $O(n \log n)$
- Space: $O(n)$

**Dependency:** template.cpp, point

```cpp
// id: hull
// hull ccw, loại điểm collinear (đổi <=0 để giữ) — trả {} nếu < 3 điểm distinct
vector<Pl> hull(vector<Pl> pts) {
    sort(all(pts));
    pts.erase(unique(all(pts)), pts.end());
    int n = sz(pts);
    if (n < 3) return {};
    vector<Pl> h(2 * n);
    int k = 0;
    rep(i, 0, n) {  // lower
        while (k >= 2 && h[k - 2].cross(h[k - 1], pts[i]) <= 0) --k;
        h[k++] = pts[i];
    }
    int lo = k + 1;
    for (int i = n - 2; i >= 0; --i) {  // upper
        while (k >= lo && h[k - 2].cross(h[k - 1], pts[i]) <= 0) --k;
        h[k++] = pts[i];
    }
    h.resize(k - 1);  // điểm cuối = điểm đầu
    return h;
}
```

## Hull diameter (rotating calipers)

**Mục đích:** Cặp điểm RA XA FUT nhất trên lớp bao lồi (đường kính), chu vi hull, cặp xa nhất.

**Ý tưởng / Observation:**
- Với hull ccw, `distance(p[i], p[j])` đơn điệu → con trỏ j quay 1 vòng $O(n)$.
- Bài "tìm cặp điểm xa nhất của tập điểm" → hull trước rồi rotating calipers — $O(n \log n)$.

**Điều kiện sử dụng:**
- Hull phải là `vector<Pl>` ccw từ mục `hull` (không có điểm trùng). Hull < 3 điểm → tính thủ công.

**Độ phức tạp:**
- Time: $O(n \log n)$ (gồm build hull), $O(n)$ nếu hull có sẵn
- Space: $O(n)$

**Dependency:** template.cpp, point, hull

```cpp
// id: hulldiameter
pair<int, int> hullDiameter(const vector<Pl>& S) {  // trả cặp chỉ số (i, j) xa nhất
    if (sz(S) == 1) return {0, 0};
    if (sz(S) == 2) return {0, 1};
    pair<ll, pii> best(0, {0, 0});
    int n = sz(S), j = 1;
    rep(i, 0, n) {
        // tìm j xa nhất từ cạnh (i, i+1)
        while (true) {
            auto cross = (S[(i + 1) % n] - S[i]).cross(S[j] - S[i]);
            auto cross2 = (S[(i + 1) % n] - S[i]).cross(S[(j + 1) % n] - S[i]);
            if (cross2 > cross) j = (j + 1) % n;
            else break;
        }
        ll d = (S[j] - S[i]).dist2();
        if (d > best.first) best = {d, {i, j}};
    }
    return best.second;
}
```

## Diện tích đa giác

**Mục đích:** Diện tích (signed) đa giác đơn giản, trọng tâm — shoelace formula.

**Ý tưởng / Observation:**
- $\frac{\sum \operatorname{cross}(P[i], P[i+1])}{2}$ → signed (ccw dương, cw âm). `abs()` để lấy diện tích.
- Point-in-polygon: cross sign với mọi cạnh (đa giác lồi) hoặc ray casting (bất kỳ) — xem mục inPolygon.

**Điều kiện sử dụng:**
- Đa giác đơn giản (không tự cắt); đỉnh theo thứ tự (ccw hoặc cw).

**Độ phức tạp:**
- Time: $O(n)$
- Space: $O(1)$

**Dependency:** template.cpp, point

```cpp
// id: polyarea
D polygonArea(const vector<Pd>& v) {  // diện tích tuyệt đối, n ≥ 3
    D a = 0;
    rep(i, 0, sz(v)) a += v[i].cross(v[(i + 1) % sz(v)]);
    return fabs(a) / 2;
}
// signed: a/2 (dương = ccw). Trong tâm (đa giác đơn giản):
// Cx = Σ (x_i + x_{i+1}) * cross_i / (6 * A), tương tự Cy.
```

## Point in polygon

**Mục đích:** Kiểm tra điểm có nằm TRONG đa giác (ray casting) hoặc đa giác LỚI (cross test) — $O(n)$/$O(\log n)$.

**Ý tưởng / Observation:**
- **Bất kỳ:** bắn tia ngang, đếm cắt đoạn → lẻ = trong (xử lý điểm trên biên trả 0/1 tùy bài).
- **Lồi:** binary search — kiểm tra 1 tam giác gốc + 2 half-plane ($O(\log n)$) — dùng khi n lớn, query nhiều.
- Bài "điểm trong/ra" + update → $O(n)$ mỗi query hoặc sweep line.

**Điều kiện sử dụng:**
- Ray casting: điểm TRÊN biên → undefined (check riêng bằng `onSegment` nếu cần chắc chắn).

**Độ phức tạp:**
- Time: $O(n)$ ray casting; $O(\log n)$ nếu lồi
- Space: $O(1)$

**Dependency:** template.cpp, point, onsegment

```cpp
// id: inpolygon
int inpolygon(const vector<Pd>& v, Pd p) {  // 1 = trong, 0 = ngoài, 2 = trên biên
    rep(i, 0, sz(v)) {
        Pd a = v[i], b = v[(i + 1) % sz(v)];
        if (onSegment(a, b, p)) return 2;
    }
    bool in = false;
    rep(i, 0, sz(v)) {
        Pd a = v[i], b = v[(i + 1) % sz(v)];
        if ((a.y > p.y) != (b.y > p.y)) {  // tia ngang cắt đoạn
            D x = a.x + (p.y - a.y) / (b.y - a.y) * (b.x - a.x);
            if (x > p.x) in = !in;
        }
    }
    return in;
}
// LÕI (convex hull ccw): kiểm tra cross > 0 với mọi cạnh (O(n)), hoặc binary:
bool inHull(const vector<Pd>& h, Pd p) {  // h ccw từ hull()
    int n = sz(h);
    if (n < 3) return false;
    if (h[0].cross(h[1], p) < 0) return false;   // ngoài cạnh đầu
    if (h[0].cross(h.back(), p) > 0) return false;  // ngoài cạnh cuối
    int lo = 1, hi = n - 1;  // binary tìm tam giác chứa
    while (hi - lo > 1) {
        int mid = (lo + hi) / 2;
        h[0].cross(h[mid], p) >= 0 ? lo = mid : hi = mid;
    }
    return h[lo].cross(h[(lo + 1) % n], p) >= 0;
}
```

## Closest pair (sweep line)

**Mục đích:** Cặp điểm gần nhất trong tập n điểm — $O(n \log n)$ thay vì $O(n^2)$.

**Ý tưởng / Observation:**
- Sort theo x, giữ dãy đã xét trong `set` theo y; với mỗi điểm mới chỉ check $\le 6$ điểm trong ô $d \times 2d$.
- Nhận ra: "$n \le 10^5$, tìm cặp distance nhỏ nhất" → sweep; "cặp > 0" → union-find với khoảng cách tăng dần.

**Điều kiện sử dụng:**
- `D = double`; tọa độ distinct (nếu trùng → distance 0 ngay). Set theo `(y, x)` pair.

**Độ phức tạp:**
- Time: $O(n \log n)$
- Space: $O(n)$

**Dependency:** template.cpp, point

```cpp
// id: closestpair
pair<int, int> closestPair(const vector<Pd>& v) {  // trả (i, j) xa nhất? → GẦN nhất
    int n = sz(v);
    vector<int> ord(n);
    iota(all(ord), 0);
    sort(all(ord), [&](int i, int j) { return v[i].x < v[j].x; });
    set<pair<D, int>> active;  // (y, index)
    D best = 1e30;
    int bi = 0, bj = 0;
    int j = 0;
    rep(ii, 0, n) {
        int i = ord[ii];
        while (v[ord[j]].x < v[i].x - best) active.erase({v[ord[j]].y, ord[j]}), ++j;
        auto lo = active.lower_bound({v[i].y - best, -1});
        auto hi = active.upper_bound({v[i].y + best, 1 << 30});
        for (auto it = lo; it != hi; ++it) {
            D d = (v[i] - v[it->second]).dist();
            if (d < best) best = d, bi = i, bj = it->second;
        }
        active.insert({v[i].y, i});
    }
    return {bi, bj};
}
// O(n²) đơn giản (n ≤ 5000): double loop min (v[i]-v[j]).dist2().
```

## Hình tròn

**Mục đích:** Giao 2 hình tròn (`circleInter`), đường tròn ngoại tiếp tam giác (`circumCircle`), đường tròn nội tiếp — dựng hình tròn từ điều kiện.

**Ý tưởng / Observation:**
- `circumCircle`: giải hệ 2 phương trình tọa độ (tốt hơn đặt bằng tay) — bản code bên dưới dùng trực tiếp định thức.
- `circleInter`: 2 tâm, khoảng cách `d`; $d > r_1 + r_2$ → không giao; $d < |r_1 - r_2|$ → chứa; $d = 0$ → trùng.
- Bài "tìm hình tròn qua 3 điểm" / "bao 3 điểm" → circumCircle; "bao tất cả điểm" → Welzl / min enclosing circle.

**Điều kiện sử dụng:**
- `D = double` với epsilon; tam giác suy biến (thẳng hàng) → không có ngoại tiếp (check cross ≈ 0).

**Độ phức tạp:**
- Time: $O(1)$
- Space: $O(1)$

**Dependency:** template.cpp, point

```cpp
// id: circle
// đường tròn ngoại tiếp qua 3 điểm — nullopt nếu thẳng hàng
optional<tuple<Pd, D>> circumCircle(Pd a, Pd b, Pd c) {
    D d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
    if (fabs(d) < 1e-12) return nullopt;
    D ux = ((a.dist2()) * (b.y - c.y) + (b.dist2()) * (c.y - a.y) + (c.dist2()) * (a.y - b.y)) / d;
    D uy = ((a.dist2()) * (c.x - b.x) + (b.dist2()) * (a.x - c.x) + (c.dist2()) * (b.x - a.x)) / d;
    Pd o(ux, uy);
    return make_tuple(o, (o - a).dist());
}
// giao 2 hình tròn (c1, r1), (c2, r2): trả danh sách điểm giao (0, 1, hoặc 2)
vector<Pd> circleInter(Pd c1, D r1, Pd c2, D r2) {
    Pd d = c2 - c1;
    D dist = d.dist();
    if (dist > r1 + r2 + 1e-9 || dist < fabs(r1 - r2) - 1e-9) return {};  // không giao
    if (dist < 1e-9) return {};  // trùng tâm → vô số
    D a = (r1 * r1 - r2 * r2 + dist * dist) / (2 * dist);
    D h2 = r1 * r1 - a * a;
    if (h2 < 0) h2 = 0;
    D h = sqrt(h2);
    Pd m = c1 + d * (a / dist);
    Pd off = d.perp().unit() * h;
    if (h < 1e-9) return {m};
    return {m + off, m - off};
}
```

## Giao đường thẳng với lớp bao lồi (lineHull)

**Mục đích:** Tìm đỉnh chiếu cực đại theo một phương (`extrVertex`) và giao đường thẳng vô hạn với convex hull — bài "cắt lồi bởi đường thẳng" $O(\log n)$.

**Ý tưởng / Observation:**
- `extrVertex`: projection lên phương dir dọc hull là unimodal — binary search trên dấu 2 cạnh kề, tie-break `ls/ms/cmp` xử lý đoạn bằng nhau (plateau) → $O(\log n)$.
- `lineHull`: tìm 2 đỉnh cực đại 2 bên đường (`endA`, `endB`) → 2 binary search trên nửa chuỗi tìm cạnh cắt mỗi bên.
- Nhận ra: nhiều đường cắt hull lặp lại → không quét $O(n)$ mỗi đường; kèm `extrVertex` cho rotating calipers/tangent.

**Điều kiện sử dụng:**
- Hull ccw **không có 3 điểm thẳng hàng** (mục `hull` mặc định đã loại collinear).
- So sánh KHÔNG epsilon (giống KACTL) → tọa độ nguyên (long double chứa chính xác $\le 2^{63}$); số thực mượt cẩn thận với trị ~0.

**Độ phức tạp:**
- Time: $O(\log n)$ mỗi lời gọi
- Space: $O(1)$

**Dependency:** template.cpp, point

```cpp
// id: linehull
// KACTL LineHullIntersection — hull ccw, không 3 điểm thẳng hàng
int sgnD(D x) { return (x > 0) - (x < 0); }
// đỉnh có CHIẾU (projection) lớn nhất theo phương dir — O(log n)
int extrVertex(const vector<Pd>& h, Pd dir) {
    int n = sz(h);
    auto cmp = [&](int i, int j) {  // sgn(dot(dir, h[j] - h[i]))
        return sgnD(dir.dot(h[j % n] - h[i % n]));
    };
    auto extr = [&](int i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1 + n) < 0; };
    int lo = 0, hi = n;
    if (extr(0)) return 0;
    while (lo + 1 < hi) {
        int m = (lo + hi) / 2;
        if (extr(m)) return m;
        int ls = cmp(lo + 1, lo), ms = cmp(m + 1, m);
        (ls < ms || (ls == ms && ls == cmp(lo, m)) ? hi : lo) = m;
    }
    return lo;
}
// giao đường thẳng vô hạn qua (a,b) với hull — trả 2 cạnh theo thứ tự đường đi:
// {-1,-1} không giao; {i,-1} chỉ chạm đỉnh i; {i,i} trùng cạnh (i,i+1);
// {i,j} cắt cạnh (i,i+1) và (j,j+1)
array<int, 2> lineHull(Pd a, Pd b, const vector<Pd>& poly) {
    int n = sz(poly);
    int endA = extrVertex(poly, (a - b).perp());
    int endB = extrVertex(poly, (b - a).perp());
    auto cmpL = [&](int i) { return sgnD((poly[i] - a).cross(b - a)); };  // 2 bên đường
    if (cmpL(endA) < 0 || cmpL(endB) > 0) return {-1, -1};
    array<int, 2> res;
    rep(i, 0, 2) {
        int lo = endB, hi = endA;
        while ((lo + 1) % n != hi) {
            int m = ((lo + hi + (lo < hi ? 0 : n)) / 2) % n;
            (cmpL(m) == cmpL(endB) ? lo : hi) = m;
        }
        res[i] = (lo + !cmpL(hi)) % n;
        swap(endA, endB);
    }
    if (res[0] == res[1]) return {res[0], -1};
    if (!cmpL(res[0]) && !cmpL(res[1]))
        switch ((res[0] - res[1] + sz(poly) + 1) % sz(poly)) {
            case 0: return {res[0], res[0]};
            case 2: return {res[1], res[1]};
        }
    return res;
}
```

## KD-tree (cặp gần nhất / range search)

**Mục đích:** Range search (đếm điểm trong rectangle/trên đường tròn) và nearest neighbor trên 2D — truy vấn nhanh hơn quét hết khi n lớn, ít query.

**Ý tưởng / Observation:**
- Chia đôi theo dimension `d = depth % 2`, median → cân bằng; `calcBnd` tính bbox subtree.
- Range search: prune nếu bbox không giao vùng truy vấn (`mayOverlap`); NN: prune nếu khoảng cách đến bbox ≥ best.
- Nhận ra: "n = 10^5, Q = 10^5 truy vấn range trên plane" → KD (hoặc grid/sweep nếu rectangle-only).

**Điều kiện sử dụng:**
- Point array + node index (không pointer) — build 1 lần, query nhiều. Nên shuffle input trước khi build (tránh worst-case).

**Độ phức tạp:**
- Time: $O(\log n)$ kỳ vọng mỗi query (thực tế ~$\sqrt{n}$), tệ nhất $O(n)$
- Space: $O(n)$

**Dependency:** template.cpp, point

```cpp
// id: kdtree
struct KDTree {
    struct Node {
        int left = 0, right = 0, axis = 0, id = -1;  // id = index gốc trong pts
        Pd p;
        Pd bmin, bmax;
    };
    vector<Node> t;
    const vector<Pd>& pts;
    vector<int> ids;
    int root = 0;
    KDTree(const vector<Pd>& v) : pts(v) {
        ids.resize(sz(v));
        iota(all(ids), 0);
        t.reserve(sz(v) + 1);
        t.push_back({});  // node 0 = sentinel rỗng
        root = build(0, sz(v), 0);
    }
    int build(int l, int r, int depth) {  // [l, r) trên ids, chia theo axis = depth % 2
        if (l >= r) return 0;
        int axis = depth % 2;
        int mid = (l + r) / 2;
        nth_element(ids.begin() + l, ids.begin() + mid, ids.begin() + r, [&](int i, int j) {
            return axis == 0 ? pts[i].x < pts[j].x : pts[i].y < pts[j].y;
        });
        int id = sz(t);
        t.push_back({});
        t[id].p = pts[ids[mid]];
        t[id].id = ids[mid];
        t[id].axis = axis;
        int L = build(l, mid, depth + 1), R = build(mid + 1, r, depth + 1);
        t[id].left = L, t[id].right = R;
        t[id].bmin = t[id].bmax = t[id].p;
        for (int c : {L, R})
            if (c) {
                t[id].bmin.x = min(t[id].bmin.x, t[c].bmin.x);
                t[id].bmin.y = min(t[id].bmin.y, t[c].bmin.y);
                t[id].bmax.x = max(t[id].bmax.x, t[c].bmax.x);
                t[id].bmax.y = max(t[id].bmax.y, t[c].bmax.y);
            }
        return id;
    }
    // đếm số điểm trong rectangle [lo.x, hi.x] × [lo.y, hi.y]
    int count(int id, Pd lo, Pd hi) {
        if (!id) return 0;
        Node& n = t[id];
        if (n.bmax.x < lo.x || n.bmin.x > hi.x || n.bmax.y < lo.y || n.bmin.y > hi.y) return 0;
        bool inside = lo.x <= n.p.x && n.p.x <= hi.x && lo.y <= n.p.y && n.p.y <= hi.y;
        return (inside ? 1 : 0) + count(n.left, lo, hi) + count(n.right, lo, hi);
    }
    // nearest neighbor: trả index gốc của điểm gần q nhất (dist2 lưu ngoài)
    ll bestD2 = LLONG_MAX;
    int bestId = -1;
    void nn(int id, Pd q) {
        if (!id) return;
        Node& n = t[id];
        D dx = (q.x < n.bmin.x ? n.bmin.x - q.x : (q.x > n.bmax.x ? q.x - n.bmax.x : 0));
        D dy = (q.y < n.bmin.y ? n.bmin.y - q.y : (q.y > n.bmax.y ? q.y - n.bmax.y : 0));
        if (dx * dx + dy * dy >= (D)bestD2) return;  // prune
        ll d2 = (ll)round((n.p - q).dist2());
        if (d2 < bestD2) bestD2 = d2, bestId = n.id;
        int near = (n.axis == 0 ? (q.x < n.p.x) : (q.y < n.p.y)) ? n.left : n.right;
        int far = near == n.left ? n.right : n.left;
        nn(near, q);
        nn(far, q);
    }
    int nearest(Pd q) {  // index gốc điểm gần q nhất (-1 nếu rỗng)
        bestD2 = LLONG_MAX, bestId = -1;
        nn(root, q);
        return bestId;
    }
};
// usage: KDTree kd(pts); kd.count(kd.root, lo, hi); int i = kd.nearest(q);
// shuffle pts trước build để tránh worst-case (input đã sort sẵn theo 1 trục).
```

## Công thức hình tam giác

**Mục đích:** Tra cứu nhanh khi tính diện tích, góc, bán kính đường tròn nội/ngoại tiếp.

**Ý tưởng / Observation:**
- Có 3 cạnh → Heron: $s = \frac{a+b+c}{2}$, $A = \sqrt{s(s-a)(s-b)(s-c)}$ — kiểm tra bất đẳng thức tam giác trước.
- Có 2 cạnh + góc xen giữa → $A = \frac{ab\sin(C)}{2}$; có góc thì dùng law of sines/cosines.
- $r = \frac{A}{s}$ (nội tiếp), $R = \frac{abc}{4A}$ (ngoại tiếp) — dùng khi bài liên quan đường tròn.

**Điều kiện sử dụng:**
- Tọa độ `double`; tam giác suy biến (thẳng hàng) → check $A \approx 0$.

**Độ phức tạp:**
- Time: $O(1)$
- Space: $O(1)$

**Code:** — (công thức tra cứu)

$$\text{Heron:} \quad s = \frac{a+b+c}{2}, \qquad A = \sqrt{s(s-a)(s-b)(s-c)}$$
$$A = \frac{ab\sin(C)}{2} \qquad \text{(2 cạnh + góc xen giữa)}$$
$$A = \frac{c^2\sin(A)\sin(B)}{2\sin(C)} \qquad \text{(2 góc + cạnh đối)}$$
$$\text{Law of cosines:} \quad c^2 = a^2 + b^2 - 2ab\cos(C)$$
$$\text{Law of sines:} \quad \frac{a}{\sin(A)} = \frac{b}{\sin(B)} = \frac{c}{\sin(C)} = 2R$$
$$r = \frac{A}{s} \qquad \text{(bán kính đường tròn nội tiếp)}$$
$$R = \frac{abc}{4A} \qquad \text{(bán kính đường tròn ngoại tiếp)}$$
$$h_a = \frac{2A}{a} \qquad \text{(độ cao theo cạnh a)}$$
$$\text{tọa độ trọng tâm:} \quad \left(\frac{x_1+x_2+x_3}{3},\ \frac{y_1+y_2+y_3}{3}\right)$$
