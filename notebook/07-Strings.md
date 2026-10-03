# 07 — Chuỗi (Strings)

## KMP (prefix function)

**Mục đích:** Tìm vị trí pattern trong text; đếm occur; xây prefix function `pi[i]` = longest proper prefix = suffix của `s[0..i]`.

**Ý tưởng / Observation:**
- `pi` trả lời mọi câu hỏi "chuỗi con lặp lại": bài đếm số lần pattern xuất hiện (lặp lại chồng nhau OK), độ dài chuỗi Periodic (check `n - pi[n-1]` chia `n`), số vị trí period.
- "Đặt pattern vào text, ngăn cách" → tìm pattern qua `s + '#' + p` rồi lấy `pi` cuối.
- Nhận ra bài: "tìm lần xuất hiện đầu tiên / tất cả vị trí" → KMP hoặc Z; "tính lặp lại" → prefix function.

**Điều kiện sử dụng:**
- Text/pattern ASCII bất kỳ (không null byte). Chỉ dùng khi pattern cố định (offline) — nếu query nhiều pattern → Aho-Corasick.

**Độ phức tạp:**
- Time: $O(|text| + |pattern|)$
- Space: $O(|pattern|)$

**Dependency:** template.cpp

```cpp
// id: kmp
vi piFunction(const string& s) {
    vi pi(sz(s));
    rep(i, 1, sz(s)) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}
// vị trí xuất hiện của p trong t (kể cả chồng):
vi kmpSearch(const string& t, const string& p) {
    vi pi = piFunction(p + "#" + t), res;
    rep(i, sz(p) + 1, sz(pi)) if (pi[i] == sz(p)) res.push_back(i - 2 * sz(p));
    return res;
}
// period: int per = sz(s) - pi.back(); if (sz(s) % per == 0) → chuỗi lặp per.
```

## Z-function

**Mục đích:** `z[i]` = độ dài chuỗi con chung lớn nhất của `s` và `s[i..]` — tìm pattern, tính chuỗi con chung.

**Ý tưởng / Observation:**
- $z[i] \ge z[i-1] - 1$ → dùng biến `l, r` (đoạn Z rightmost) tính $O(1)$ mỗi vị trí.
- Pattern match: `s = p + '#' + t`, `z[sz(p)+1+i] == sz(p)` → occur tại i (giống KMP nhưng z của text).
- Nhận ra: "chuỗi con chung của s với mọi hậu tố" / "đếm vị trí mà z == k" → Z.

**Điều kiện sử dụng:**
- $O(n)$ mỗi chuỗi; không dùng khi query nhiều pattern.

**Độ phức tạp:**
- Time: $O(n)$
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: zfunc
vi zFunction(const string& s) {
    int n = sz(s);
    vi z(n);
    for (int i = 1, l = 0, r = 0; i < n; ++i) {
        if (i < r) z[i] = min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) ++z[i];
        if (i + z[i] > r) l = i, r = i + z[i];
    }
    return z;
}
```

## Manacher (palindrome)

**Mục đích:** Với mọi vị trí, bán kính palindrome lớn nhất (lẻ & chẵn) — tìm palindrome dài nhất, đếm palindrome con.

**Ý tưởng / Observation:**
- `p[1][i]` = bán kính PALINDROME LẺ tâm tại i (kể cả tâm); `p[0][i]` = bán kính chẵn (tâm giữa i-1, i).
- Chuỗi sau khi chèn `#` → mỗi palindrome thành lẻ → dùng biến `l, r` rightmost như Z.
- Bài "đảo chuỗi/conpoly" → đếm = $\sum p[0][i] + p[1][i]$.

**Điều kiện sử dụng:**
- Chuỗi `char` bất kỳ (không null). Kết quả `p[1]` = số ký tự mỗi bên (kể cả tâm).

**Độ phức tạp:**
- Time: $O(n)$
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: manacher
// pair(p0, p1): p0 = bán kính chẵn (tâm giữa i-1/i), p1 = bán kính lẻ tâm i
pair<vi, vi> manacher(const string& s) {
    int n = sz(s);
    vi p1(n), p0(n);
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        p1[i] = i <= r ? min(r - i + 1, p1[l + r - i]) : 1;
        while (i - p1[i] >= 0 && i + p1[i] < n && s[i - p1[i]] == s[i + p1[i]]) ++p1[i];
        if (i + p1[i] - 1 > r) l = i - p1[i] + 1, r = i + p1[i] - 1;
    }
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        p0[i] = i <= r ? min(r - i + 1, p0[l + r - i + 1]) : 0;
        while (i - p0[i] - 1 >= 0 && i + p0[i] < n && s[i - p0[i] - 1] == s[i + p0[i]]) ++p0[i];
        if (i + p0[i] - 1 > r) l = i - p0[i], r = i + p0[i] - 1;
    }
    return {p0, p1};
}
// palindrome lẻ dài nhất: tâm i, độ dài = 2*p1[i]-1... với p1 tính cả tâm → 2*p1-1.
```

## Rolling hash

**Mục đích:** So sánh nhanh 2 chuỗi con bằng nhau $O(1)$ — đếm distinct substrings, chuỗi xoay, tìm pattern nhiều vị trí.

**Ý tưởng / Observation:**
- $h[r] = h[l] \cdot B^{r-l} + (s[l..r)) \bmod 2^{64}$ (unsigned) — nhanh, đủ cho hầu hết bài.
- Chống hack: base ngẫu nhiên + mod đơn (1e9+7/9) hoặc double hash (2 mod) → chống test đối kháng.
- Chuỗi xoay: `s+s`, substring `len n` → hash compare $O(n)$.

**Điều kiện sử dụng:**
- Base `B` nên ≥ alphabet size, ngẫu nhiên `128..255`; $h[0] = 0$ (chuỗi rỗng). `int` → chuyển `ll` để tránh tràn khi nhân.

**Độ phức tạp:**
- Time: $O(n)$ build, $O(1)$ query
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: hash
typedef unsigned long long ull;
struct RollingHash {
    const ull B = 911382323;  // base ngẫu nhiên
    vector<ull> h, p;
    RollingHash(const string& s) : h(sz(s) + 1, 0), p(sz(s) + 1, 1) {
        rep(i, 0, sz(s)) {
            h[i + 1] = h[i] * B + (ull)(s[i] + 1);
            p[i + 1] = p[i] * B;
        }
    }
    ull get(int l, int r) const {  // hash của s[l..r)
        return h[r] - h[l] * p[r - l];
    }
};
// so sánh s[l1,r1) == s[l2,r2): hash.get(l1,r1) == hash.get(l2,r2) (kiểm tra độ dài).
// double hash: khai báo 2 struct với mod prime (1e9+7, 1e9+9) → chống hack.
```

## Trie (mảng)

**Mục đích:** Lưu tập chuỗi, tìm prefix/count — autocomplete, kiểm tra tồn tại.

**Ý tưởng / Observation:**
- Mảng tĩnh `nxt[v][26]` với `v` = số node ≤ tổng độ dài chuỗi — $O(1)$/phép, không pointer.
- Bài "đếm số chuỗi có prefix p" → duyệt node của p, trả `cnt[node]`.
- Nếu chữ thường/không thường → `alpha = 26`, `first = 'a'` — đổi theo input.

**Điều kiện sử dụng:**
- Alphabet nhỏ ($\alpha \le 26$); nếu $|\Sigma|$ lớn → map / hash per node.

**Độ phức tạp:**
- Time: $O(L)$ insert/search (L = độ dài chuỗi)
- Space: $O(nodes \times α)$

**Dependency:** template.cpp

```cpp
// id: trie
// cnt[v] = số chuỗi ĐI QUA node v (prefix count); end[v] = số chuỗi KẾT THÚC tại v
// instance ~10MB (mảng tĩnh) → khai báo global
struct Trie {
    int nxt[100005][26], cnt[100005], end[100005], nnode;
    Trie() : nnode(1) {
        memset(nxt, 0, sizeof nxt);
        memset(cnt, 0, sizeof cnt);
        memset(end, 0, sizeof end);
    }
    int add(const string& s) {  // trả số lần chuỗi này đã xuất hiện (trước khi thêm)
        int v = 0;
        for (char c : s) {
            int& u = nxt[v][c - 'a'];
            if (!u) u = nnode++;
            v = u;
            cnt[v]++;  // tăng MỌI node trên đường đi → prefix count đúng
        }
        return end[v]++;
    }
    int countPrefix(const string& s) {  // số chuỗi có prefix s (kể cả bằng đúng s)
        int v = 0;
        for (char c : s) {
            if (!nxt[v][c - 'a']) return 0;
            v = nxt[v][c - 'a'];
        }
        return cnt[v];
    }
    int countExact(const string& s) {  // số chuỗi bằng đúng s
        int v = 0;
        for (char c : s) {
            if (!nxt[v][c - 'a']) return 0;
            v = nxt[v][c - 'a'];
        }
        return end[v];
    }
};
```

## Suffix array + LCP

**Mục đích:** Sắp thứ tự mọi hậu tố; chuỗi con chung lớn nhất 2 hậu tố — distinct substrings, chuỗi con chung, palindrome.

**Ý tưởng / Observation:**
- `sa[0]` = "" (suffix rỗng, sentinel `\0` cuối); sort $O(n \log n)$ bằng 2 key `(rank[i], rank[i+k])`.
- `lcp[i]` = LCP(sa[i], sa[i-1]) ($O(n)$ bằng Kawaguchi/ Kasai).
- **Distinct substrings:** $\frac{n(n+1)}{2} - \sum lcp[i]$.
- Bài "chuỗi con chung dài nhất của 2 mảng con" / "chuỗi con có mặt $\ge k$ lần" → suffix array.

**Điều kiện sử dụng:**
- Input `string` có sentinel `\0` (KACTL tự thêm). Alphabet `char` bất kỳ.

**Độ phức tạp:**
- Time: $O(n \log n)$ build sa, $O(n)$ lcp
- Space: $O(n)$

**Dependency:** template.cpp

```cpp
// id: suffixarray
struct SuffixArray {
    string s;
    vi sa, lcp;
    SuffixArray(const string& str) : s(str) {
        s += '\0';  // sentinel
        int n = sz(s);
        sa.resize(n);
        iota(all(sa), 0);
        vi rank(n), tmp(n);
        rep(i, 0, n) rank[i] = s[i];
        for (int k = 1; k < n; k *= 2) {
            auto cmp = [&](int i, int j) {
                if (rank[i] != rank[j]) return rank[i] < rank[j];
                int ri = i + k < n ? rank[i + k] : -1;
                int rj = j + k < n ? rank[j + k] : -1;
                return ri < rj;
            };
            sort(all(sa), cmp);
            tmp[sa[0]] = 0;
            rep(i, 1, n) tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);
            rank = tmp;
            if (rank[sa[n - 1]] == n - 1) break;
        }
        sa.erase(sa.begin());  // bỏ sentinel
        buildLcp();
    }
    void buildLcp() {  // Kasai
        int n = sz(s) - 1;
        vi inv(n);
        rep(i, 0, n) inv[sa[i]] = i;
        lcp.assign(n, 0);
        int h = 0;
        rep(i, 0, n) {
            int r = inv[i];
            if (r > 0) {
                int j = sa[r - 1];
                while (i + h < n && j + h < n && s[i + h] == s[j + h]) ++h;
                lcp[r] = h;
                if (h) --h;
            }
        }
    }
    // distinct substrings = n*(n+1)/2 - Σ lcp
    // longest common substring of s1,s2: concat s1 + '#' + sa của nó → dùng với s2.
};
```

## Aho-Corasick

**Mục đích:** Tìm TẤT CẢ pattern cùng lúc trong text — Q pattern, tổng độ dài M, text N.

**Ý tưởng / Observation:**
- Trie + fail link (như KMP prefix func) → duyệt text 1 lần, mỗi node 1 bước.
- `find(s, patterns)` trả về danh sách `(k, endpos)` — pattern k kết thúc ở endpos.
- Bài "đếm số lần mỗi pattern xuất hiện" / "chuỗi con KHÔNG chứa pattern nào" → Aho.

**Điều kiện sử dụng:**
- Alphabet $\alpha = 26$ (đổi `first` nếu chữ thường); $M \lesssim 10^6$. Pattern trùng lặp OK.

**Độ phức tạp:**
- Time: $O(M \times α + N \times matches)$
- Space: $O(M \times α)$

**Dependency:** template.cpp

```cpp
// id: aho
struct AhoCorasick {
    enum { alpha = 26, first = 'A' };
    struct Node {
        int next[alpha], link = 0, out = 0;
        Node() { memset(next, 0, sizeof next); }
    };
    vector<Node> trie = vector<Node>(1);  // node 0 = root
    void add(const string& s, int idx = 0) {
        int v = 0;
        for (char c : s) {
            int u = trie[v].next[c - first];  // copy — KHÔNG giữ reference qua emplace_back
            if (!u) {
                u = sz(trie);
                trie[v].next[c - first] = u;  // ghi index TRƯỚC khi thêm node
                trie.emplace_back();
            }
            v = u;
        }
        trie[v].out = idx + 1;  // đánh dấu pattern idx (out != 0 → pattern kết thúc tại node)
    }
    void build() {  // fail link BFS
        queue<int> q;
        for (int& c : trie[0].next)
            if (c) q.push(c);
            else c = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int c = 0; c < alpha; ++c) {
                int& u = trie[v].next[c];
                if (u) {
                    trie[u].link = trie[trie[v].link].next[c];
                    q.push(u);
                } else
                    u = trie[trie[v].link].next[c];
            }
        }
    }
    // tìm (patternIdx, endPos) trong s — patternIdx = thứ tự thêm (0-based)
    vector<pii> find(const string& s) {
        vector<pii> res;
        int v = 0;
        rep(i, 0, sz(s)) {
            v = trie[v].next[s[i] - first];
            for (int u = v; u; u = trie[u].link)
                if (trie[u].out) res.push_back({trie[u].out - 1, i});
        }
        return res;
    }
};
```

## Min rotation (Booth)

**Mục đích:** Chuỗi xoay LEXICOGRAPHIC nhỏ nhất (vòng tròn) — tối ưu hóa chuỗi vòng, so sánh chuỗi xoay.

**Ý tưởng / Observation:**
- Booth: 2 con trỏ `i, j`, step `k` — so sánh ký tự, nhảy `i/j` khi mất thế. $O(n)$.
- Bài "chuỗi con nhỏ nhất của chuỗi vòng" / "xếp chuỗi vòng sao lexicographically min" → min rotation.

**Điều kiện sử dụng:**
- `s` là chuỗi thường (không cần `\0`). Kết quả = index bắt đầu (0-based).

**Độ phức tạp:**
- Time: $O(n)$
- Space: $O(n)$ (hoặc $O(1)$ nếu dùng 2 con trỏ trong string)

**Dependency:** template.cpp

```cpp
// id: minrotation
int minRotation(const string& s) {
    int n = sz(s), i = 0, j = 1, k = 0;
    while (i < n && j < n && k < n) {
        char a = s[(i + k) % n], b = s[(j + k) % n];
        if (a == b) ++k;
        else {
            if (a > b) i = i + k + 1;
            else j = j + k + 1;
            if (i == j) ++j;
            k = 0;
        }
    }
    return min(i, j);
}
// chuỗi xoay nhỏ nhất: s.substr(minRotation(s)) + s.substr(0, minRotation(s));
```
