"""Chuyển O(...) trong prose (ngoài code fence, ngoài $...$) thành $O(...)$ LaTeX."""
import re
import sys
from pathlib import Path

NB = Path(__file__).resolve().parent.parent / "notebook"

SUP = {"⁰": "^0", "¹": "^1", "²": "^2", "³": "^3", "⁴": "^4",
       "⁵": "^5", "⁶": "^6", "⁷": "^7", "⁸": "^8", "⁹": "^9"}


def fix_inner(s: str) -> str:
    s = re.sub(r"sqrt\(([^()]*)\)", r"\\sqrt{\1}", s)
    for k, v in SUP.items():
        s = s.replace(k, v)
    s = re.sub(r"\blog\b", r"\\log", s)
    s = re.sub(r"\bln\b", r"\\ln", s)
    s = s.replace("·", "\\cdot ")
    s = s.replace("≤", "\\le ").replace("≥", "\\ge ").replace("≠", "\\ne ")
    s = s.replace("×", "\\times ")
    s = s.replace("−", "-")
    s = s.replace("∑", "\\sum")
    s = s.replace("  ", " ")
    return s


def find_close(text: str, start: int) -> int:
    """start = index OF '('; trả về index của ')' tương ứng, -1 nếu thiếu."""
    depth = 0
    for i in range(start, len(text)):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                return i
    return -1


def conv_bare(text: str) -> str:
    """Chuyển O(...) chưa nằm trong $...$."""
    out = []
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if ch == "$":
            # copy nguyên segment math
            j = text.find("$", i + 1)
            if j == -1:
                out.append(text[i:])
                break
            if i + 1 < n and text[i + 1] == "$":  # $$ ... $$
                j = text.find("$$", i + 2)
                if j == -1:
                    out.append(text[i:])
                    break
                out.append(text[i:j + 2])
                i = j + 2
                continue
            out.append(text[i:j + 1])
            i = j + 1
            continue
        if (ch == "O" and i + 1 < n and text[i + 1] == "("
                and (i == 0 or not (text[i - 1].isalnum() or text[i - 1] in "_$"))):
            close = find_close(text, i + 1)
            if close != -1:
                out.append("$O" + fix_inner(text[i + 1:close + 1]) + "$")
                i = close + 1
                continue
        out.append(ch)
        i += 1
    return "".join(out)


BT_O = re.compile(r"`(O\(.+?\))`")


def conv_backtick_o(text: str) -> str:
    """`O(...)` → $O(...)$ (full-span)."""
    def rep(m):
        inner = m.group(1)
        close = find_close(inner, 1)
        if close != len(inner) - 1:
            return m.group(0)
        return "$O" + fix_inner(inner[1:close + 1]) + "$"
    return BT_O.sub(rep, text)


def process(path: Path) -> bool:
    lines = path.read_text(encoding="utf-8").split("\n")
    in_fence = False
    changed = False
    for idx, line in enumerate(lines):
        if line.startswith("```"):
            in_fence = not in_fence
            continue
        if in_fence:
            continue
        new = conv_backtick_o(line)
        new = conv_bare(new)
        if new != line:
            lines[idx] = new
            changed = True
    if changed:
        path.write_text("\n".join(lines), encoding="utf-8")
    return changed


def main():
    any_change = False
    for p in sorted(NB.glob("*.md")):
        if p.name == "10-Algorithm-List.md":
            continue
        if process(p):
            print("updated", p.name)
            any_change = True
    if not any_change:
        print("no changes")
    return 0


if __name__ == "__main__":
    sys.exit(main())
