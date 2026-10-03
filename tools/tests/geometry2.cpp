//@ ids: point, onsegment, hull, inpolygon, closestpair, circle, linehull, kdtree
// Randomized tests (fixed seed) - every call checked against brute force / an
// independent formula. File 2/2: inpolygon, inHull, closestpair, circle,
// linehull+extrVertex, kdtree. (File 1 geometry.cpp: point, onsegment,
// linedist, seginter, hull, hulldiameter, polyarea.)
int fails = 0;
void check(bool ok, const char* msg) { if (!ok) { ++fails; printf("FAIL: %s\n", msg); } }
mt19937_64 rng(20261008);
int rnd(int l, int r) { return int(rng() % (unsigned)(r - l + 1)) + l; }

// ---------- independent references (exact, ll) ----------
ll xcr(ll ax, ll ay, ll bx, ll by) { return ax * by - ay * bx; }
ll dist2ll(Pl a, Pl b) { ll dx = a.x - b.x, dy = a.y - b.y; return dx * dx + dy * dy; }
int refSide(Pl a, Pl b, Pl p) {
    ll v = xcr(b.x - a.x, b.y - a.y, p.x - a.x, p.y - a.y);
    return (v > 0) - (v < 0);
}
bool refOnSeg(Pl a, Pl b, Pl p) {
    if (refSide(a, b, p)) return false;
    return min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x) &&
           min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y);
}
vector<Pl> genNo3(int n, int C) {
    vector<Pl> p;
    for (int g = 0; sz(p) < n && g < 20000; ++g) {
        Pl q(rnd(-C, C), rnd(-C, C));
        bool ok = true;
        for (auto& r : p) if (r == q) ok = false;
        for (int i = 0; ok && i < sz(p); ++i)
            for (int j = i + 1; ok && j < sz(p); ++j) ok = refSide(p[i], p[j], q) != 0;
        if (ok) p.push_back(q);
    }
    return p;
}
// inpolygon reference: double every coordinate (p is on the 0.5 grid) so the
// even-odd test and the boundary test are exact in ll arithmetic
int refInPoly(const vector<Pl>& v, Pd p) {
    ll px = llround(p.x * 2), py = llround(p.y * 2);
    int n = sz(v);
    vector<Pl> q(n);
    rep(i, 0, n) q[i] = Pl(2 * v[i].x, 2 * v[i].y);
    rep(i, 0, n) if (refOnSeg(q[i], q[(i + 1) % n], Pl(px, py))) return 2;
    bool in = false;
    rep(i, 0, n) {
        Pl a = q[i], b = q[(i + 1) % n];
        if ((a.y > py) != (b.y > py)) {  // horizontal ray, half-open rule (same as notebook)
            ll dy = b.y - a.y;           // != 0
            ll lhs = a.x * dy + (py - a.y) * (b.x - a.x);  // = x_cross * dy
            if (dy > 0 ? lhs > px * dy : lhs < px * dy) in = !in;
        }
    }
    return in;
}
// inHull reference: ccw convex core -> inside/on edge iff all cross >= 0
bool refInHull(const vector<Pd>& h, Pd p) {
    rep(i, 0, sz(h)) if (h[i].cross(h[(i + 1) % sz(h)], p) < 0) return false;
    return true;
}
// segment of intersection of line (A,B) with convex core h - edge scan, exact ll
vector<Pd> refLineClip(const vector<Pl>& h, Pl A, Pl B) {
    int n = sz(h);
    vector<ll> s(n);
    bool pos = false, neg = false;
    rep(i, 0, n) {
        s[i] = xcr(B.x - A.x, B.y - A.y, h[i].x - A.x, h[i].y - A.y);
        pos |= s[i] > 0; neg |= s[i] < 0;
    }
    if (!pos && !neg) return {};  // line stays on one side of the core
    vector<Pd> pts;
    rep(i, 0, n) {
        int j = (i + 1) % n;
        if (s[i] == 0) pts.push_back(Pd(h[i].x, h[i].y));       // vertex on the line
        if ((s[i] > 0 && s[j] < 0) || (s[i] < 0 && s[j] > 0)) {  // edge crossing the line
            D t = (D)s[i] / (D)(s[i] - s[j]);
            pts.push_back(Pd(h[i].x, h[i].y) +
                          (Pd(h[j].x, h[j].y) - Pd(h[i].x, h[i].y)) * t);
        }
    }
    Pd a(A.x, A.y), dir = Pd(B.x, B.y) - a;
    int lo = 0, hi = 0;  // intersection segment = extreme projections on the line
    rep(k, 1, sz(pts)) {
        if ((pts[k] - a).dot(dir) < (pts[lo] - a).dot(dir)) lo = k;
        if ((pts[k] - a).dot(dir) > (pts[hi] - a).dot(dir)) hi = k;
    }
    if (pts.empty()) return {};  // all vertices strictly on one side: no intersection
    return {pts[lo], pts[hi]};
}
// map a lineHull result to the actual intersection points on the line
vector<Pd> lineHullPts(array<int, 2> r, const vector<Pl>& h, Pl A, Pl B) {
    int n = sz(h);
    if (r[0] == -1) return {};
    if (r[1] == -1) return {Pd(h[r[0]].x, h[r[0]].y)};  // touches vertex r0 only
    if (r[0] == r[1]) {  // line coincides with edge (r0, r0+1) -> that whole edge
        Pl a = h[r[0]], b = h[(r[0] + 1) % n];
        return {Pd(a.x, a.y), Pd(b.x, b.y)};
    }
    vector<Pd> out;
    for (int idx : {r[0], r[1]}) {  // point on edge (idx, idx+1)
        int j = (idx + 1) % n;
        ll s0 = xcr(B.x - A.x, B.y - A.y, h[idx].x - A.x, h[idx].y - A.y);
        ll s1 = xcr(B.x - A.x, B.y - A.y, h[j].x - A.x, h[j].y - A.y);
        Pd pi(h[idx].x, h[idx].y), pj(h[j].x, h[j].y);
        if (s0 == 0) out.push_back(pi);
        else if (s1 == 0) out.push_back(pj);
        else out.push_back(pi + (pj - pi) * ((D)s0 / (D)(s0 - s1)));
    }
    return out;
}
// normalize before comparing: merge near-equal points, sort by (x, y)
vector<Pd> canon(vector<Pd> v) {
    if (sz(v) == 2 && (v[0] - v[1]).dist() < 1e-7) v.pop_back();
    sort(all(v), [](Pd x, Pd y) { return tie(x.x, x.y) < tie(y.x, y.y); });
    return v;
}

int main() {
    // ---------- inpolygon: simple x-monotone polygon (may be concave) ----------
    rep(tt, 0, 45) {
        int k = rnd(4, 9);
        vector<int> xs(k), ty(k), by(k);
        int x = 0;
        rep(i, 0, k) { x += rnd(1, 7); xs[i] = x; }
        rep(i, 0, k) { ty[i] = rnd(1, 25); by[i] = rnd(1, 25); }
        vector<Pl> v;
        rep(i, 0, k) v.push_back(Pl(xs[i], ty[i]));                       // top chain (y > 0)
        for (int i = k - 1; i >= 0; --i) v.push_back(Pl(xs[i], -by[i]));  // bottom chain (y < 0)
        vector<Pd> pv;
        for (auto& q : v) pv.push_back(Pd(q.x, q.y));
        rep(q, 0, 30) {  // integer + half-integer queries
            Pd p(rnd(-8, xs[k - 1] + 8), rnd(-30, 30));
            check(inpolygon(pv, p) == refInPoly(v, p), "inpolygon.mono-int");
            Pd p2(rnd(-8, xs[k - 1] + 8) + 0.5, rnd(-30, 30) + 0.5);
            check(inpolygon(pv, p2) == refInPoly(v, p2), "inpolygon.mono-half");
        }
        rep(i, 0, sz(v)) {  // vertices + edge midpoints: always on the boundary
            Pd vert(v[i].x, v[i].y);
            check(inpolygon(pv, vert) == 2, "inpolygon.vertex");
            Pl nx = v[(i + 1) % sz(v)];
            Pd mid((v[i].x + nx.x) / 2.0, (v[i].y + nx.y) / 2.0);
            check(refInPoly(v, mid) == 2, "inpolygon.mid-ref");
            check(inpolygon(pv, mid) == 2, "inpolygon.mid");
        }
    }
    // ---------- inpolygon + inHull on a convex hull (output of hull) ----------
    rep(tt, 0, 35) {
        vector<Pl> pts = genNo3(rnd(3, 22), 28);
        if (sz(pts) < 3) continue;
        vector<Pl> h = hull(pts);
        if (sz(h) < 3) continue;
        vector<Pd> hv;
        for (auto& q : h) hv.push_back(Pd(q.x, q.y));
        rep(q, 0, 30) {
            Pd p(rnd(-35, 35), rnd(-35, 35));
            check(inpolygon(hv, p) == refInPoly(h, p), "inpolygon.hull");
            check(inHull(hv, p) == refInHull(hv, p), "inHull.random");
        }
        rep(i, 0, sz(h)) {  // boundary points: vertex + edge midpoint
            Pl e = h[(i + 1) % sz(h)];
            Pd vert(h[i].x, h[i].y), mid((h[i].x + e.x) / 2.0, (h[i].y + e.y) / 2.0);
            check(inpolygon(hv, vert) == 2, "inpolygon.hull-vertex");
            check(inpolygon(hv, mid) == 2, "inpolygon.hull-mid");
            check(inHull(hv, vert), "inHull.vertex");
            check(inHull(hv, mid), "inHull.edge-mid");
        }
    }
    // ---------- closestpair ----------
    rep(tt, 0, 70) {
        int n = rnd(2, 110), mode = rnd(0, 3);
        vector<Pd> v;
        rep(i, 0, n) {
            if (mode == 0) v.push_back(Pd(rnd(-50, 50), rnd(-50, 50)));  // many duplicate coords
            else if (mode == 1) v.push_back(Pd(rnd(-50, 50) + 0.5, rnd(-50, 50) + 0.5));
            else if (mode == 2) v.push_back(Pd(rnd(-4, 4), rnd(-4, 4)));  // tiny cluster, many ties
            else v.push_back(Pd(i, rnd(-30, 30)));                         // x already sorted
        }
        pair<int, int> pr = closestPair(v);
        int i1 = pr.first, j1 = pr.second;
        check(0 <= i1 && i1 < n && 0 <= j1 && j1 < n && i1 != j1, "closestpair.idx");
        D want = 1e30;
        rep(a, 0, n) rep(b, a + 1, n) want = min(want, (v[a] - v[b]).dist());
        D got = (v[i1] - v[j1]).dist();
        check(fabs(got - want) <= 1e-9 * (1 + want), "closestpair.min");
    }
    // ---------- circle: circumCircle ----------
    rep(tt, 0, 200) {
        Pl a(rnd(-30, 30), rnd(-30, 30)), b(rnd(-30, 30), rnd(-30, 30)), c(rnd(-30, 30), rnd(-30, 30));
        ll cr = xcr(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y);
        auto got = circumCircle(Pd(a.x, a.y), Pd(b.x, b.y), Pd(c.x, c.y));
        if (cr == 0) { check(!got, "circle.circum-colinear"); continue; }
        check((bool)got, "circle.circum-has");
        if (!got) continue;
        Pd o = get<0>(*got), pa(a.x, a.y), pb(b.x, b.y), pc(c.x, c.y);
        D r = get<1>(*got);
        check(fabs((o - pa).dist() - r) <= 1e-9 * (1 + r), "circle.circum-ra");
        check(fabs((o - pa).dist() - (o - pb).dist()) <= 1e-9 * (1 + r), "circle.circum-ab");
        check(fabs((o - pa).dist() - (o - pc).dist()) <= 1e-9 * (1 + r), "circle.circum-ac");
        // cross-check: intersection of two perpendicular bisectors (Cramer)
        Pd m1 = (pa + pb) / 2, d1 = pb - pa, m2 = (pa + pc) / 2, d2 = pc - pa;
        D A1 = d1.x, B1 = d1.y, C1 = d1.x * m1.x + d1.y * m1.y;
        D A2 = d2.x, B2 = d2.y, C2 = d2.x * m2.x + d2.y * m2.y;
        D det = A1 * B2 - A2 * B1;
        D rx = (C1 * B2 - C2 * B1) / det, ry = (A1 * C2 - A2 * C1) / det;
        check(fabs(o.x - rx) <= 1e-8 * (1 + fabs(rx)) &&
              fabs(o.y - ry) <= 1e-8 * (1 + fabs(ry)), "circle.circum-cramer");
    }
    // ---------- circle: circleInter ----------
    rep(tt, 0, 300) {
        Pl c1(rnd(-30, 30), rnd(-30, 30)), c2(rnd(-30, 30), rnd(-30, 30));
        int r1 = rnd(1, 30), r2 = rnd(1, 30);
        ll d2 = dist2ll(c1, c2), s = r1 + r2, dd = llabs((ll)(r1 - r2));
        auto got = circleInter(Pd(c1.x, c1.y), (D)r1, Pd(c2.x, c2.y), (D)r2);
        check(sz(got) <= 2, "circle.inter-atmost2");
        if (d2 == 0) {  // same center: infinitely many (or zero) points, notebook returns {}
            check(got.empty(), "circle.inter-same-center");
            continue;
        }
        // compare SQUARED distances against integers: exact classification, no epsilon
        int want = (d2 > s * s || d2 < dd * dd) ? 0
                 : (d2 == s * s || d2 == dd * dd) ? 1 : 2;
        if (want == 0) { check(got.empty(), "circle.inter-none"); continue; }
        if (want == 2) check(sz(got) == 2, "circle.inter-count2");
        else {
            check(1 <= sz(got) && sz(got) <= 2, "circle.inter-count1");
            if (sz(got) == 2)  // tangent -> the two returned points must coincide
                check((got[0] - got[1]).dist() <= 1e-4, "circle.inter-tangent-close");
        }
        Pd p1(c1.x, c1.y), p2(c2.x, c2.y);
        for (auto& p : got) {
            check(fabs((p - p1).dist() - r1) <= 1e-6, "circle.inter-on1");
            check(fabs((p - p2).dist() - r2) <= 1e-6, "circle.inter-on2");
        }
    }
    // ---------- linehull: extrVertex + lineHull ----------
    rep(tt, 0, 250) {
        vector<Pl> pts = genNo3(rnd(3, 20), 20);
        if (sz(pts) < 3) continue;
        vector<Pl> h = hull(pts);  // hull() drops collinear: ccw, no 3 collinear
        if (sz(h) < 3) continue;
        vector<Pd> hv;
        for (auto& q : h) hv.push_back(Pd(q.x, q.y));
        Pl A(rnd(-30, 30), rnd(-30, 30)), B(rnd(-30, 30), rnd(-30, 30));
        if (!(A == B)) {
            array<int, 2> got = lineHull(Pd(A.x, A.y), Pd(B.x, B.y), hv);
            vector<Pd> want = canon(refLineClip(h, A, B));
            vector<Pd> g = canon(lineHullPts(got, h, A, B));
            check(sz(g) == sz(want), "linehull.size");
            if (sz(g) == sz(want))
                rep(m, 0, sz(g))
                    check((g[m] - want[m]).dist() <= 1e-6 * (1 + want[m].dist()), "linehull.point");
        }
        Pl dp(rnd(-10, 10), rnd(-10, 10));  // extrVertex = vertex with max projection on dir
        if (dp.x == 0 && dp.y == 0) continue;
        Pd dir(dp.x, dp.y);
        int gotv = extrVertex(hv, dir);
        check(0 <= gotv && gotv < sz(hv), "linehull.extr-range");
        D best = dir.dot(hv[0]);
        rep(i, 1, sz(hv)) best = max(best, dir.dot(hv[i]));
        check(fabs(dir.dot(hv[gotv]) - best) <= 1e-9 * (1 + fabs(best)), "linehull.extr-max");
    }
    // ---------- kdtree ----------
    rep(tt, 0, 40) {
        int n = rnd(1, 150);
        vector<Pd> v;
        if (rnd(0, 1)) rep(i, 0, n) v.push_back(Pd(rnd(-40, 40), rnd(-40, 40)));
        else rep(i, 0, n) v.push_back(Pd(rnd(-40, 40) + 0.5, rnd(-40, 40) + 0.5));
        KDTree kd(v);
        rep(q, 0, 10) {  // count points inside an axis-aligned rectangle
            int x1 = rnd(-45, 45), x2 = rnd(-45, 45), y1 = rnd(-45, 45), y2 = rnd(-45, 45);
            Pd lo(min(x1, x2), min(y1, y2)), hi(max(x1, x2), max(y1, y2));
            int want = 0;
            for (auto& p : v)
                if (lo.x <= p.x && p.x <= hi.x && lo.y <= p.y && p.y <= hi.y) ++want;
            check(kd.count(kd.root, lo, hi) == want, "kdtree.count");
        }
        rep(q, 0, 10) {  // nearest neighbor vs brute force O(n)
            Pd p(rnd(-55, 55), rnd(-55, 55));
            int want = 0;
            D bd = (v[0] - p).dist2();
            rep(i, 1, n) {
                D d = (v[i] - p).dist2();
                if (d < bd) bd = d, want = i;
            }
            int got = kd.nearest(p);
            check(0 <= got && got < n, "kdtree.nn-range");
            if (0 <= got && got < n)
                check(fabs((v[got] - p).dist2() - bd) <= 1e-9 * (1 + bd), "kdtree.nn");
        }
    }
    {  // empty / single point edge cases
        vector<Pd> empty;
        KDTree ke(empty);
        check(ke.nearest(Pd(1, 1)) == -1, "kdtree.empty-nn");
        check(ke.count(ke.root, Pd(-10, -10), Pd(10, 10)) == 0, "kdtree.empty-count");
        vector<Pd> one{Pd(3, 4)};
        KDTree k1(one);
        check(k1.nearest(Pd(0, 0)) == 0, "kdtree.one-nn");
        check(k1.count(k1.root, Pd(3, 4), Pd(3, 4)) == 1, "kdtree.one-count");
    }

    if (fails) { printf("geometry2: %d loi\n", fails); return 1; }
    printf("geometry2: OK\n");
    return 0;
}




