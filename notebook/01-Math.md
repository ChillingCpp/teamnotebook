# 01 — Toán (Math)

## Phương trình & hệ phương trình

**Mục đích:** Nghiệm phương trình bậc 2, hệ 2 ẩn, generalized Cramer cho hệ tuyến tính.

**Ý tưởng / Observation:**
- Parabol $ax^2+bx+c$: đỉnh tại $x = -b/2a$ — check nhanh max/min của hàm bậc 2 (binary search trên nghiệm).
- Hệ 2 ẩn: chia thức con nếu $ad - bc = 0$ → song song hoặc trùng.

**Điều kiện sử dụng:**
- Dùng double khi cần nghiệm thực; $b^2-4ac$ tràn `ll` nếu hệ số ~ $10^9$ → tính bằng `long double` hoặc kiểm tra cận.

**Độ phức tạp:**
- Time: $O(1)$
- Space: $O(1)$

**Code:** — (công thức tra cứu)

$$ax^2 + bx + c = 0 \;\Rightarrow\; x = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}, \qquad \text{đỉnh: } x = -\frac{b}{2a}$$
$$ax + by = e,\ cx + dy = f \;\Rightarrow\; x = \frac{ed - bf}{ad - bc}, \quad y = \frac{af - ec}{ad - bc}$$
$$Ax = b \ (\text{Cramer}): \quad x_i = \frac{\det(A \text{ với cột } i \text{ thay bằng } b)}{\det(A)}$$

## Đệ quy tuyến tính

**Mục đích:** Đóng bài (closed form) cho dãy thỏa $a_n = c_1a_{n-1} + \dots + c_k a_{n-k}$, hoặc tính phần tử thứ n rất lớn.

**Ý tưởng / Observation:**
- Phương trình đặc trưng: $x^k = c_1x^{k-1} + \dots + c_k$. Nghiệm phân biệt $r_1..r_k$ → $a_n = \sum d_i r_i^n$; nghiệm bội $r$ lặp m lần → thêm hạng $p(n)r^n$ (đa thức bậc $m-1$).
- $d_i$ tìm từ điều kiện ban đầu (giải hệ k ẩn) — hoặc đoán dãy bằng Berlekamp–Massey (mục Numeric) rồi tính n lớn bằng linear recurrence.
- Quay vòng tuyến tính $a_n$ với $n \le 10^{18}$ → dùng matrix power (mục Matrix), không lặp.

**Điều kiện sử dụng:**
- Đệ quy vô hạn gốc phải hội tụ mới có nghĩa (khi xấp xỉ số thực).

**Độ phức tạp:**
- Time: $O(k^3 \log n)$ nếu nhân ma trận, $O(k)$ nếu đã có công thức đóng.
- Space: $O(k)$

**Code:** — (công thức tra cứu)

$$a_n = c_1a_{n-1} + \dots + c_k a_{n-k}, \qquad \text{nghiệm } r_1..r_k \text{ phân biệt:}$$
$$a_n = d_1r_1^n + \dots + d_kr_k^n \qquad (d_i \text{ từ hệ điều kiện ban đầu})$$
$$\text{nghiệm bội } r \ (m \text{ lần}): \quad \text{thêm hạng } (d_1n + d_2)\, n^{m-2} \cdots r^n$$

## Lượng giác

**Mục đích:** Rút gọn biểu thức, xoay/tính góc, tối ưu dạng $a\cos x + b\sin x$.

**Ý tưởng / Observation:**
- $a\cos x + b\sin x = r\cos(x - \varphi)$ với $r = \sqrt{a^2+b^2}$, $\varphi = \operatorname{atan2}(b, a)$ → giá trị max của biểu thức là $r$ (dùng để "nén" 2 tham số thành 1).
- Sum-to-product khi cần biến đổi tổng trig về tích (hoặc ngược lại) để so sánh đơn điệu.

**Điều kiện sử dụng:**
- `atan2(y, x)` trả về $(-\pi, \pi]$; so sánh góc nhớ xử lý cắt quanh $0$ / $2\pi$.

**Độ phức tạp:**
- Time: $O(1)$

**Code:** — (công thức tra cứu)

$$\sin(v+w) = \sin v \cos w + \cos v \sin w, \qquad \cos(v+w) = \cos v \cos w - \sin v \sin w$$
$$\tan(v+w) = \frac{\tan v + \tan w}{1 - \tan v \tan w}$$
$$\sin v + \sin w = 2\sin\tfrac{v+w}{2}\cos\tfrac{v-w}{2}, \qquad \cos v + \cos w = 2\cos\tfrac{v+w}{2}\cos\tfrac{v-w}{2}$$
$$a\cos x + b\sin x = r\cos(x - \varphi), \quad r = \sqrt{a^2+b^2}, \quad \varphi = \operatorname{atan2}(b, a)$$

xoay điểm $p$ quay gốc, góc $\theta$ (độ): $\left(x\cos\theta - y\sin\theta,\ x\sin\theta + y\cos\theta\right)$

## Tổng, chuỗi & Taylor

**Mục đích:** Tính tổng đóng bài, khai triển xấp xỉ khi cần số thực, ước lượng cấp độ.

**Ý tưởng / Observation:**
- Tổng cấp số nhân $1 + c + \dots + c^b = \frac{c^{b+1} - c^a}{c - 1}$ — cẩn thận $c = 1$.
- $\sqrt{1+x}$ khai triển quanh $x = 0$ hội tụ cho $|x| \le 1$ → xấp xỉ nhanh khi nghiệm nằm sát 1.
- Khi cần đếm chính xác tới $10^{18}$: đừng cộng dồn, dùng công thức đóng bài (xem bảng).

**Điều kiện sử dụng:**
- Taylor chỉ xấp xỉ; kiểm tra dãy hội tụ (điều kiện hội tụ ghi kèm).

**Độ phức tạp:**
- Time: $O(1)$ cho tổng đóng bài; $O(k)$ cho khai triển bậc k.

**Code:** — (công thức tra cứu)

$$\sum_{i=a}^{b} c^i = \frac{c^{b+1} - c^a}{c - 1} \qquad (c \ne 1)$$
$$1 + 2 + \dots + n = \frac{n(n+1)}{2}$$
$$1^2 + \dots + n^2 = \frac{n(n+1)(2n+1)}{6}, \qquad 1^3 + \dots + n^3 = \frac{n^2(n+1)^2}{4}$$
$$1^4 + \dots + n^4 = \frac{n(n+1)(2n+1)(3n^2+3n-1)}{30}$$
$$e^x = 1 + x + \frac{x^2}{2!} + \dots \qquad (-\infty < x < \infty)$$
$$\ln(1+x) = x - \frac{x^2}{2} + \frac{x^3}{3} - \dots \qquad (-1 < x \le 1)$$
$$\sqrt{1+x} = 1 + \frac{x}{2} - \frac{x^2}{8} + \dots \qquad (|x| \le 1)$$
$$\sin x = x - \frac{x^3}{3!} + \frac{x^5}{5!} - \dots, \qquad \cos x = 1 - \frac{x^2}{2!} + \frac{x^4}{4!} - \dots$$

## Xác suất & kỳ vọng

**Mục đích:** Tính kỳ vọng/phương sai, biến đổi đếm → xác suất (indicator), các phân bố chuẩn.

**Ý tưởng / Observation:**
- **Linearity of expectation:** $E(aX+bY) = aE(X)+bE(Y)$ — KHÔNG cần độc lập. Đây là đòn bẩy chính: biến chỉ báo $I[\text{event}]$ → $E[\text{số event}] = \sum P(\text{event})$.
- $Var(X) = E(X^2) - E(X)^2$; độc lập → $Var(aX+bY) = a^2Var(X)+b^2Var(Y)$.
- Kỳ vọng của min/max: $E[\max] = \sum_{k \ge 1} P(\max \ge k)$, $E[X] = \sum_{k \ge 1} P(X \ge k)$ (X không âm) — biến bài đếm thành tổng xác suất.
- Bài "đếm cặp thỏa mãn" → chia cho tổng, hoặc linearity trên từng cặp.

**Điều kiện sử dụng:**
- Công thức $E[X] = \sum k \cdot P(X=k)$ với X rời rạc; liên tục thay Sum → Integral.

**Độ phức tạp:**
- Time: $O(\text{số hạng tính tay})$ — thường $O(1)$ / $O(n)$.

**Code:** — (công thức tra cứu)

$$E(X) = \sum x \cdot P(X=x), \qquad Var(X) = E(X^2) - E(X)^2, \qquad sd = \sqrt{Var}$$
$$E(aX + bY) = aE(X) + bE(Y) \qquad (\text{kể cả } X, Y \text{ phụ thuộc})$$
$$X, Y \text{ độc lập: } \quad Var(aX + bY) = a^2Var(X) + b^2Var(Y)$$
$$\text{Bin}(n,p): \ E = np,\ Var = np(1-p) \qquad \text{Poisson}(\lambda): \ E = Var = \lambda$$
$$\text{thứ nhất thành công } p(k) = p(1-p)^{k-1}: \quad E = \frac{1}{p}, \ Var = \frac{1-p}{p^2}$$
$$U(a,b): \ E = \frac{a+b}{2}, \ Var = \frac{(b-a)^2}{12} \qquad \text{Exp}(\lambda): \ E = \frac{1}{\lambda}, \ Var = \frac{1}{\lambda^2}$$
$$X \text{ không âm nguyên}: \quad E(X) = \sum_{k \ge 1} P(X \ge k)$$

## Chuỗi Markov

**Mục đích:** Ma trận chuyển trạng thái, phân bố cân bằng, xác suất hấp thụ / thời gian hấp thụ.

**Ý tưởng / Observation:**
- $p^{(n)} = P^n p^{(0)}$ — nhân ma trận nhanh (mục Matrix) thay vì lặp.
- Phân bố cân bằng $\pi = \pi P$; nghiệm hệ $(I - P^T)\pi = 0$ + $\sum \pi = 1$.
- Đồ thị vô hướng không nhị phân, bước nhảy đều → $\pi_i \propto \deg(i)$ (dùng ngay không cần giải).
- Chuỗi hấp thụ (A absorbing): giải hệ tuyến tính nhỏ $a_{ij} = p_{ij} + \sum_{k \in G} a_{ik}p_{kj}$, thời gian kỳ vọng $t_i = 1 + \sum_k p_{ik}t_k$.

**Điều kiện sử dụng:**
- Hấp thụ/ergodic để đảm bảo giới hạn tồn tại: vô cùng liên thông + chu kỳ = 1 (aperiodic).

**Độ phức tạp:**
- Time: $O(S^3 \log n)$ cho $P^n$ (S = số trạng thái).

**Code:** — (công thức tra cứu)

$$p^{(n)} = P^n p^{(0)}, \qquad \pi = \pi P \;\Rightarrow\; (I - P^T)\pi = 0, \quad \sum_i \pi_i = 1$$
$$\text{hấp thụ: } \quad a_{ij} = p_{ij} + \sum_{k \in G} a_{ik}p_{kj} \qquad (\text{hệ tuyến tính})$$
$$\text{thời gian hấp thụ: } \quad t_i = 1 + \sum_{k \in G} p_{ik}t_k$$

## Ma trận

**Mục đích:** Nhân ma trận, lũy thừa $A^n$, nhân ma trận × vector — dùng cho DP tuyến tính n lớn, quay vòng, graph power.

**Ý tưởng / Observation:**
- $A^n$ bằng lũy thừa bình phương: $O(N^3 \log n)$ thay vì $O(N^3 n)$.
- Đệ quy tuyến tính bậc k → đặt ma trận k×k chuyển trạng thái $[a_n, a_{n-1}, \dots]$.
- `vector<T> operator*` để áp transition vào state vector một bước.

**Điều kiện sử dụng:**
- $N \lesssim 100$ với $\log n \le 60$ thì ~ $10^7$ phép nhân (nửa giây). `T` = ll → cẩn thận tràn khi tổng hạng mục ~ $10^{18}$.

**Độ phức tạp:**
- Time: $O(N^3 \log p)$ cho mũ, $O(N^3)$ cho nhân.
- Space: $O(N^2)$

**Dependency:** template.cpp

```cpp
// id: matrix
template <class T, int N>
struct Matrix {
    using M = Matrix;
    array<array<T, N>, N> d{};
    M operator*(const M& m) const {
        M a;
        rep(i, 0, N) rep(j, 0, N) rep(k, 0, N) a.d[i][j] += d[i][k] * m.d[k][j];
        return a;
    }
    vector<T> operator*(const vector<T>& v) const {
        vector<T> r(N);
        rep(i, 0, N) rep(j, 0, N) r[i] += d[i][j] * v[j];
        return r;
    }
    M operator^(ll p) const {
        assert(p >= 0);
        M a, b(*this);
        rep(i, 0, N) a.d[i][i] = 1;
        for (; p; p >>= 1) {
            if (p & 1) a = a * b;
            b = b * b;
        }
        return a;
    }
};
// dùng: Matrix<ll,2> A; A.d = {{{{1,1}},{{1,0}}}}; auto f = (A^(n-1)) * vector<ll>{1,0};
```

## Determinant (số thực)

**Mục đích:** Định thức ma trận vuông (Gaussian + pivoting). Hủy dữ liệu input.

**Ý tưởng / Observation:**
- Chọn pivot theo tuyệt đối lớn nhất (partial pivoting) để tránh chia gần 0.
- `det == 0` → ma trận suy biến (không có nghiệm duy nhất / không lồi …).

**Điều kiện sử dụng:**
- `double` → sai số; với nghiệm chính xác tuyệt đối dùng bản mod (mục bên dưới) hoặc số nguyên.

**Độ phức tạp:**
- Time: $O(N^3)$
- Space: $O(N^2)$ (in-place trên input)

**Dependency:** template.cpp

```cpp
// id: determinant
double det(vector<vector<double>> a) {
    int n = sz(a);
    double res = 1;
    rep(i, 0, n) {
        int b = i;
        rep(j, i + 1, n) if (fabs(a[j][i]) > fabs(a[b][i])) b = j;
        if (i != b) swap(a[i], a[b]), res *= -1;
        res *= a[i][i];
        if (res == 0) return 0;
        rep(j, i + 1, n) {
            double v = a[j][i] / a[i][i];
            if (v != 0) rep(k, i + 1, n) a[j][k] -= v * a[i][k];
        }
    }
    return res;
}
```

## Determinant (modulo)

**Mục đích:** Định thức trên trường hữu hạn / số nguyên — dùng cho Matrix-Tree theorem (đếm cây khung), kiểm tra suy biến chính xác.

**Ý tưởng / Observation:**
- Bước gcd nguyên thuần (không cần nghịch đảo) → hoạt động với mọi mod (kể cả không nguyên tố) và cả không mod.
- Đổi hàng → đổi dấu; dừng sớm khi `ans == 0`.

**Điều kiện sử dụng:**
- Kết quả trả về `[0, mod)`. Ma trận n × n với mọi phần tử `|a| < mod`.

**Độ phức tạp:**
- Time: $O(N^3)$
- Space: $O(N^2)$

**Dependency:** template.cpp

```cpp
// id: detmod
ll detmod(vector<vector<ll>> a, ll mod) {
    int n = sz(a);
    ll ans = 1;
    rep(i, 0, n) {
        rep(j, i + 1, n) {
            while (a[j][i] != 0) {  // bước gcd thay vì nghịch đảo
                ll t = a[i][i] / a[j][i];
                if (t) rep(k, i, n) a[i][k] = (a[i][k] - a[j][k] * t) % mod;
                swap(a[i], a[j]);
                ans *= -1;
            }
        }
        ans = ans * a[i][i] % mod;
        if (!ans) return 0;
    }
    return (ans % mod + mod) % mod;
}
```

## Giải hệ Ax = b (số thực)

**Mục đích:** Gaussian elimination — nghiệm/ rank/ vô nghiệm của hệ tuyến tính, nghịch đảo ma trận (gắn matrix bên phải).

**Ý tưởng / Observation:**
- Partial pivoting trên cả mảng `col` → theo dõi hoán vị cột, dựng nghiệm đúng vị trí.
- Trả về rank: `rank == m` → nghiệm duy nhất; `rank < m` → vô nghiệm (`-1`) hoặc vô nghiệm (chọn tự do).
- Cần toàn bộ nghiệm xác định → sửa bước eliminate (xóa cả cột trái) — xem bản "SolveLinear2" trong ghi chú.

**Điều kiện sử dụng:**
- `x` phải được allocate sẵn: `vd x(m);`. Sai số double: epsilon `1e-12`.

**Độ phức tạp:**
- Time: $O(n^2m)$
- Space: $O(nm)$

**Dependency:** template.cpp

```cpp
// id: solvelinear
typedef vector<double> vd;
const double EPS = 1e-12;
// trả về rank; x = nghiệm (m = sz(x)); -1 = vô nghiệm; A,b bị phá
int solveLinear(vector<vd>& A, vd& b, vd& x) {
    int n = sz(A), m = sz(x), rank = 0, br, bc;
    if (n) assert(sz(A[0]) == m);
    vi col(m);
    iota(all(col), 0);
    rep(i, 0, n) {
        double v, bv = 0;
        rep(r, i, n) rep(c, i, m) if ((v = fabs(A[r][c])) > bv) br = r, bc = c, bv = v;
        if (bv <= EPS) {
            rep(j, i, n) if (fabs(b[j]) > EPS) return -1;
            break;
        }
        swap(A[i], A[br]);
        swap(b[i], b[br]);
        swap(col[i], col[bc]);
        rep(j, 0, n) swap(A[j][i], A[j][bc]);
        bv = 1 / A[i][i];
        rep(j, i + 1, n) {
            double fac = A[j][i] * bv;
            b[j] -= fac * b[i];
            rep(k, i + 1, m) A[j][k] -= fac * A[i][k];
        }
        rank++;
    }
    x.assign(m, 0);
    for (int i = rank; i--;) {
        b[i] /= A[i][i];
        x[col[i]] = b[i];
        rep(j, 0, i) b[j] -= A[j][i] * b[i];
    }
    return rank;
}
```

## Giải hệ trên F₂ (bitset)

**Mục đích:** Gaussian trên trường 2 nhị phân — hệ phương trình XOR (mô phỏng lights-out, linear basis kiểm tra phụ thuộc, stitch chuỗi bit).

**Ý tưởng / Observation:**
- `A[j] ^= A[i]` thay cho trừ hàng → $O(n^2m/64)$ nhờ bitset.
- `x` tìm được là một nghiệm bất kỳ khi rank < m.

**Điều kiện sử dụng:**
- $m \le \texttt{sz}(x)$ và `x` là `bitset` (đổi hằng `1000` theo m).

**Độ phức tạp:**
- Time: $O(n^2m / 64)$
- Space: $O(nm/64)$

**Dependency:** template.cpp

```cpp
// id: solvebinary
typedef bitset<1000> bs;
// trả về rank; x = nghiệm; -1 = vô nghiệm; A,b bị phá. m = số ẩn
int solveLinear2(vector<bs>& A, vi& b, bs& x, int m) {
    int n = sz(A), rank = 0, br;
    assert(m <= sz(x));
    vi col(m);
    iota(all(col), 0);
    rep(i, 0, n) {
        for (br = i; br < n; ++br) if (A[br].any()) break;
        if (br == n) {
            rep(j, i, n) if (b[j]) return -1;
            break;
        }
        int bc = (int)A[br]._Find_next(i - 1);
        swap(A[i], A[br]);
        swap(b[i], b[br]);
        swap(col[i], col[bc]);
        rep(j, 0, n) if (A[j][i] != A[j][bc]) {
            A[j].flip(i);
            A[j].flip(bc);
        }
        rep(j, i + 1, n) if (A[j][i]) {
            b[j] ^= b[i];
            A[j] ^= A[i];
        }
        rank++;
    }
    x = bs();
    for (int i = rank; i--;) {
        if (!b[i]) continue;
        x[col[i]] = 1;
        rep(j, 0, i) b[j] ^= A[j][i];
    }
    return rank;
}
```
