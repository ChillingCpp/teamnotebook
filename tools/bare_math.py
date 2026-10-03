# -*- coding: utf-8 -*-
"""Convert bare-prose math inequalities/superscripts (outside fences/code/$..$)
to LaTeX $...$ — exact full-line replacements, assert uniqueness."""
import sys
from pathlib import Path

NB = Path(__file__).resolve().parent.parent / "notebook"

# (file, old_line, new_line)
EDITS = [
    ("00-Template.md",
     "- `cin.tie(0)->sync_with_stdio(0)` rồi dùng `cin/cout` là đủ nhanh cho hầu hết bài (dưới ~2×10⁶ số).",
     "- `cin.tie(0)->sync_with_stdio(0)` rồi dùng `cin/cout` là đủ nhanh cho hầu hết bài (dưới ~$2 \\times 10^6$ số)."),
    ("00-Template.md",
     "kiểm tra tràn (`int` khi tổng ≤ 10¹⁸?);",
     "kiểm tra tràn (`int` khi tổng $\\le 10^{18}$?);"),
    ("02-Number-Theory.md",
     "- Bài cần phân tích 1 số lớn ≤ 10^18 → Pollard rho (mục Factor).",
     "- Bài cần phân tích 1 số lớn $\\le 10^{18}$ → Pollard rho (mục Factor)."),
    ("02-Number-Theory.md",
     "- Mảng `int[LIM]` ~ 4·LIM byte; SPF dùng `LIM ≤ 2·10^7`.",
     "- Mảng `int[LIM]` ~ $4 \\cdot LIM$ byte; SPF dùng $LIM \\le 2 \\cdot 10^7$."),
    ("02-Number-Theory.md",
     "**Mục đích:** Kiểm tra nguyên tố xác định cho số ≤ 7·10^18.",
     "**Mục đích:** Kiểm tra nguyên tố xác định cho số $\\le 7 \\cdot 10^{18}$."),
    ("02-Number-Theory.md",
     "**Mục đích:** Phân tích n (≤ 2^64) thành thừa số nguyên tố, không cần n nguyên tố nhỏ.",
     "**Mục đích:** Phân tích n ($\\le 2^{64}$) thành thừa số nguyên tố, không cần n nguyên tố nhỏ."),
    ("02-Number-Theory.md",
     "- Số ước $d(n)$: ~100 khi n < 5·10⁴, ~500 khi n < 10⁷, ~2000 khi n < 10¹⁰ — check nhanh giới hạn DFS.",
     "- Số ước $d(n)$: ~100 khi $n < 5 \\cdot 10^4$, ~500 khi $n < 10^7$, ~2000 khi $n < 10^{10}$ — check nhanh giới hạn DFS."),
    ("03-Combinatorics.md",
     "- `p` nguyên tố; precompute `fac` đến `p-1` (p ≤ ~10⁶).",
     "- `p` nguyên tố; precompute `fac` đến `p-1` ($p \\lesssim 10^6$)."),
    ("03-Combinatorics.md",
     "— chọn subset điều kiện (2ⁿ khi n ≤ 20).",
     "— chọn subset điều kiện ($2^n$ khi $n \\le 20$)."),
    ("03-Combinatorics.md",
     "- $2^{|S|}$ phải tính được; kiểm tra |S| ≤ 25.",
     "- $2^{|S|}$ phải tính được; kiểm tra $|S| \\le 25$."),
    ("04-Data-Structures.md",
     "- Grid tĩnh (không update). Dùng `ll` khi tổng > 2·10⁹.",
     "- Grid tĩnh (không update). Dùng `ll` khi tổng $> 2 \\cdot 10^9$."),
    ("04-Data-Structures.md",
     "**Mục đích:** Ánh xạ giá trị lớn/rời rạc (10⁹) về index `[0, n)` nhỏ — cần cho segtree theo giá trị.",
     "**Mục đích:** Ánh xạ giá trị lớn/rời rạc ($10^9$) về index `[0, n)` nhỏ — cần cho segtree theo giá trị."),
    ("05-Graph.md",
     "đỉnh thuộc ≥ 2 block ⇔ điểm cắt.",
     "đỉnh thuộc $\\ge 2$ block ⇔ điểm cắt."),
    ("05-Graph.md",
     "**Mục đích:** Đường đi Euler (đi qua mọi cạnh đúng 1 lần) — tồn tại khi và chỉ khi liên thông + số đỉnh lẻ ≤ 2.",
     "**Mục đích:** Đường đi Euler (đi qua mọi cạnh đúng 1 lần) — tồn tại khi và chỉ khi liên thông + số đỉnh lẻ $\\le 2$."),
    ("05-Graph.md",
     "- Cây n (0-based); `P[root] = root`. `log` ≤ 60 với n ≤ 10¹⁸? → `lg = 63 - clz`.",
     "- Cây n (0-based); `P[root] = root`. `log` $\\le 60$ với $n \\le 10^{18}$? → `lg = 63 - clz`."),
    ("05-Graph.md",
     "- Cây n thành ≤ log n light edge trên đường root→node → mỗi query chạm ≤ log n đoạn liên tục trong mảng `pos`.",
     "- Cây n thành $\\le \\log n$ light edge trên đường root→node → mỗi query chạm $\\le \\log n$ đoạn liên tục trong mảng `pos`."),
    ("05-Graph.md",
     "- Highest-label + gap heuristic → thực tế rất nhanh (n ≤ 5·10³, m ≤ 10⁵).",
     "- Highest-label + gap heuristic → thực tế rất nhanh ($n \\le 5 \\cdot 10^3$, $m \\le 10^5$)."),
    ("06-DP-Optimization.md",
     "- Sản phẩm $a_i \\cdot b_j$ với `ll` tràn → check cận (≤ 10^18).",
     "- Sản phẩm $a_i \\cdot b_j$ với `ll` tràn → check cận ($\\le 10^{18}$)."),
    ("06-DP-Optimization.md",
     "— $O(2^n \\cdot n^2)$, n ≤ 20.",
     "— $O(2^n \\cdot n^2)$, $n \\le 20$."),
    ("07-Strings.md",
     'Bài "chuỗi con chung dài nhất của 2 mảng con" / "chuỗi con có mặt ≥ k lần" → suffix array.',
     'Bài "chuỗi con chung dài nhất của 2 mảng con" / "chuỗi con có mặt $\\ge k$ lần" → suffix array.'),
    ("08-Geometry.md",
     "chỉ dùng cross/dot int — cẩn thận tràn với tọa độ ≤ 10^9 → cross ~10^18 OK borderline).",
     "chỉ dùng cross/dot int — cẩn thận tràn với tọa độ $\\le 10^9$ → cross ~$10^{18}$ OK borderline)."),
    ("08-Geometry.md",
     "`ll` → so sánh chính xác (cross với |tọa độ| ≤ 10^9).",
     "`ll` → so sánh chính xác (cross với |tọa độ| $\\le 10^9$)."),
    ("08-Geometry.md",
     "với mỗi điểm mới chỉ check ≤ 6 điểm trong ô $d \\times 2d$.",
     "với mỗi điểm mới chỉ check $\\le 6$ điểm trong ô $d \\times 2d$."),
    ("08-Geometry.md",
     'Nhận ra: "n ≤ 10^5, tìm cặp distance nhỏ nhất" → sweep;',
     'Nhận ra: "$n \\le 10^5$, tìm cặp distance nhỏ nhất" → sweep;'),
    ("08-Geometry.md",
     "→ tọa độ nguyên (long double chứa chính xác ≤ 2^63); số thực mượt cẩn thận với trị ~0.",
     "→ tọa độ nguyên (long double chứa chính xác $\\le 2^{63}$); số thực mượt cẩn thận với trị ~0."),
    ("08-Geometry.md",
     "(thực tế ~√n), tệ nhất $O(n)$",
     "(thực tế ~$\\sqrt{n}$), tệ nhất $O(n)$"),
]


def main() -> int:
    by_file = {}
    for fn, old, new in EDITS:
        by_file.setdefault(fn, []).append((old, new))
    bad = 0
    for fn, edits in by_file.items():
        p = NB / fn
        text = p.read_text(encoding="utf-8")
        for old, new in edits:
            n = text.count(old)
            if n != 1:
                print(f"ERROR {fn}: {n}x match for: {old[:70]}")
                bad += 1
                continue
            text = text.replace(old, new)
        p.write_text(text, encoding="utf-8")
    print("errors:", bad)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
