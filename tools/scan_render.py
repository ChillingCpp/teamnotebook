# -*- coding: utf-8 -*-
"""Quét MỌI trường hợp $ liền kề ký tự khác space (rule GitHub: opening $ phải
đứng sau whitespace / đầu dòng / '(' thì mới render; closing $ không được dính chữ).
Bỏ qua ```fence``` và span `inline code`."""
import re
import glob
import io
import sys

BACKTICK = re.compile(r"`[^`]*`")


def scan_file(path):
    text = io.open(path, encoding="utf-8").read()
    lines = text.split("\n")
    in_fence = False
    out = []
    for i, raw in enumerate(lines, 1):
        if raw.strip().startswith("```"):
            in_fence = not in_fence
            continue
        if in_fence:
            continue
        line = BACKTICK.sub(lambda m: " " * len(m.group(0)), raw)

        if "$$" in line:
            s = line.strip()
            # $$...$$ đơn dòng phải bắt đầu ngay $$ (không indent) và
            # không dính text khác ngoài $$...$$
            if re.match(r"^\s*\$\$.+", s) and not re.match(r"^\$\$", s):
                out.append(("dd_indent", i, raw[:110]))
            if re.match(r"^\$\$.+\$\$.+", s):
                out.append(("dd_trailing_text", i, raw[:110]))
            continue

        if line.lstrip().startswith("|") and "$" in line:
            out.append(("table_dollar", i, raw[:110]))
        # indent >=4 có $ chỉ nguy hiểm khi dòng trước trống (indented code block);
        # dòng trước là text -> lazy continuation của paragraph -> math vẫn render
        prev_raw = lines[i - 2] if i >= 2 else ""
        if re.match(r"^( {4,})\S", raw) and "$" in line and not prev_raw.strip():
            out.append(("indent4_dollar", i, raw[:110]))

        positions = [j for j, ch in enumerate(line) if ch == "$"]
        if len(positions) % 2:
            out.append(("odd_dollar", i, raw[:110]))
        for idx in range(0, len(positions) - 1, 2):
            op, cl = positions[idx], positions[idx + 1]
            prev = line[op - 1] if op > 0 else ""
            if prev and not prev.isspace():
                out.append(("open_after_char", i, repr(prev + "│" + line[op:cl + 1][:45])))
            nxt = line[cl + 1] if cl + 1 < len(line) else ""
            if nxt and not nxt.isspace():
                # ')' '.' ',' ')' ... thường OK; vẫn liệt kê để probe
                out.append(("close_before_char", i, repr(line[op:cl + 1][-45:] + "│" + nxt)))
            content = line[op + 1:cl]
            if content.startswith(" ") or content.endswith(" ") or not content:
                out.append(("space_inside", i, repr("$" + content[:40] + "$")))
    return out


def main():
    import collections
    cats = {}
    op_chars = collections.Counter()
    cl_chars = collections.Counter()
    for f in sorted(glob.glob("notebook/*.md")):
        for cat, ln, snip in scan_file(f):
            cats.setdefault(cat, []).append((f, ln, snip))
            if cat == "open_after_char":
                op_chars[snip.split("\u2502")[0].strip("'")] += 1
            elif cat == "close_before_char":
                cl_chars[snip.split("\u2502")[-1].strip("'")] += 1
    for cat in sorted(cats):
        print(f"== {cat}: {len(cats[cat])}")
        if cat in ("open_after_char", "close_before_char"):
            print(f"   chars: {dict(op_chars if cat.startswith('open') else cl_chars)}")
        else:
            for f, ln, snip in cats[cat][:80]:
                print(f"   {f}:{ln}: {snip}")
    if not cats:
        print("clean")
    return 0


if __name__ == "__main__":
    sys.exit(main())
