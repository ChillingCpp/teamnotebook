# 01 — Toán (Math)

## Hệ thức Cramer

$$Ax = b \ (\text{Cramer}): \quad x_i = \frac{\det(A \text{ với cột } i \text{ thay bằng } b)}{\det(A)}$$

## Đệ quy tuyến tính

**Use case:**
- Tính $a_{n}$ với $n$ rất lớn khi $a_{n}$ là linear recurrence bậc k.

**Công thức:**
$a_{n}=c_1a_{n-1}+\cdots+c_ka_{n-k}$

- Nghiệm đặc trưng phân biệt $r_i$:
$
a_{n}=\sum_{i=1}^{k} d_i r_i^{n}
$

- Nghiệm $r$ bội $m$:
$
a_{n}=P_{m-1}(n)r^{n}
$

**Complexity:**
- Matrix exponentiation: `O(k³ log n)`
- Closed form: `O(k)`

## Xác suất & kỳ vọng

**Mục đích:** Tính kỳ vọng/phương sai, biến đổi đếm → xác suất (indicator), các phân bố chuẩn.


**Code:** — (công thức tra cứu)

$$
E(X) = \sum_x xP(X=x)
$$

$$
Var(X) = E(X^2) - E(X)^2,
\qquad
sd(X) = \sqrt{Var(X)}
$$

$$
E(aX+bY)=aE(X)+bE(Y)
$$

$$
X,Y\text{ độc lập}
\quad\Longrightarrow\quad
Var(aX+bY)=a^2Var(X)+b^2Var(Y)
$$

$$
X\sim Bin(n,p):
\qquad
E(X)=np,
\qquad
Var(X)=np(1-p)
$$

$$
X\sim Pois(\lambda):
\qquad
E(X)=Var(X)=\lambda
$$

$$
X\sim Geom(p),\quad P(X=k)=p(1-p)^{k-1}:
\qquad
E(X)=\frac1p,
\qquad
Var(X)=\frac{1-p}{p^2}
$$

$$
X\sim U(a,b):
\qquad
E(X)=\frac{a+b}{2},
\qquad
Var(X)=\frac{(b-a)^2}{12}
$$

$$
X\sim Exp(\lambda):
\qquad
E(X)=\frac1\lambda,
\qquad
Var(X)=\frac1{\lambda^2}
$$

$$
X\in\mathbb Z_{\ge0}:
\qquad
E(X)=\sum_{k\ge1}P(X\ge k)
$$

## Chuỗi Markov

**Mục đích:** Ma trận chuyển trạng thái, phân bố cân bằng, xác suất hấp thụ / thời gian hấp thụ.

**Điều kiện sử dụng:**
- Hấp thụ/ergodic để đảm bảo giới hạn tồn tại: vô cùng liên thông + chu kỳ = 1 (aperiodic).
- Đồ thị vô hướng không nhị phân, bước nhảy đều → $\pi_i \propto \deg(i)$ (dùng ngay không cần giải hệ).

**Độ phức tạp:**
- Time: $O(S^3 \log n)$ cho $P^n$ (S = số trạng thái).

**Code:** — (công thức tra cứu)

$$p^{(n)} = P^n p^{(0)}, \qquad \pi = \pi P \;\Rightarrow\; (I - P^T)\pi = 0, \quad \sum_i \pi_i = 1$$
$$\text{hấp thụ: } \quad a_{ij} = p_{ij} + \sum_{k \in G} a_{ik}p_{kj} \qquad (\text{hệ tuyến tính})$$
$$\text{thời gian hấp thụ: } \quad t_i = 1 + \sum_{k \in G} p_{ik}t_k$$

## Ma trận

**Mục đích:** Nhân ma trận, lũy thừa $A^n$, nhân ma trận × vector — dùng cho DP tuyến tính n lớn, quay vòng, graph power.

**Điều kiện sử dụng:**
- $N \lesssim 100$ với $\log n \le 60$ thì ~ $10^7$ phép nhân (nửa giây). `T` = ll → cẩn thận tràn khi tổng hạng mục ~ $10^{18}$.
- Đệ quy tuyến tính bậc k → đặt ma trận k×k chuyển trạng thái $[a_n, a_{n-1}, \dots]$.

**Độ phức tạp:**
- Time: $O(N^3 \log p)$ cho mũ, $O(N^3)$ cho nhân.
- Space: $O(N^2)$

**Dependency:** template.cpp

```cpp
// id: matrix
// A^n bằng lũy thừa bình phương: O(N^3 log p) thay vì O(N^3 n).
template <class T, int N>
struct Matrix {
    using M = Matrix;
    array<array<T, N>, N> d{};
    M operator*(const M& m) const {
        M a;
        rep(i, 0, N) rep(j, 0, N) rep(k, 0, N) a.d[i][j] += d[i][k] * m.d[k][j];
        return a;
    }
    // áp transition vào state vector một bước
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

**Điều kiện sử dụng:**
- `double` → sai số; với nghiệm chính xác tuyệt đối dùng bản mod (mục bên dưới) hoặc số nguyên.
- `det == 0` → ma trận suy biến (không có nghiệm duy nhất / không lồi).

**Độ phức tạp:**
- Time: $O(N^3)$
- Space: $O(N^2)$ (in-place trên input)

**Dependency:** template.cpp

```cpp
// id: determinant
// partial pivoting: chọn pivot |.| lớn nhất để tránh chia gần 0.
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

**Điều kiện sử dụng:**
- Kết quả trả về `[0, mod)`. Ma trận n × n với mọi phần tử `|a| < mod`.
- Hoạt động với mọi mod (kể cả không nguyên tố) nhờ bước gcd nguyên thuần (không cần nghịch đảo).

**Độ phức tạp:**
- Time: $O(N^3)$
- Space: $O(N^2)$

**Dependency:** template.cpp

```cpp
// id: detmod
// đổi hàng → đổi dấu; dừng sớm khi ans == 0.
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

**Điều kiện sử dụng:**
- `x` phải được allocate sẵn: `vd x(m);`. Sai số double: epsilon `1e-12`.
- Trả về rank: `rank == m` → nghiệm duy nhất; `rank < m` → vô nghiệm (`-1`) hoặc vô số nghiệm (chọn tự do).

**Độ phức tạp:**
- Time: $O(n^2m)$
- Space: $O(nm)$

**Dependency:** template.cpp

```cpp
// id: solvelinear
// partial pivoting theo dõi hoán vị cột (mảng col) → dựng nghiệm đúng vị trí.
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

**Điều kiện sử dụng:**
- $m \le \texttt{sz}(x)$ và `x` là `bitset` (đổi hằng `1000` theo m).
- `x` tìm được là một nghiệm bất kỳ khi rank < m.

**Độ phức tạp:**
- Time: $O(n^2m / 64)$
- Space: $O(nm/64)$

**Dependency:** template.cpp

```cpp
// id: solvebinary
// A[j] ^= A[i] thay cho trừ hàng → O(n^2 m / 64) nhờ bitset.
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
