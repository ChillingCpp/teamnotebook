# -*- coding: utf-8 -*-
"""Trích mọi ```cpp block từ notebook/*.md, ghép dependency theo `// id:` và
`**Dependency:**`, biên dịch từng khối bằng g++ -std=c++17 -fsyntax-only.

Usage:
    python tools/check_notebook.py            # check hết
    python tools/check_notebook.py fenwick    # check 1 id (kèm deps)
    python tools/check_notebook.py --gen fenwick  # in TU ra stdout

Exit code 0 = mọi khối biên dịch sạch.
"""
import io
import re
import subprocess
import sys
from pathlib import Path

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")

ROOT = Path(__file__).resolve().parent.parent
NB = ROOT / "notebook"
GXX = r"E:\DATA\D\usr\bin\g++.exe"
CXXFLAGS = ["-std=c++17", "-fsyntax-only", "-Wall", "-Wno-unused-variable",
            "-Wno-unused-but-set-variable", "-Wno-sign-compare", "-Wno-unused-function"]

BLOCK_RE = re.compile(r"```cpp\n(.*?)```", re.S)
ID_RE = re.compile(r"^// id: (\w+)", re.M)
DEP_RE = re.compile(r"\*\*Dependency:\*\*\s*(.+)")


def parse_notebook():
    """-> dict[id] = dict(code, deps[list], file, heading)"""
    snippets = {}
    for md in sorted(NB.glob("*.md")):
        text = md.read_text(encoding="utf-8")
        # tách theo heading để lấy heading + dependency cho mỗi block
        parts = re.split(r"(?m)^## (.+)$", text)
        # parts[0] = preamble, sau đó lặp (heading, body)
        cur_heading = "(top)"
        for chunk in parts[1:] if len(parts) > 1 else [text]:
            pass
        # simpler: quét mọi block, tìm heading gần nhất phía trên
        lines = text.splitlines()
        heading = "(top)"
        pos = 0
        first_cpp = True  # block cpp đầu của 00-Template.md chính là header mẫu
        for i, line in enumerate(lines):
            if line.startswith("## "):
                heading = line[3:]
            if line.strip() == "```cpp":
                # tìm block kết thúc
                j = i + 1
                while j < len(lines) and lines[j].strip() != "```":
                    j += 1
                code = "\n".join(lines[i + 1:j])
                # tìm Dependency trong [heading..block)
                dep_line = ""
                for k in range(i - 1, max(-1, i - 40), -1):
                    m = DEP_RE.match(lines[k])
                    if m:
                        dep_line = m.group(1).strip()
                        break
                deps = []
                if dep_line and dep_line not in ("—", "-", "--"):
                    for d in re.split(r"[,;]", dep_line):
                        d = d.strip()
                        if d and d != "template.cpp":
                            deps.append(d)
                m = ID_RE.search(code)
                sid = m.group(1) if m else None
                if sid:
                    if sid in snippets:
                        print(f"[dup] id '{sid}' ở {md.name} bị bỏ qua (đã có ở {snippets[sid]['file']})")
                    else:
                        snippets[sid] = dict(code=code, deps=deps,
                                             file=md.name, heading=heading,
                                             is_header=False)
                else:
                    snippets[f"__noid__{md.stem}_{len(snippets)}"] = dict(
                        code=code, deps=deps, file=md.name, heading=heading,
                        is_header=(md.name == "00-Template.md" and first_cpp))
                first_cpp = False
    return snippets


def template_header():
    """Khối template.cpp (bỏ main) làm header chung."""
    text = (NB / "00-Template.md").read_text(encoding="utf-8")
    m = re.search(r"```cpp\n(.*?)```", text, re.S)
    code = m.group(1)
    idx = code.find("int main()")
    if idx != -1:
        code = code[:idx]
    return code


def resolve(sid, snippets, seen=None):
    """Trả list code dep theo thứ tự (topological, dedup)."""
    seen = seen if seen is not None else set()
    if sid in seen:
        return []
    seen.add(sid)
    sn = snippets[sid]
    out = []
    for d in sn["deps"]:
        if d not in snippets:
            print(f"[warn] {sid}: dep '{d}' không tồn tại")
            continue
        out.extend(resolve(d, snippets, seen))
    out.append(sn["code"])
    return out


def build_tu(sid, snippets, with_main=True):
    if snippets[sid].get("is_header"):
        # header mẫu tự chứa main: biên dịch độc lập, không ghép header lên chính nó
        tu = snippets[sid]["code"]
    else:
        header = template_header()
        body = resolve(sid, snippets)
        tu = header + "\n" + "\n\n".join(body)
    if with_main and "int main(" not in tu:
        tu += "\n\nint main() { (void)0; }\n"
    return tu


def compile_tu(tu, tag):
    p = Path(__import__("tempfile").gettempdir()) / f"nb_{tag}.cpp"
    p.write_text(tu, encoding="utf-8")
    r = subprocess.run([GXX, *CXXFLAGS, str(p)],
                       capture_output=True, text=True, encoding="utf-8",
                       errors="replace")
    return r.returncode, (r.stderr or "").strip()


def main():
    args = [a for a in sys.argv[1:]]
    gen = False
    if args and args[0] == "--gen":
        gen = True
        args = args[1:]
    snippets = parse_notebook()
    if gen:
        sid = args[0]
        print(build_tu(sid, snippets, with_main=False))
        return 0
    targets = args if args else sorted(snippets)
    bad = []
    for sid in targets:
        if sid not in snippets:
            print(f"[miss] id '{sid}' không có")
            bad.append(sid)
            continue
        rc, err = compile_tu(build_tu(sid, snippets), re.sub(r"\W", "_", sid))
        if rc != 0:
            bad.append(sid)
            print(f"[FAIL] {sid}  ({snippets[sid]['file']} · {snippets[sid]['heading']})")
            for line in err.splitlines()[:25]:
                print("    " + line)
        else:
            print(f"[ ok ] {sid}")
    print(f"\n{len(targets) - len(bad)}/{len(targets)} khối biên dịch OK")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
