# -*- coding: utf-8 -*-
"""Kiểm tra render-được của display math $$ và GFM strikethrough ~...~

GitHub rule (đã probe): $$ phải là paragraph RIÊNG — dòng liền trước và dòng
liền sau block phải trống; nếu dính text cùng paragraph thì GitHub trả raw.

- Block đa dòng:   $$\\n<formula>\\n$$
- Block đơn dòng:  $$<formula>$$
Dòng nằm trong block (content) không cần check.

Ngoài ra: dòng có >= 2 ký tự ~ ngoài code span/fence -> GFM strikethrough.
"""
import re
import glob
import io
import sys

BACKTICK = re.compile(r"`[^`]*`")
SINGLE = re.compile(r"^\s*\$\$.+\$\$\s*$")


def check(path):
    lines = io.open(path, encoding="utf-8").read().split("\n")
    problems = []
    in_fence = False
    i = 0
    while i < len(lines):
        l = lines[i]
        if l.strip().startswith("```"):
            in_fence = not in_fence
            i += 1
            continue
        if in_fence:
            i += 1
            continue
        l_nb = BACKTICK.sub(" ", l)  # bỏ inline code: `$$...$$` trong backtick là meta
        if "$$" in l_nb:
            prev = lines[i - 1] if i > 0 else ""
            if SINGLE.match(l):
                end = i
            elif l.strip() == "$$":
                j = i + 1
                while j < len(lines) and lines[j].strip() != "$$":
                    if lines[j].strip().startswith("```"):
                        problems.append(("dd_fence_inside", j + 1, lines[j][:80], ""))
                        break
                    j += 1
                if j >= len(lines):
                    problems.append(("dd_unclosed", i + 1, l[:80], ""))
                    i += 1
                    continue
                end = j
            else:
                problems.append(("dd_weird_form", i + 1, l[:100], ""))
                end = i
            after = lines[end + 1] if end + 1 < len(lines) else ""
            if prev.strip() and "$$" not in prev:
                problems.append(("dd_no_blank_before", i + 1, lines[i][:90], prev[:70]))
            if after.strip() and "$$" not in after:
                problems.append(("dd_no_blank_after", end + 1, lines[end][:90], after[:70]))
            i = end + 1
            continue
        # strikethrough GFM: cần opener (~ + ko-space) đứng TRƯỚC closer (ko-space + ~)
        text = BACKTICK.sub(" ", l)
        tildes = [m.start() for m in re.finditer(r"~", text)]
        openers = [p for p in tildes if p + 1 < len(text) and not text[p + 1].isspace()]
        closers = [p for p in tildes if p > 0 and not text[p - 1].isspace()]
        if any(o < c for o in openers for c in closers):
            problems.append(("tilde_pair", i + 1, l[:110], f"~x{len(tildes)}"))
        i += 1
    return problems


def main():
    n = 0
    for f in sorted(glob.glob("notebook/*.md")):
        for cat, ln, snip, extra in check(f):
            print(f"{cat}: {f}:{ln}: {snip}  |ctx: {extra}")
            n += 1
    print(f"total {n}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
