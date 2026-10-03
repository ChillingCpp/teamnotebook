# 03 — Tổ hợp (Combinatorics)

## nCr & giai thừa (mod nguyên tố)

**Mục đích:** Chỉnh hợp / tổ hợp modulo nguyên tố, precompute `fac`, `ifac` cho mọi $n \le N$.

**Ý tưởng / Observation:**
- `C(n,k) = fac[n] · ifac[k] · ifac[n-k] mod p` — precompute $O(N)$ một lần, mỗi truy vấn $O(1)$.
- $n!$ tràn `ll` ở `n = 21` → LUÔN làm modulo kể cả khi đề bài "nhỏ" (hay mod p).
- `n` rất lớn, `k` nhỏ → tính trực tiếp $\frac{n \cdot (n-1) \cdots}{k!}$ bằng nghịch đảo từng bước.

**Điều kiện sử dụng:**
- `mod` nguyên tố, `N < mod` (nếu $N \ge mod$ dùng Lucas).

**Độ phức tạp:**
- Time: $O(N)$ precompute, $O(1)$ mỗi query.
- Space: $O(N)$

**Dependency:** template.cpp, modpow

```cpp
// id: ncr
// MOD lấy từ modpow (dependency) — đổi tại đó nếu bài dùng mod khác
const int NCR_MAX = 1000000;
ll fac[NCR_MAX + 1], ifac[NCR_MAX + 1];
void buildNcr() {
    fac[0] = ifac[0] = 1;
    rep(i, 1, NCR_MAX + 1) fac[i] = fac[i - 1] * i % MOD;
    ifac[NCR_MAX] = modpow(fac[NCR_MAX], MOD - 2, MOD);
    for (int i = NCR_MAX; i > 0; --i) ifac[i - 1] = ifac[i] * i % MOD;
}
ll C(ll n, ll k) {
    if (k < 0 || k > n) return 0;
    return fac[n] * ifac[k] % MOD * ifac[n - k] % MOD;
}
// nCk exact (không mod, dừng khi tràn ll — dùng để check sınır):
// ll nCk(ll n, ll k) { if (n-k<k) swap(k,n-k); ll r=1; rep(i,1,k+1) r = r*(n-i+1)/i; return r; }
```

## Lucas (nCr mod p bất kỳ)

**Mục đích:** $C(n,k) \bmod p$ với `n, k` bất kỳ (p nguyên tố, kể cả $n \ge p$).

**Ý tưởng / Observation:**
- Viết n, k theo cơ sở p: $C(n,k) \equiv \prod C(n_i, k_i) \pmod{p}$ — mỗi digit dùng bảng nhỏ $\le p$.
- $k_i > n_i$ → kết quả 0 ngay (nhận ra nhanh "không chọn được").

**Điều kiện sử dụng:**
- `p` nguyên tố; precompute `fac` đến `p-1` ($p \lesssim 10^6$).

**Độ phức tạp:**
- Time: $O(p + log_p n)$
- Space: $O(p)$

**Dependency:** template.cpp

```cpp
// id: lucas
ll lucas(ll n, ll k, ll p, const vi& fac, const vi& invfac) {
    if (k < 0 || k > n) return 0;
    ll res = 1;
    while (n || k) {
        ll ni = n % p, ki = k % p;
        if (ki > ni) return 0;
        res = res * fac[ni] % p * invfac[ki] % p * invfac[ni - ki] % p;
        n /= p;
        k /= p;
    }
    return res;
}
// build: fac[0]=1; rep(i,1,p) fac[i]=fac[i-1]*i%p;
// invfac[p-1] = modpow(fac[p-1], p-2, p); suy ngược.
```

## Multi-nomial

**Mục đích:** $C(k_1+\dots+k_n;\ k_1,\dots,k_n) = \frac{(\sum k_i)!}{k_1! \cdots k_n!}$ — chia n phần tử thành nhóm.

**Ý tưởng / Observation:**
- Nhân chia dần, KHÔNG tính $(\sum k_i)!$ riêng (tràn) — chia ngay khi nhân: `c = c * ++m / (j+1)` luôn chia hết từng bước.

**Điều kiện sử dụng:**
- Không mod (kết quả chia hết từng bước); kiểm tra cận trước khi dùng.

**Độ phức tạp:**
- Time: $O(Σ ki)$
- Space: $O(1)$

**Dependency:** template.cpp

```cpp
// id: multinomial
ll multinomial(vi& v) {
    ll c = 1;
    int m = v.empty() ? 1 : v[0];
    rep(i, 1, sz(v)) rep(j, 0, v[i]) c = c * ++m / (j + 1);
    return c;
}
```

## Catalan

**Mục đích:** Đếm cấu trúc: đường đơn điệu, ngoặc đúng, tam giác hóa đa giác lồi, dyck path.

**Ý tưởng / Observation:**
- $C_n = \frac{C(2n,n)}{n+1}$ — làm mod: nhân với `nghịch đảo(n+1)` (mod nguyên tố).
- Nhận ra Catalan: bài đếm kết quả = $C(2n,n)$ với constraint "không vượt đường chéo" →很可能 là Catalan.

**Điều kiện sử dụng:**
- Nhanh nhận diện bằng các bài mẫu (ngoặc, path không vượt y=x, parenthesization).

**Độ phức tạp:**
- Time: $O(1)$ với nCr đã build, $O(n)$ nếu tính dần.
- Space: $O(1)$

**Dependency:** template.cpp, ncr

```cpp
// id: catalan
ll catalan(ll n) { return C(2 * n, n) * modinv(n + 1, MOD) % MOD; }
// tính dần: cat[0]=1; rep(i,1,n) cat[i] = cat[i-1] * (4*i-2) % MOD * modinv(i+1) % MOD;
```

## Stirling, Eulerian, Bell

**Mục đích:** Đếm chu trình hoán vị, cách chia nhóm, hoán vị có đúng k điểm tăng (đứng dậy).

**Ý tưởng / Observation:**
- Stirling 1 (số cycle): $c(n,k) = c(n-1,k-1) + (n-1) \cdot c(n-1,k)$.
- Stirling 2 (chia k nhóm): $S(n,k) = S(n-1,k-1) + k \cdot S(n-1,k)$; sum over k = $B_n$ (Bell).
- Eulerian $E(n,k)$ = số hoán vị đúng k điểm tăng: $E(n,k) = (n-k)E(n-1,k-1) + (k+1)E(n-1,k)$.
- Nhận diện: "điều kiện chỉ phụ thuộc vị trí tương đối của hoán vị" → Stirling/Eulerian.

**Điều kiện sử dụng:**
- Table $O(n^2)$ modulo; $n \le 2000$ OK.

**Độ phức tạp:**
- Time: $O(n^2)$ build.
- Space: $O(n^2)$

**Dependency:** template.cpp, modpow

```cpp
// id: stirling
const int STN = 505;
ll stir1[STN][STN], stir2[STN][STN], bell[STN], euler[STN][STN];
void buildStirling(int n) {
    stir1[0][0] = stir2[0][0] = 1;
    euler[0][0] = 1;
    bell[0] = 1;  // B₀ = S(0,0) = 1 (vòng lặp dưới bắt đầu từ i = 1)
    rep(i, 1, n + 1) {
        rep(k, 1, i + 1) {
            stir1[i][k] = (stir1[i - 1][k - 1] + (i - 1) * stir1[i - 1][k]) % MOD;
            stir2[i][k] = (stir2[i - 1][k - 1] + k * stir2[i - 1][k]) % MOD;
        }
        rep(k, 0, i)  // E(n,k), k = 0..n-1
            euler[i][k] = ((i - k) * (k > 0 ? euler[i - 1][k - 1] : 0) + (k + 1) * euler[i - 1][k]) % MOD;
        rep(k, 0, i + 1) bell[i] = (bell[i] + stir2[i][k]) % MOD;
    }
}
// Công thức đóng (Stirling 2): S(n,k) = 1/k! · Σ_j (-1)^{k-j} C(k,j) j^n
```

## Burnside / Polya

**Mục đích:** Đếm cấu trúc "với đối xứng" — vòng quay, phản xạ, color necklaces.

**Ý tưởng / Observation:**
- Burnside: số lớp = $\frac{1}{|G|} \sum_g |\mathrm{Fix}(g)|$ — mỗi phần tử nhóm tính số cấu trúc bất động.
- Chỉ cần đối xứng LUÂN VỊ (xoay vòng): $g(n) = \frac{1}{n} \sum_{k=0}^{n-1} f(\gcd(n,k)) = \frac{1}{n} \sum_{d \mid n} f(d) \cdot \varphi(n/d)$.
- Nhận diện: "2 cấu trúc coi là bằng nhau nếu xoay được" → Burnside với nhóm Z_n.

**Điều kiện sử dụng:**
- Kết quả chia $|G|$ → làm modulo với nghịch đảo.

**Độ phức tạp:**
- Time: $O(n)$ hoặc $O(Σ ước)$ với Euler phi.
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: burnside — đếm vòng quay: f(n) = (1/n) Σ_{k=0}^{n-1} f(gcd(n,k))
ll rotateNecklace(int n, auto f) {  // f(d) = số cấu trúc bất động khi xoay gcd
    ll s = 0;
    rep(k, 0, n) s += f(__gcd(n, k));
    return s / n;  // hoặc s * modinv(n) % MOD nếu làm mod
}
```

## Inclusion–Exclusion

**Mục đích:** Đếm phần tử thỏa ÍT NHẤT một điều kiện / loại trừ từng cái sai.

**Ý tưởng / Observation:**
- $|\cap \text{complements}| = \sum_{S} (-1)^{|S|} \cdot |\text{thỏa mọi điều kiện trong } S|$ — chọn subset điều kiện ($2^n$ khi $n \le 20$).
- Khi n lớn → dùng công thức Möbius (mục Phi/Mobius) hoặc DP theo trạng thái.
- Dạng hay gặp: "mỗi vị trí nằm trong đoạn nào đó" → IE trên vị trí bị vi phạm.

**Điều kiện sử dụng:**
- $2^{|S|}$ phải tính được; kiểm tra $|S| \le 25$.

**Độ phức tạp:**
- Time: $O(2ⁿ \cdot poly)$ hoặc $O(n\cdot 2ⁿ)$.
- Space: $O(2ⁿ)$

**Dependency:** template.cpp

```cpp
// id: inclusion — đếm x có ít nhất 1 thuộc mỗi nhóm (mask bit = nhóm)
ll ieCount(int groups, auto cntSubset) {  // cntSubset(mask) = #x thỏa mọi nhóm trong mask
    ll res = 0;
    rep(mask, 1, 1 << groups) res += (__builtin_popcount(mask) & 1 ? 1 : -1) * cntSubset(mask);
    return res;
}
```

## Công thức tổ hợp thường dùng

**Mục đích:** Tra cứu nhanh các đẳng thức đếm.

**Ý tưởng / Observation:**
- Nhận diện "chia tay" → stars and bars; "đảo điều kiện" → IE; "vòng" → Burnside/phi.
- Thép (`Pick`): $A = I + \frac{B}{2} - 1$ — đếm điểm nguyên trong/lề tam giác/đa giác nguyên.

**Điều kiện sử dụng:**
- Kiểm tra biến nhỏ bằng brute trước khi bắn công thức.

**Độ phức tạp:**
- Time: $O(1)$ tra cứu.

**Code:** — (công thức tra cứu)

$$\text{Thurtles \& bars: } \binom{n+k-1}{k} = \text{cách chia n sao có k nhóm (nhóm rỗng OK)}$$
$$\text{Chọn ra 1 rồi chia: chọn } k \text{ phần tử bất kỳ} \Rightarrow C(n,k) \cdot (\text{còn lại})$$
$$\text{Pick's theorem: } A = I + \frac{B}{2} - 1 \quad (\text{I = điểm trong, B = điểm trên biên})$$
$$\text{Vandermonde: } \sum_k C(a,k)\,C(b,n-k) = C(a+b, n)$$
$$\text{Đệ quy Pascal: } C(n,k) = C(n-1,k-1) + C(n-1,k)$$
$$\text{Phân hoạch } p(n)\text{: } p(n) = \sum_{k \ne 0} (-1)^{k+1} p\left(n - \frac{k(3k-1)}{2}\right) \quad [\text{Euler pentagonal}]$$
$$\text{Derangement } D(n)\text{: } D(n) = (n-1)(D(n-1)+D(n-2)) = n \cdot D(n-1) + (-1)^n, \quad D(n) = \operatorname{round}(n!/e)$$
$$\text{Cây ngụy nhiên: } n^{n-2} \text{ cách (Cayley)};\ \text{labeled trees trên } n \text{ đỉnh có bậc } d_i\text{: } \frac{(n-2)!}{\prod (d_i-1)!}$$
$$\text{Catalan: } C_n = \frac{C(2n,n)}{n+1} \text{ — ngoặc, path, tam giác hóa, monotone path}$$
