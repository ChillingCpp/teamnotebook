//@ ids: point, onsegment, linedist, seginter, hull, hulldiameter, polyarea, inpolygon, closestpair, circle, linehull, kdtree
// Test ngẫu nhiên (seed cố định) — so mọi phép với brute force / công thức độc lập.
// File này test: point, onsegment, linedist, seginter, hull, hulldiameter, polyarea.
// Ids còn lại (inpolygon, closestpair, circle, linehull, kdtree) test ở geometry2.cpp.
int fails = 0;
void check(bool ok, const char* msg) { if (!ok) { ++fails; printf("FAIL: %s\n", msg); } }
mt19937_64 rng(20261006);
int rnd(int l, int r) { return int(rng() % (unsigned)(r - l + 1)) + l; }
const D PI = acosl(-1.0L);

// ---------- tham chiếu độc lập (exact, ll) ----------
ll xcr(ll ax, ll ay, ll bx, ll by) { return ax * by - ay * bx; }
ll dist2ll(Pl a, Pl b) { ll dx = a.x - b.x, dy = a.y - b.y; return dx * dx + dy * dy; }
int refSide(Pl a, Pl b, Pl p) {  // không epsilon — toạ độ nguyên
    ll v = xcr(b.x - a.x, b.y - a.y, p.x - a.x, p.y - a.y);
    return (v > 0) - (v < 0);
}
bool refOnSeg(Pl a, Pl b, Pl p) {
    if (refSide(a, b, p)) return false;
    return min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x) &&
           min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y);
}
bool refSegInter(Pl p1, Pl e1, Pl p2, Pl e2) {  // tham số t,s — thuật toán khác notebook
    ll dx1 = e1.x - p1.x, dy1 = e1.y - p1.y, dx2 = e2.x - p2.x, dy2 = e2.y - p2.y;
    ll nx = p2.x - p1.x, ny = p2.y - p1.y;
    ll den = xcr(dx1, dy1, dx2, dy2);
    if (den) {  // p1 + t*d1 = p2 + s*d2 → t = [n d2]/den, s = [n d1]/den
        ll t = xcr(nx, ny, dx2, dy2), s = xcr(nx, ny, dx1, dy1);
        if (den > 0) return 0 <= t && t <= den && 0 <= s && s <= den;
        return den <= t && t <= 0 && den <= s && s <= 0;
    }
    if (xcr(nx, ny, dx1, dy1)) return false;  // song song, không trùng hàng
    ll lo1 = min(p1.x, e1.x), hi1 = max(p1.x, e1.x);
    ll lo2 = min(p2.x, e2.x), hi2 = max(p2.x, e2.x);
    if (!dx1) {  // d1 là đường dọc → chiếu trục y
        lo1 = min(p1.y, e1.y); hi1 = max(p1.y, e1.y);
        lo2 = min(p2.y, e2.y); hi2 = max(p2.y, e2.y);
    }
    return lo1 <= hi2 && lo2 <= hi1;
}
vector<Pl> genNo3(int n, int C) {  // điểm distinct, không 3 thẳng hàng
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

int main() {
    // ---------- point ----------
    rep(tt, 0, 400) {
        ll ax = rnd(-999, 999), ay = rnd(-999, 999), bx = rnd(-999, 999), by = rnd(-999, 999);
        Pl a(ax, ay), b(bx, by), c(rnd(-999, 999), rnd(-999, 999));
        check(a.cross(b) == ax * by - ay * bx, "point.cross");
        check(a.cross(b) == -b.cross(a), "point.cross-anti");
        check(a.dot(b) == ax * bx + ay * by, "point.dot");
        check(a.perp().dot(b) == a.cross(b), "point.perp");
        check(a.cross(b, c) == xcr(b.x - ax, b.y - ay, c.x - ax, c.y - ay), "point.cross3");
        check(a.dist2() == ax * ax + ay * ay, "point.dist2");
        check((a == b) == (tie(ax, ay) == tie(bx, by)), "point.eq");
        check((a < b) == (tie(ax, ay) < tie(bx, by)), "point.less");
        check(a + b == Pl(ax + bx, ay + by) && a - b == Pl(ax - bx, ay - by), "point.addsub");
        if (ax % 7 == 0 && ay % 7 == 0)
            check(a / 7 == Pl(ax / 7, ay / 7) && (a / 7) * 7 == a, "point.muldiv");
    }
    rep(tt, 0, 300) {
        Pd a(rnd(-100, 100), rnd(-100, 100));
        if (a.x == 0 && a.y == 0) continue;
        check(fabs(a.dist() - sqrtl((D)(a.x * a.x + a.y * a.y))) < 1e-9, "point.dist");
        check(fabs(a.unit().dist() - 1) < 1e-9, "point.unit");
        check(fabs(a.angle() - atan2((D)a.y, (D)a.x)) < 1e-15, "point.angle");
        Pd r = a.rotate(PI / 2);  // xoay 90° = perp
        check(fabs(r.x + a.y) < 1e-9 && fabs(r.y - a.x) < 1e-9, "point.rotate90");
        check(fabs(a.rotate(0.37).dist() - a.dist()) < 1e-9, "point.rotate-len");
    }

    // ---------- onsegment ----------
    rep(tt, 0, 500) {
        Pl s(rnd(-50, 50), rnd(-50, 50)), e(rnd(-50, 50), rnd(-50, 50)), p(rnd(-50, 50), rnd(-50, 50));
        int typ = rnd(0, 4);  // ép các vị trí "interesting"
        if (typ == 0) p = s;
        else if (typ == 1) p = e;
        else if (typ == 2) p = s + (e - s);            // cùng hàng, ngoài đoạn (2 đầu mút)
        else if (typ == 3) p = s - (e - s);            // cùng hàng, ngoài phía sau
        int gs = sideOf(Pd(s.x, s.y), Pd(e.x, e.y), Pd(p.x, p.y));
        bool go = onSegment(Pd(s.x, s.y), Pd(e.x, e.y), Pd(p.x, p.y));
        check(gs == refSide(s, e, p), "onsegment.side");
        check(go == refOnSeg(s, e, p), "onsegment.on");
    }

    // ---------- linedist ----------
    rep(tt, 0, 400) {
        Pl A(rnd(-60, 60), rnd(-60, 60)), B(rnd(-60, 60), rnd(-60, 60)), P(rnd(-60, 60), rnd(-60, 60));
        if (A == B) continue;
        Pd a(A.x, A.y), b(B.x, B.y), p(P.x, P.y);
        D gl = lineDist(a, b, p);
        D want = (D)llabs(xcr(B.x - A.x, B.y - A.y, P.x - A.x, P.y - A.y)) / (b - a).dist();
        // NOTE: doc nói |cross|/|ab| nhưng code trả SIGNED distance → chỉ assert độ lớn
        check(fabs(gl) - want <= 1e-9 * (1 + want), "linedist.magnitude");
        check((fabs(gl) < 1e-12) == (refSide(A, B, P) == 0), "linedist.zero");
        D vx = b.x - a.x, vy = b.y - a.y;
        D t = ((p.x - a.x) * vx + (p.y - a.y) * vy) / (vx * vx + vy * vy);
        t = max((D)0, min((D)1, t));  // chiếu vuông góc, clamp về đoạn
        D ws = sqrtl((p.x - a.x - t * vx) * (p.x - a.x - t * vx) +
                     (p.y - a.y - t * vy) * (p.y - a.y - t * vy));
        Pd s2 = a, e2 = b;  // lvalues cho segDist (ký hiệu non-const)
        D gs = segDist(s2, e2, p);
        check(fabs(gs - ws) <= 1e-9 * (1 + ws), "linedist.seg");
        check(gs <= min((p - a).dist(), (p - b).dist()) + 1e-9, "linedist.seg-endpoints");
        // đoạn ⊂ đường thẳng ⇒ khoảng cách tới đoạn >= khoảng cách tới đường
        check(gs + 1e-9 >= fabs(gl), "linedist.seg-ge-line");
    }

    // ---------- seginter ----------
    check(segInter(Pd(0, 0), Pd(10, 0), Pd(5, 0), Pd(5, 5)), "seginter.tjunction");
    check(segInter(Pd(0, 0), Pd(10, 0), Pd(10, 0), Pd(0, 10)), "seginter.cross");
    check(segInter(Pd(0, 0), Pd(10, 0), Pd(5, 0), Pd(15, 0)), "seginter-overlap");
    check(segInter(Pd(0, 0), Pd(10, 0), Pd(10, 0), Pd(10, 3)), "seginter.touch");
    check(!segInter(Pd(0, 0), Pd(10, 0), Pd(0, 1), Pd(10, 1)), "seginter.parallel");
    check(!segInter(Pd(0, 0), Pd(3, 0), Pd(5, 0), Pd(9, 0)), "seginter.collinear-disjoint");
    rep(tt, 0, 600) {
        Pl s1(rnd(-30, 30), rnd(-30, 30)), e1(rnd(-30, 30), rnd(-30, 30));
        Pl s2(rnd(-30, 30), rnd(-30, 30)), e2(rnd(-30, 30), rnd(-30, 30));
        int f = rnd(0, 4);  // ép trường hợp đặc biệt
        if (f == 0) e2 = s1;                       // trùng đầu mút
        else if (f == 1) { s2 = s1; e2 = e1; }     // trùng hẳn đoạn
        else if (f == 2) {                          // chung 1 phần (thẳng hàng khi may)
            s2 = s1;
            e2 = Pl((s1.x + e1.x) / 2, (s1.y + e1.y) / 2);
            if (e2 == s2) e2 = e1;
        }
        if (s1 == e1) continue;
        bool got = segInter(Pd(s1.x, s1.y), Pd(e1.x, e1.y), Pd(s2.x, s2.y), Pd(e2.x, e2.y));
        check(got == refSegInter(s1, e1, s2, e2), "seginter.random");
    }
    rep(tt, 0, 400) {  // lineInter — tham chiếu: hệ ax+by=c (Cramer), khác công thức notebook
        Pl A(rnd(-40, 40), rnd(-40, 40)), B(rnd(-40, 40), rnd(-40, 40));
        Pl C(rnd(-40, 40), rnd(-40, 40)), E(rnd(-40, 40), rnd(-40, 40));
        if (A == B || C == E) continue;
        ll den = xcr(B.x - A.x, B.y - A.y, E.x - C.x, E.y - C.y);
        auto got = lineInter(Pd(A.x, A.y), Pd(B.x, B.y), Pd(C.x, C.y), Pd(E.x, E.y));
        if (den == 0) { check(!got, "seginter.line-parallel"); continue; }
        check((bool)got, "seginter.line-hasinter");
        if (!got) continue;
        D A1 = (D)(B.y - A.y), B1 = (D)(A.x - B.x), C1 = A1 * A.x + B1 * A.y;
        D A2 = (D)(E.y - C.y), B2 = (D)(C.x - E.x), C2 = A2 * C.x + B2 * C.y;
        D det = A1 * B2 - A2 * B1;
        D rx = (C1 * B2 - C2 * B1) / det, ry = (A1 * C2 - A2 * C1) / det;
        check(fabs(got->x - rx) <= 1e-9 * (1 + fabs(rx)), "seginter.line-x");
        check(fabs(got->y - ry) <= 1e-9 * (1 + fabs(ry)), "seginter.line-y");
    }

    // ---------- hull ----------
    rep(tt, 0, 60) {
        vector<Pl> pts = genNo3(rnd(3, 40), 25);
        if (sz(pts) < 3) continue;
        vector<Pl> h = hull(pts);
        check(sz(h) >= 3, "hull.size");
        set<Pl> inPts(all(pts));
        for (auto& q : h) check(inPts.count(q) == 1, "hull.vertex-in-input");
        int hn = sz(h);
        rep(i, 0, hn) {  // ccw + lồi chặt (cross > 0)
            Pl a = h[i], b = h[(i + 1) % hn], c = h[(i + 2) % hn];
            check(xcr(b.x - a.x, b.y - a.y, c.x - b.x, c.y - b.y) > 0, "hull.ccw-convex");
        }
        set<Pl> inH(all(h));
        for (auto& q : pts) rep(i, 0, hn) {  // hull chứa mọi điểm; không-hull-point ở trong nghiêm ngặt
            Pl a = h[i], b = h[(i + 1) % hn];
            ll s = xcr(b.x - a.x, b.y - a.y, q.x - a.x, q.y - a.y);
            check(s >= 0, "hull.contains");
            if (!inH.count(q)) check(s > 0, "hull.strict-interior");
        }
    }
    {  // trường hợp suy biến
        vector<Pl> col;
        rep(i, 0, 8) col.push_back(Pl(3 * i - 10, 7 * i - 5));  // 8 điểm thẳng hàng
        vector<Pl> hc = hull(col);
        check(sz(hc) == 2, "hull.collinear-size2");
        if (sz(hc) == 2)
            check(hc[0] == *min_element(all(col)) && hc[1] == *max_element(all(col)),
                  "hull.collinear-extremes");
        check(hull(vector<Pl>{Pl(1, 1), Pl(2, 2)}).empty(), "hull.two-points");
        check(hull(vector<Pl>{Pl(1, 1), Pl(1, 1), Pl(2, 2)}).empty(), "hull-distinct-lt3");
    }

    // ---------- hulldiameter ----------
    rep(tt, 0, 80) {
        vector<Pl> pts = genNo3(rnd(3, 30), 25);
        if (sz(pts) < 3) continue;
        vector<Pl> h = hull(pts);
        if (sz(h) < 3) continue;
        pair<int, int> pr = hullDiameter(h);
        int di = pr.first, dj = pr.second;
        check(0 <= di && di < sz(h) && 0 <= dj && dj < sz(h), "hulldiameter.range");
        ll best = 0, bestPts = 0;
        rep(a, 0, sz(h)) rep(b, 0, sz(h)) best = max(best, dist2ll(h[a], h[b]));
        rep(a, 0, sz(pts)) rep(b, 0, sz(pts)) bestPts = max(bestPts, dist2ll(pts[a], pts[b]));
        check(dist2ll(h[di], h[dj]) == best, "hulldiameter");
        check(best == bestPts, "hulldiameter-vs-set");  // đường kính hull = đường kính tập điểm
    }
    {
        vector<Pl> one{Pl(2, 3)}, two{Pl(0, 0), Pl(3, 4)};
        check(hullDiameter(one) == make_pair(0, 0), "hulldiameter.sz1");
        check(hullDiameter(two) == make_pair(0, 1), "hulldiameter.sz2");
    }

    // ---------- polyarea ----------
    rep(tt, 0, 60) {  // đa giác lồi: so với phân tích tam giác từ h[0]
        vector<Pl> pts = genNo3(rnd(3, 30), 25);
        if (sz(pts) < 3) continue;
        vector<Pl> h = hull(pts);
        if (sz(h) < 3) continue;
        D want = 0;
        rep(i, 1, sz(h) - 1)
            want += (D)llabs(xcr(h[i].x - h[0].x, h[i].y - h[0].y,
                                 h[i + 1].x - h[0].x, h[i + 1].y - h[0].y)) / 2;
        vector<Pd> v;
        for (auto& q : h) v.push_back(Pd(q.x, q.y));
        check(fabs(polygonArea(v) - want) <= 1e-9 * (1 + want), "polyarea.convex");
        reverse(all(v));  // cw → cùng diện tích (abs)
        check(fabs(polygonArea(v) - want) <= 1e-9 * (1 + want), "polyarea.reversed");
    }
    {  // hình vuông đơn vị + đa giác x-monotone (đơn giản, không lồi)
        vector<Pd> sq{Pd(0, 0), Pd(1, 0), Pd(1, 1), Pd(0, 1)};
        check(fabs(polygonArea(sq) - 1.0) < 1e-12, "polyarea.square");
    }
    rep(tt, 0, 40) {
        int k = rnd(3, 12);
        vector<int> xs, ty(k), by(k);
        int x = 0;
        rep(i, 0, k) { x += rnd(1, 8); xs.push_back(x); }
        rep(i, 0, k) { ty[i] = rnd(1, 30); by[i] = rnd(1, 30); }
        vector<Pd> v;
        rep(i, 0, k) v.push_back(Pd(xs[i], ty[i]));             // chuỗi trên: x tăng, y > 0
        for (int i = k - 1; i >= 0; --i) v.push_back(Pd(xs[i], -by[i]));  // chuỗi dưới: x giảm
        D want = 0;  // phân tích hình thang theo dải [xs[i], xs[i+1]]
        rep(i, 0, k - 1)
            want += (D)(xs[i + 1] - xs[i]) * ((ty[i] + by[i]) + (ty[i + 1] + by[i + 1])) / 2;
        check(fabs(polygonArea(v) - want) <= 1e-9 * (1 + want), "polyarea.monotone");
    }

    if (fails) { printf("geometry: %d loi\n", fails); return 1; }
    printf("geometry: OK\n");
    return 0;
}
