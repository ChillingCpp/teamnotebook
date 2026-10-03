# 02 — Số học (Number theory)

## Modular arithmetic

**Mục đích:** Phép toán modulo: lũy thừa, nghịch đảo, cộng/trừ/nhân chia an toàn.

**Điều kiện sử dụng:**
- `mod` phải là hằng số toàn cục; với mod nguyên tố dùng `modpow`.
- Mod nguyên tố: nghịch đảo = $a^{p-2} \bmod p$ (Fermat). Mod không nguyên tố → Euclid mở rộng (mục euclid).
- Tràn khi `(a*b) % mod` với $a,b < mod \le 10^{18}$ → dùng `__int128` hoặc modmul mục riêng.

**Độ phức tạp:**
- Time: $O(\log e)$ cho lũy thừa, $O(\log n)$ cho nghịch đảo.
- Space: $O(1)$

**Dependency:** template.cpp

```cpp
// id: modpow
// Fermat: a^(p-1) ≡ 1 (mod p) → nghịch đảo = a^(p-2) mod p (p nguyên tố).
const ll MOD = 1000000007;
ll modpow(ll b, ll e, ll mod = MOD) {
    ll a = 1;
    for (; e; b = b * b % mod, e /= 2)
        if (e & 1) a = a * b % mod;
    return a;
}
ll modinv(ll a, ll mod = MOD) { return modpow(a, mod - 2, mod); }  // mod nguyên tố
// nghịch đảo precompute O(n) — mod nguyên tố, n < mod:
// vi inv(n); inv[1] = 1; rep(i,2,n) inv[i] = mod - mod / i * inv[mod % i] % MOD;
```

## Euclid mở rộng

**Mục đích:** Tìm `x, y` sao cho $ax + by = \gcd(a, b)$; từ đó giải Diophantine, nghịch đảo modulo tổng quát.

**Điều kiện sử dụng:**
- $a, b \ge 0$; kết quả `x, y` có thể âm → chuẩn hóa `(x % m + m) % m`.
- `gcd(a,b) == 1` → `x` là nghịch đảo của `a` mod `b`. Hệ $ax + by = c$ có nghiệm khi $\gcd(a,b) \mid c$; nghiệm tổng quát: `x += k·b/g`, `y -= k·a/g`.

**Độ phức tạp:**
- Time: $O(\log min(a,b))$
- Space: $O(\log)$ đệ quy (có thể viết iterative)

**Dependency:** template.cpp

```cpp
// id: euclid
ll euclid(ll a, ll b, ll& x, ll& y) {
    if (!b) return x = 1, y = 0, a;
    ll d = euclid(b, a % b, y, x);
    return y -= a / b * x, d;
}
// giải ax + by = c: g = euclid(a,b,x,y); if (c%g) vô nghiệm;
// x *= c/g, y *= c/g; nghiệm đặc biệt (x,y), tổng quát (x + k*b/g, y - k*a/g)
```

## CRT (Chinese Remainder)

**Mục đích:** Giải hệ đồng dư $x \equiv a \pmod{m}$, $x \equiv b \pmod{n}$ → $x \bmod \operatorname{lcm}(m,n)$.

**Điều kiện sử dụng:**
- `m*n < 2^62` (tránh tràn); `|a| < m`, `|b| < n` để kết quả nằm `[0, lcm)`.
- Tồn tại nghiệm khi `(a - b) % gcd(m,n) == 0`. Gộp dần nhiều đồng dư: mỗi bước kết quả mod `lcm` tích lũy.

**Độ phức tạp:**
- Time: $O(\log max(m,n))$
- Space: $O(1)$

**Dependency:** template.cpp, euclid

```cpp
// id: crt
ll crt(ll a, ll m, ll b, ll n) {
    if (n > m) swap(a, b), swap(m, n);
    ll x, y, g = euclid(m, n, x, y);
    assert((a - b) % g == 0);  // không có nghiệm
    x = (b - a) % n * x % n / g * m + a;
    return x < 0 ? x + m * n / g : x;
}
// nhiều hệ: gộp dần: a = crt(a, m, b_i, m_i), m = lcm(m, m_i)
```

## Sieve nguyên tố

**Mục đích:** Sinh toàn bộ nguyên tố < LIM, đánh dấu, phân tích thừa số bằng SPF.

**Điều kiện sử dụng:**
- Mảng `int[LIM]` ~ $4 \cdot LIM$ byte; SPF dùng $LIM \le 2 \cdot 10^7$.
- SPF (smallest prime factor) phân tích nhanh mọi số ≤ LIM — ưu tiên khi cần `phi`, `mu`, đếm ước. Phân tích 1 số lớn $\le 10^{18}$ → Pollard rho (mục factor).

**Độ phức tạp:**
- Time: $O(LIM \log \log LIM)$
- Space: $O(LIM)$

**Dependency:** template.cpp

```cpp
// id: sieve
// Eratosthenes bitset: đủ cho LIM ≤ 1e7 trong < 0.1s.
const int LIM = 1000000;
vi primes;
bitset<LIM + 1> isPrime;
void eratosthenes() {
    isPrime.set();
    isPrime[0] = isPrime[1] = 0;
    for (int i = 2; i * i <= LIM; ++i)
        if (isPrime[i])
            for (int j = i * i; j <= LIM; j += i) isPrime[j] = 0;
    rep(i, 2, LIM + 1) if (isPrime[i]) primes.push_back(i);
}
// spf — phân tích thừa số O(log n):
// vi spf(LIM+1); rep(i,2,LIM+1) if (!spf[i]) for (ll j=i;j<=LIM;j+=i) if(!spf[j]) spf[j]=i;
// void factor(int x) { while (x > 1) { int p = spf[x], c = 0; while (x % p == 0) x /= p, ++c; } }
```

## Sàng nguyên tố đoạn [L, R]

**Mục đích:** Liệt kê / kiểm tra nguyên tố trong đoạn $[L, R]$ khi $R$ quá lớn cho mảng full nhưng độ dài đoạn $R - L$ vẫn nhỏ.

**Điều kiện sử dụng:**
- $R - L + 1 \le \sim 2 \cdot 10^7$ (mảng bool đoạn); $0 \le L \le R$. Chú ý $0, 1$ không phải nguyên tố.
- Chỉ cần mảng $R - L + 1$ bool (không lưu tới $R$) → $R \le 10^{12}$ với đoạn ngắn. Kết hợp Miller–Rabin để chắc chắn.

**Độ phức tạp:**
- Time: $O\big((R - L) \log \log R + \sqrt{R} \log \log \sqrt{R}\big)$
- Space: $O(R - L + \sqrt{R})$

**Dependency:** template.cpp

```cpp
// id: segsieve
// Nguyên tố trong [L, R]: is[i] = true ↔ (L + i) là nguyên tố.
// Sàng nhỏ ≤ √R trước, rồi với mỗi p ≤ √R đánh dấu bội của p trong [L, R] từ max(p^2, ⌈L/p⌉·p).
vector<bool> segSieve(ll L, ll R) {
    int len = (int)(R - L + 1);
    vector<bool> is(len, true);
    int rt = (int)sqrt((long double)R) + 1;
    vector<bool> small(rt + 1, true);
    vi sp;
    for (int i = 2; i <= rt; ++i) {
        if (small[i]) {
            sp.push_back(i);
            for (ll j = (ll)i * i; j <= rt; j += i) small[j] = false;
        }
    }
    for (int p : sp) {
        ll st = max((ll)p * p, (L + p - 1) / p * p);
        for (ll j = st; j <= R; j += p) is[j - L] = false;
    }
    if (L <= 0 && R >= 0) is[0 - L] = false;   // 0
    if (L <= 1 && R >= 1) is[1 - L] = false;   // 1
    return is;
}
// for (ll i = L; i <= R; ++i) if (is[i - L]) ... // nguyên tố
```

## Miller–Rabin

**Mục đích:** Kiểm tra nguyên tố xác định cho số $\le 7 \cdot 10^{18}$.

**Điều kiện sử dụng:**
- Cần `modmul` an toàn (n có thể > 2^32) — dùng modmul mục riêng.
- Bộ cơ sở `{2, 325, 9375, 28178, 450775, 9780504, 1795265022}` đủ cho mọi `n < 2^64`.

**Độ phức tạp:**
- Time: $O(7 \cdot \log^3 n)$ xấp xỉ.
- Space: $O(1)$

**Dependency:** template.cpp, modmul

```cpp
// id: millerrabin
// n - 1 = d·2^s; với mỗi cơ sở a: kiểm tra a^d ≡ 1 hoặc a^(d·2^r) ≡ -1 (r < s).
bool isPrime(ull n) {
    if (n < 2 || n % 6 % 4 != 1) return (n | 1) == 3;
    ull A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    int s = __builtin_ctzll(n - 1);
    ull d = n >> s;
    for (ull a : A) {
        ull p = modpow(a % n, d, n), i = s;
        while (p != 1 && p != n - 1 && a % n && i--) p = modmul(p, p, n);
        if (p != n - 1 && i != s) return false;
    }
    return true;
}
```

## Modmul an toàn (2^64)

**Mục đích:** `a*b % c` và `b^e % c` với $a, b, c \le 7.2 \cdot 10^{18}$ — dùng cho Miller–Rabin, Pollard rho, modpow mod lớn.

**Điều kiện sử dụng:**
- $c \ge 1$; long double đủ độ chính xác cho 64-bit trên x86 (80-bit).
- `modpow` gọi `modmul` thay `*` khi mod > 2^32.

**Độ phức tạp:**
- Time: $O(1)$ modmul, $O(\log e)$ modpow.
- Space: $O(1)$

**Dependency:** template.cpp

```cpp
// id: modmul
// long double ước lượng thương 1.L/c * a * b rồi trừ — O(1).
typedef unsigned long long ull;
ull modmul(ull a, ull b, ull M) {
    ll ret = a * b - M * ull(1.L / M * a * b);
    return ret + M * (ret < 0) - M * (ret >= (ll)M);
}
ull modpow(ull b, ull e, ull mod) {
    ull ans = 1;
    for (; e; b = modmul(b, b, mod), e /= 2)
        if (e & 1) ans = modmul(ans, b, mod);
    return ans;
}
```

## Pollard Rho (phân tích thừa số)

**Mục đích:** Phân tích n ($\le 2^{64}$) thành thừa số nguyên tố, không cần n nguyên tố nhỏ.

**Điều kiện sử dụng:**
- Kết quả là DANH SÁCH (có thể lặp) → `sort` rồi gộp nếu cần dạng $p^k$.
- Trước khi rho: chia thử 2,3,5.. rồi check `isPrime` để rút ngắn.

**Độ phức tạp:**
- Time: $O(n^{1/4})$ mỗi bước (kỳ vọng).
- Space: $O(\log n)$ đệ quy.

**Dependency:** template.cpp, modmul, millerrabin

```cpp
// id: factor
// f(x) = x^2 + i mod n, tìm gcd(|x-y|, n); trung bình O(n^(1/4)).
ull pollard(ull n) {
    ull x = 0, y = 0, t = 30, prd = 2, i = 1, q;
    auto f = [&](ull v) { return modmul(v, v, n) + i; };
    while (t++ % 40 || __gcd(prd, n) == 1) {
        if (x == y) x = ++i, y = f(x);
        if ((q = modmul(prd, max(x, y) - min(x, y), n))) prd = q;
        x = f(x), y = f(f(y));
    }
    return __gcd(prd, n);
}
vector<ull> factor(ull n) {
    if (n == 1) return {};
    if (isPrime(n)) return {n};
    ull x = pollard(n);
    auto l = factor(x), r = factor(n / x);
    l.insert(l.end(), all(r));
    return l;
}
```

## Mod sqrt (Tonelli–Shanks)

**Mục đích:** Tìm `x` sao cho $x^2 \equiv a \pmod{p}$, p nguyên tố lẻ — modular square root.

**Điều kiện sử dụng:**
- `p` nguyên tố lẻ; nếu `legendre(a,p) != 1` → không có nghiệm (assert).
- `p % 4 == 3` → nghiệm ngay $a^{(p+1)/4}$.

**Độ phức tạp:**
- Time: $O(\log^2 p)$ tệ nhất, $O(\log p)$ hầu hết.
- Space: $O(1)$

**Dependency:** template.cpp, modmul

```cpp
// id: modsqrt
// trường hợp tổng quát: tách p - 1 = s·2^r, tìm căn gốc phụ n (không phải QR), lặp nâng bậc 2.
ll modsqrt(ll a, ll p) {
    a %= p;
    if (a < 0) a += p;
    if (a == 0) return 0;
    assert(modpow(a, (p - 1) / 2, p) == 1);  // không có nghiệm
    if (p % 4 == 3) return modpow(a, (p + 1) / 4, p);
    ll s = p - 1;
    int r = 0, m;
    while (s % 2 == 0) ++r, s /= 2;
    ll n = 2;
    while (modpow(n, (p - 1) / 2, p) != p - 1) ++n;  // n không phải QR
    ll x = modpow(a, (s + 1) / 2, p), b = modpow(a, s, p), g = modpow(n, s, p);
    while (true) {  // invariant: thứ tự b chia 2^{r-1} → m <= r-1
        ll t = b;
        for (m = 0; m < r && t != 1; ++m) t = t * t % p;
        if (m == 0) return x;
        ll gs = modpow(g, 1LL << (r - m - 1), p);
        g = gs * gs % p;
        x = x * gs % p;
        b = b * g % p;
        r = m;
    }
}
```

## Discrete log (modLog)

**Mục đích:** Tìm `x` nhỏ nhất `> 0` với $a^x \equiv b \pmod{m}$; hoặc thứ tự của `a` (gọi `modLog(a,1,m)`).

**Điều kiện sử dụng:**
- `m` bất kỳ (không cần nguyên tố); `unordered_map` cache bước con.
- Không tồn tại → kiểm tra `gcd(m, a^{n}) == gcd(m, b)` trước khi kết luận -1.

**Độ phức tạp:**
- Time: $O(√m)$
- Space: $O(√m)$

**Dependency:** template.cpp

```cpp
// id: modlog
// baby-step giant-step: x = i·n + j, precompute a^j vào hash, lặp a^(i·n) tra.
ll modLog(ll a, ll b, ll m) {
    ll n = (ll)sqrt(m) + 1, e = 1, f = 1, j = 1;
    unordered_map<ll, ll> A;
    while (j <= n && (e = f = e * a % m) != b % m) A[e * b % m] = j++;
    if (e == b % m) return j;
    if (__gcd(m, e) == __gcd(m, b))
        rep(i, 2, n + 2) if (A.count(e = e * f % m)) return n * i - A[e];
    return -1;
}
```

## Phi Euler & Mobius

**Mục đích:** $\varphi(n)$ = số ước nguyên tố với n; $\mu(n)$ = Mobius — dùng cho Euler theorem, đếm theo ước, Mobius inversion.

**Điều kiện sử dụng:**
- Sàng mảng $O(LIM)$; công thức Π theo phân tích thừa số (dùng spf).
- Euler: $a^{\varphi(n)} \equiv 1$ khi $\gcd(a,n) = 1$ → lũy thừa modulo KHÔNG nguyên tố. Lọc Mobius: $g(n) = \sum_{d \mid n} f(d)$ → $f(n) = \sum \mu(d) g(n/d)$.

**Độ phức tạp:**
- Time: $O(LIM \log \log LIM)$ sàng; $O(\log n)$ mỗi số.
- Space: $O(LIM)$

**Dependency:** template.cpp

```cpp
// id: phi
// φ(n) = n Π (1 - 1/p); φ(p^k) = (p-1)p^(k-1); Σ_{d|n} φ(d) = n. μ(n) = 0 nếu n có bình phương thừa.
const int PHI_LIM = 1000000;
int phi[PHI_LIM + 1], mu[PHI_LIM + 1];
vi phiPrimes;
void sievePhiMu() {
    rep(i, 0, PHI_LIM + 1) phi[i] = i, mu[i] = 1;
    rep(i, 2, PHI_LIM + 1) {
        if (phi[i] == i) {  // i nguyên tố
            phiPrimes.push_back(i);
            phi[i] = i - 1;
            mu[i] = -1;
        }
        for (int p : phiPrimes) {
            if (i * p > PHI_LIM) break;
            if (i % p == 0) {
                phi[i * p] = phi[i] * p;  // p | i → φ(ip) = φ(i)·p
                mu[i * p] = 0;
                break;
            }
            phi[i * p] = phi[i] * (p - 1);  // p ∤ i → φ(ip) = φ(i)·(p-1)
            mu[i * p] = -mu[i];
        }
    }
    phi[1] = 1;
}
// phi(n) cho n lớn (không cần mảng): factor(n) rồi phi = n * Π (1 - 1/p)
```

## Công thức số học nhanh

**Mục đích:** Ước lượng, tính chất ước/chữ số để bound nhanh khi thi.

**Điều kiện sử dụng:**
- Chỉ để ước lượng/ bound, không dùng làm chính xác.

**Độ phức tạp:**
- Time: $O(1)$ tra cứu.

**Code:** — (công thức tra cứu)

$$\text{Số ước của n: } \approx 100\ (n < 5 \cdot 10^4),\ \approx 500\ (n < 10^7),\ \approx 2000\ (n < 10^{10}),\ \approx 2 \cdot 10^5\ (n < 10^{19})$$
$$\sum_{d \mid n} d = O(n \log \log n)$$
$$\text{n! số chữ số: } 20! \approx 2 \cdot 10^{18}\ (\text{cận ll}),\quad 171! > \text{DBL\_MAX}$$
$$2^{64} \approx 1.8 \cdot 10^{19};\quad 10^{18} < 2^{60}$$
$$\text{Nguyên tố cho hashing: } 10^9+7,\ 10^9+9,\ 998244353 = 119 \cdot 2^{23} + 1$$
$$\text{Pythagorean: } a = k(m^2 - n^2),\ b = k \cdot 2mn,\ c = k(m^2 + n^2),\ m > n > 0,\ m \not\equiv n \pmod{2}$$

## Floor sum (modsum)

**Mục đích:** Tính $\sum_{i=0}^{n-1} (a \cdot i + b) / m$ (floor) — dạng xuất hiện khi đếm cặp, tích phân số nguyên.

**Điều kiện sử dụng:**
- Toàn unsigned/ll, `m > 0`; tổng có thể ~10^18 → dùng ull cẩn thận.
- `modsum` tổng modulo: $\sum (k \cdot i + c) \% m = k \cdot \operatorname{sumsq}(n) + c \cdot n - m \cdot \operatorname{divsum}(\dots)$.

**Độ phức tạp:**
- Time: $O(\log m)$
- Space: $O(\log m)$ đệ quy

**Dependency:** template.cpp

```cpp
// id: floorsum
// đệ quy biến đổi (n,a,b,m) giống Euclid → O(log m).
typedef unsigned long long ull;
ull sumsq(ull to) { return to / 2 * ((to - 1) | 1); }  // Σ_{i<to} i
ull divsum(ull to, ull c, ull k, ull m) {               // Σ floor((k*i+c)/m), i<to
    ull res = k / m * sumsq(to) + c / m * to;
    k %= m;
    c %= m;
    if (!k) return res;
    ull to2 = (to * k + c) / m;
    return res + (to - 1) * to2 - divsum(to2, m - 1 - c, m, k);
}
ll modsum(ull to, ll c, ll k, ll m) {  // Σ_{i<to} (k*i+c) % m
    c = ((c % m) + m) % m;
    k = ((k % m) + m) % m;
    return to * c + k * sumsq(to) - m * divsum(to, c, k, m);
}
```
