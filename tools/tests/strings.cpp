//@ ids: kmp, zfunc, manacher, hash, trie, suffixarray, aho, minrotation
// Test ngẫu nhiên (seed cố định) so từng snippet chuỗi với brute force thuần.
int fails = 0;
void check(bool ok, const char* msg) {
    if (!ok) { ++fails; printf("FAIL: %s\n", msg); }
}

// ---------- tham chiếu brute ----------
vi refPi(const string& s) {
    vi pi(sz(s));
    rep(i, 0, sz(s)) {
        for (int k = i; k >= 1; --k)
            if (s.substr(0, k) == s.substr(i + 1 - k, k)) { pi[i] = k; break; }
    }
    return pi;
}
vi refSearch(const string& t, const string& p) {
    vi res;
    rep(i, 0, sz(t) - sz(p) + 1) if (t.compare(i, sz(p), p) == 0) res.push_back(i);
    return res;
}
vi refZ(const string& s) {
    vi z(sz(s));
    rep(i, 1, sz(s)) {
        int c = 0;
        while (i + c < sz(s) && s[c] == s[i + c]) ++c;
        z[i] = c;
    }
    return z;
}
pair<vi, vi> refManacher(const string& s) {
    int n = sz(s);
    vi p0(n), p1(n);
    rep(i, 0, n) {
        int r = 1;
        while (i - r >= 0 && i + r < n && s[i - r] == s[i + r]) ++r;
        p1[i] = r;
    }
    rep(i, 0, n) {
        int r = 0;
        while (i - r - 1 >= 0 && i + r < n && s[i - r - 1] == s[i + r]) ++r;
        p0[i] = r;
    }
    return {p0, p1};
}
string refMinRotation(const string& s) {
    string best = s;
    rep(k, 1, sz(s)) best = min(best, s.substr(k) + s.substr(0, k));
    return best;
}
Trie gTrie;  // ~11MB static → khai báo global (không đặt trong main)

int main() {
    mt19937_64 rng(20261003);
    auto rnd = [&](int l, int r) { return int(rng() % (r - l + 1)) + l; };
    auto randStr = [&](int len, int alpha) {
        string s;
        rep(i, 0, len) s += char('a' + rnd(0, alpha - 1));
        return s;
    };
    auto randUp = [&](int len, int alpha) {
        string s;
        rep(i, 0, len) s += char('A' + rnd(0, alpha - 1));
        return s;
    };

    // ---------- kmp ----------
    rep(tt, 0, 300) {
        int alpha = rnd(1, 4);
        string t = randStr(rnd(0, 60), alpha);
        string p = randStr(rnd(1, 15), alpha);
        check(piFunction(t) == refPi(t), "kmp.pi-text");
        check(piFunction(p) == refPi(p), "kmp.pi-pattern");
        check(kmpSearch(t, p) == refSearch(t, p), "kmp.search");
    }

    // ---------- zfunc ----------
    rep(tt, 0, 300) {
        string s = randStr(rnd(0, 80), rnd(1, 4));
        check(zFunction(s) == refZ(s), "zfunc");
    }

    // ---------- manacher ----------
    rep(tt, 0, 300) {
        string s = randStr(rnd(0, 60), rnd(1, 3));
        auto [p0, p1] = manacher(s);
        auto [r0, r1] = refManacher(s);
        check(p0 == r0 && p1 == r1, "manacher");
    }

    // ---------- hash ----------
    rep(tt, 0, 100) {
        int n = rnd(1, 60);
        string s = randStr(n, rnd(1, 4));
        RollingHash rh(s);
        rep(l, 0, n + 1) check(rh.get(l, l) == 0, "hash.empty");
        rep(q, 0, 50) {
            int l1 = rnd(0, n - 1), l2 = rnd(0, n - 1);
            int len = rnd(1, n - max(l1, l2));
            bool eq = s.compare(l1, len, s, l2, len) == 0;
            ull h1 = rh.get(l1, l1 + len), h2 = rh.get(l2, l2 + len);
            if (eq) check(h1 == h2, "hash.equal");
            else check(h1 != h2, "hash.unequal");
        }
    }

    // ---------- trie ----------
    vector<string> corpus;
    rep(tt, 0, 60) {
        rep(i, 0, rnd(1, 15)) {
            string s = randStr(rnd(1, 8), rnd(1, 4));
            int want = 0;
            for (auto& w : corpus) want += (w == s);
            check(gTrie.add(s) == want, "trie.add-return");
            corpus.push_back(s);
        }
        rep(q, 0, 30) {
            string qr = randStr(rnd(1, 8), rnd(1, 4));
            if (!corpus.empty() && rnd(0, 1)) {
                const string& w = corpus[rnd(0, sz(corpus) - 1)];
                qr = w.substr(0, rnd(1, sz(w)));
            }
            int wp = 0, we = 0;
            for (auto& w : corpus) {
                if (sz(w) >= sz(qr) && w.compare(0, sz(qr), qr) == 0) ++wp;
                if (w == qr) ++we;
            }
            check(gTrie.countPrefix(qr) == wp, "trie.countPrefix");
            check(gTrie.countExact(qr) == we, "trie.countExact");
        }
    }

    // ---------- suffixarray ----------
    rep(tt, 0, 60) {
        int n = rnd(1, 60);
        string s = randStr(n, rnd(1, 4));
        SuffixArray sa(s);
        vi ord(n);
        iota(all(ord), 0);
        sort(all(ord), [&](int i, int j) { return s.substr(i) < s.substr(j); });
        check(sa.sa == ord, "suffixarray.sa");
        check(sz(sa.lcp) == n, "suffixarray.lcp-size");
        rep(i, 0, n) {
            int want = 0;
            if (i) {
                int x = ord[i], y = ord[i - 1];
                while (x + want < n && y + want < n && s[x + want] == s[y + want]) ++want;
            }
            check(sa.lcp[i] == want, "suffixarray.lcp");
        }
        set<string> subs;
        rep(i, 0, n) rep(j, i + 1, n + 1) subs.insert(s.substr(i, j - i));
        ll distinct = (ll)n * (n + 1) / 2;
        for (int v : sa.lcp) distinct -= v;
        check(distinct == (ll)sz(subs), "suffixarray.distinct");
    }

    // ---------- aho ----------
    // (add() đã fix: không giữ reference qua emplace_back — reserve dưới đây
    //  chỉ là tối ưu capacity, không còn bắt buộc để tránh UB)
    rep(tt, 0, 60) {
        int np = rnd(1, 8);
        vector<string> pats;
        while (sz(pats) < np) {
            string p = randUp(rnd(1, 5), rnd(1, 4));
            bool dup = false;
            for (auto& q : pats) dup |= (q == p);
            if (!dup) pats.push_back(p);
        }
        AhoCorasick ac;
        ac.trie.reserve(sz(pats) * 6 + 8);  // tránh reallocation (xem NOTE ở trên)
        rep(k, 0, sz(pats)) ac.add(pats[k], k);
        ac.build();
        string text = randUp(rnd(1, 60), rnd(1, 4));
        auto got = ac.find(text);
        vector<pii> want;
        rep(k, 0, sz(pats)) {
            int L = sz(pats[k]);
            rep(e, L - 1, sz(text)) if (text.compare(e - L + 1, L, pats[k]) == 0) want.push_back({k, e});
        }
        sort(all(got));
        sort(all(want));
        check(got == want, "aho.find");
    }

    // ---------- minrotation ----------
    rep(tt, 0, 300) {
        string s = randStr(rnd(1, 50), rnd(1, 4));
        int k = minRotation(s);
        check(0 <= k && k < sz(s), "minrotation.range");
        check(s.substr(k) + s.substr(0, k) == refMinRotation(s), "minrotation");
    }

    if (fails) { printf("strings: %d loi\n", fails); return 1; }
    printf("strings: OK\n");
    return 0;
}
