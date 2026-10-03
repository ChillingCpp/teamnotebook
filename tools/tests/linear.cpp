//@ ids: matrix, determinant, detmod, solvelinear, solvebinary
// Test ngẫu nhiên (seed cố định): so từng phép với brute force tham chiếu
// (Leibniz cho định thức, hạng mod GF(p) cho hệ tuyến tính, BF 2^m cho GF(2)).
int fails = 0;
void check(bool ok, const char* msg) {
    if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
}

mt19937_64 rng(20261004);
int rnd(int l, int r) { return int(rng() % (unsigned)(r - l + 1)) + l; }

ll modPow(ll a, ll e, ll p) {
    ll r = 1;
    for (; e; e >>= 1) { if (e & 1) r = r * a % p; a = a * a % p; }
    return r;
}

// Hạng ma trận nguyên over GF(p). Với |a| ≤ 3 và cỡ ≤ 5×6 thì mọi subminor
// < p = 1e9+7 (Hadamard) → khớp chính xác hạng over Q.
ll refRankMod(vector<vector<ll>> a, ll p) {
    int n = sz(a), m = n ? sz(a[0]) : 0;
    rep(i, 0, n) rep(j, 0, m) { a[i][j] %= p; if (a[i][j] < 0) a[i][j] += p; }
    ll rk = 0;
    rep(c, 0, m) {
        int piv = -1;
        rep(r, rk, n) if (a[r][c]) { piv = r; break; }
        if (piv < 0) continue;
        swap(a[rk], a[piv]);
        ll inv = modPow(a[rk][c], p - 2, p);
        rep(r, rk + 1, n) if (a[r][c]) {
            ll f = a[r][c] * inv % p;
            rep(cc, c, m) {
                a[r][cc] = (a[r][cc] - f * a[rk][cc]) % p;
                if (a[r][cc] < 0) a[r][cc] += p;
            }
        }
        ++rk;
    }
    return rk;
}

// Định thức Leibniz (n ≤ 5) mod bất kỳ — tham chiếu chính xác cho detmod.
ll refDetMod(const vector<vector<ll>>& a, ll mod) {
    int n = sz(a);
    vi p(n);
    iota(all(p), 0);
    ll res = 0;
    do {
        int inv = 0;
        rep(i, 0, n) rep(j, i + 1, n) if (p[i] > p[j]) ++inv;
        __int128 prod = 1;
        rep(i, 0, n) {
            ll v = a[i][p[i]] % mod;
            if (v < 0) v += mod;
            prod = prod * v % mod;
        }
        if (inv & 1) prod = (mod - prod) % mod;
        res = (ll)((res + (ll)prod) % mod);
    } while (next_permutation(all(p)));
    return res;
}

// Định thức Leibniz có dấu (n ≤ 4, |a| ≤ 4) — tham chiếu chính xác cho det().
ll refDetInt(const vector<vector<ll>>& a) {
    int n = sz(a);
    vi p(n);
    iota(all(p), 0);
    ll res = 0;
    do {
        ll prod = 1;
        rep(i, 0, n) prod *= a[i][p[i]];
        int inv = 0;
        rep(i, 0, n) rep(j, i + 1, n) if (p[i] > p[j]) ++inv;
        res += (inv & 1) ? -prod : prod;
    } while (next_permutation(all(p)));
    return res;
}

template <int N>
void testMatrix() {
    Matrix<ll, N> I;
    rep(i, 0, N) I.d[i][i] = 1;
    rep(tt, 0, 60) {
        Matrix<ll, N> A, B;
        rep(i, 0, N) rep(j, 0, N) { A.d[i][j] = rnd(-3, 3); B.d[i][j] = rnd(-3, 3); }
        auto AI = A * I, IA = I * A;
        rep(i, 0, N) rep(j, 0, N)
            check(AI.d[i][j] == A.d[i][j] && IA.d[i][j] == A.d[i][j], "matrix.identity");
        auto C = A * B;
        rep(i, 0, N) rep(j, 0, N) {
            ll s = 0;
            rep(k, 0, N) s += A.d[i][k] * B.d[k][j];
            check(C.d[i][j] == s, "matrix.mul");
        }
        vector<ll> v(N), want(N);
        rep(i, 0, N) v[i] = rnd(-4, 4);
        rep(i, 0, N) {
            ll s = 0;
            rep(k, 0, N) s += A.d[i][k] * v[k];
            want[i] = s;
        }
        check((A * v) == want, "matrix.matvec");
        int p = rnd(0, 8);
        auto P = A ^ p;
        Matrix<ll, N> R;
        rep(i, 0, N) R.d[i][i] = 1;
        rep(t, 0, p) R = R * A;
        rep(i, 0, N) rep(j, 0, N) check(P.d[i][j] == R.d[i][j], "matrix.pow");
        auto P0 = A ^ 0;
        rep(i, 0, N) rep(j, 0, N) check(P0.d[i][j] == I.d[i][j], "matrix.pow0");
    }
}

int main() {
    // ---------- matrix ----------
    testMatrix<2>();
    testMatrix<3>();
    {  // fib: (F^n) * {1,0} = {F_{n+1}, F_n} (giới hạn n ≤ 63 để không tràn ll khi lũy thừa bội)
        Matrix<ll, 2> F;
        F.d[0][0] = 1; F.d[0][1] = 1; F.d[1][0] = 1; F.d[1][1] = 0;
        ll f0 = 0, f1 = 1;
        rep(n, 0, 64) {
            vector<ll> e{1, 0}, r = (F ^ n) * e;
            check(r[0] == f1 && r[1] == f0, "matrix.fib");
            ll nf = f0 + f1;
            f0 = f1; f1 = nf;
        }
    }

    // ---------- determinant ----------
    rep(tt, 0, 300) {
        int n = rnd(1, 4);
        vector<vector<ll>> a(n, vector<ll>(n));
        rep(i, 0, n) rep(j, 0, n) a[i][j] = rnd(-4, 4);
        if (n >= 2 && rnd(0, 3) == 0)        // nhân đôi hàng → suy biến
            rep(j, 0, n) a[n - 1][j] = a[0][j];
        if (n >= 3 && rnd(0, 3) == 0)        // hàng cuối = tổng 2 hàng trên
            rep(j, 0, n) a[n - 1][j] = a[0][j] + a[1][j];
        if (n >= 2 && rnd(0, 3) == 0)        // hàng toàn 0
            rep(j, 0, n) a[n - 1][j] = 0;
        ll want = refDetInt(a);
        vector<vector<double>> ad(n, vector<double>(n));
        rep(i, 0, n) rep(j, 0, n) ad[i][j] = (double)a[i][j];
        double got = det(ad);
        check(fabs(got - (double)want) <= 1e-9 * max(1.0, fabs((double)want)), "determinant");
    }

    // ---------- detmod ----------
    rep(tt, 0, 300) {
        ll mods[5] = {1000000007, 1000000009, 1000000000, 97, 1000003};
        ll mod = mods[rnd(0, 4)];
        int n = rnd(1, 5);
        vector<vector<ll>> a(n, vector<ll>(n));
        rep(i, 0, n) rep(j, 0, n) {
            a[i][j] = rnd(0, int(mod - 1));
            if (rnd(0, 1)) a[i][j] -= mod;  // cho phép cả phần tử âm (|a| < mod)
        }
        if (n >= 2 && rnd(0, 3) == 0)
            rep(j, 0, n) a[n - 1][j] = a[0][j];
        ll want = refDetMod(a, mod);
        auto ac = a;
        ll got = detmod(ac, mod);
        check(0 <= got && got < mod, "detmod.range");
        check(got == want, "detmod");
    }

    // ---------- solvelinear (Ax = b, số thực) ----------
    const ll P = 1000000007;
    rep(tt, 0, 400) {
        int n = rnd(1, 5), m = rnd(1, 5);  // n pt, m ẩn
        vector<vector<ll>> A0(n, vector<ll>(m));
        rep(i, 0, n) rep(j, 0, m) A0[i][j] = rnd(-3, 3);
        vi b0(n), xt(m);
        bool constructed = rnd(0, 1);
        if (constructed) {  // b = A*x* → chắc chắn có nghiệm
            rep(j, 0, m) xt[j] = rnd(-2, 2);
            rep(i, 0, n) {
                ll s = 0;
                rep(j, 0, m) s += A0[i][j] * xt[j];
                b0[i] = s;
            }
        } else {
            rep(i, 0, n) b0[i] = rnd(-5, 5);
        }
        vector<vector<ll>> aug = A0;
        rep(i, 0, n) aug[i].push_back(b0[i]);
        ll rA = refRankMod(A0, P), rAug = refRankMod(aug, P);
        bool wantOk = (rA == rAug);
        vector<vd> A(n, vd(m));
        vd b(n), x(m);
        rep(i, 0, n) { rep(j, 0, m) A[i][j] = (double)A0[i][j]; b[i] = (double)b0[i]; }
        int got = solveLinear(A, b, x);
        if (!wantOk) {
            check(got == -1, "solvelinear.inconsistent");
        } else {
            check(got == (int)rA, "solvelinear.rank");
            rep(i, 0, n) {
                double s = 0;
                rep(j, 0, m) s += A0[i][j] * x[j];
                check(fabs(s - b0[i]) <= 1e-7, "solvelinear.solution");
            }
            if (constructed && rA == m)  // đầy đủ ẩn → nghiệm duy nhất = x*
                rep(j, 0, m) check(fabs(x[j] - xt[j]) <= 1e-7, "solvelinear.unique");
        }
    }

    // ---------- solvebinary (Ax = b trên GF(2)) ----------
    rep(tt, 0, 300) {
        int n = rnd(1, 8), m = rnd(1, 10);
        vector<bs> A(n);
        vi b(n);
        rep(i, 0, n) rep(j, 0, m) if (rnd(0, 2)) A[i].set(j);
        if (rnd(0, 3) == 0) {  // chèn hàng toàn 0 (và có thể làm hệ vô nghiệm)
            int r = rnd(0, n - 1);
            A[r].reset();
            if (rnd(0, 1)) b[r] = 1;
        }
        bool constructed = rnd(0, 1);
        if (constructed) {
            bs xt;
            rep(j, 0, m) if (rnd(0, 1)) xt.set(j);
            rep(i, 0, n) b[i] = int(((A[i] & xt).count()) & 1);
        } else {
            rep(i, 0, n) b[i] = rnd(0, 1);
        }
        // brute force 2^m: đếm nghiệm của Ax=0 → nullity = log2(số nghiệm);
        // Ax=b có nghiệm ⇔ tồn tại mask với y == b
        int nsol = 0;
        bool cons = false;
        rep(mask, 0, 1 << m) {
            bs t;
            rep(j, 0, m) if (mask & (1 << j)) t.set(j);
            vi y(n);
            rep(i, 0, n) y[i] = int(((A[i] & t).count()) & 1);
            bool zero = true;
            rep(i, 0, n) if (y[i]) { zero = false; break; }
            if (zero) ++nsol;
            if (y == b) cons = true;
        }
        int nullity = 0;
        while ((1 << nullity) < nsol) ++nullity;  // nsol = 2^nullity
        check((1 << nullity) == nsol, "solvebinary.nsol-power2");
        int rank = m - nullity;
        auto Ac = A;
        vi bc = b;
        bs x;
        int got = solveLinear2(Ac, bc, x, m);
        if (!cons) {
            check(got == -1, "solvebinary.inconsistent");
        } else {
            check(got == rank, "solvebinary.rank");
            rep(i, 0, n) check(int(((A[i] & x).count()) & 1) == b[i], "solvebinary.solution");
        }
    }

    if (fails) { printf("linear: %d loi\n", fails); return 1; }
    printf("linear: OK\n");
    return 0;
}
