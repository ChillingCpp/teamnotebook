# -*- coding: utf-8 -*-
"""Chuyển các span backtick chứa công thức thuần (bất đẳng thức/số) sang $...$.
Áp theo (file, line 1-based, old_span_with_backticks, new_latex) — assert có trên đúng dòng."""
import sys
from pathlib import Path

NB = Path(__file__).resolve().parent.parent / "notebook"

EDITS = [
    ("01-Math.md", 152, "`N ≤ ~100`", r"$N \lesssim 100$"),
    ("01-Math.md", 152, "`log n ≤ 60`", r"$\log n \le 60$"),
    ("02-Number-Theory.md", 10, "`a,b < mod ≤ 10^18`", r"$a,b < mod \le 10^{18}$"),
    ("02-Number-Theory.md", 44, "`a, b ≥ 0`", r"$a, b \ge 0$"),
    ("02-Number-Theory.md", 97, "`LIM ≤ 10^7`", r"$LIM \le 10^7$"),
    ("02-Number-Theory.md", 208, "`a, b, c ≤ 7.2·10^18`", r"$a, b, c \le 7.2 \cdot 10^{18}$"),
    ("02-Number-Theory.md", 215, "`c ≥ 1`", r"$c \ge 1$"),
    ("03-Combinatorics.md", 5, "`n ≤ N`", r"$n \le N$"),
    ("03-Combinatorics.md", 13, "`N ≥ mod`", r"$N \ge mod$"),
    ("03-Combinatorics.md", 42, "`n ≥ p`", r"$n \ge p$"),
    ("03-Combinatorics.md", 45, "`≤ p`", r"$\le p$"),
    ("03-Combinatorics.md", 135, "`n ≤ 2000`", r"$n \le 2000$"),
]


def main() -> int:
    by_file = {}
    for fn, ln, old, new in EDITS:
        by_file.setdefault(fn, {} ).setdefault(ln, []).append((old, new))
    bad = 0
    for fn, lines_edits in by_file.items():
        p = NB / fn
        lines = p.read_text(encoding="utf-8").split("\n")
        for ln, edits in lines_edits.items():
            idx = ln - 1
            for old, new in edits:
                if old in lines[idx]:
                    lines[idx] = lines[idx].replace(old, new, 1)
                elif new not in lines[idx]:
                    print(f"ERROR {fn}:{ln} not found: {old}")
                    bad += 1
        p.write_text("\n".join(lines), encoding="utf-8")
    print("errors:", bad)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
