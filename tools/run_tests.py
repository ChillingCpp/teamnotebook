# -*- coding: utf-8 -*-
"""Chạy test function cho snippet notebook.

Mỗi file tools/tests/*.cpp có dòng đầu tiên:

    //@ ids: fenwick, segtree, ...

Runner ghép: template header (00, bỏ main) + mọi snippet id kèm dependency
(topo-sorted, dedup) + phần còn lại của file test (chứa int main),
biên dịch bằng g++ -std=c++17 -O2 rồi CHẠY. Exit code 0 = pass.

Usage:
    python tools/run_tests.py             # chạy hết
    python tools/run_tests.py ds1 dp      # chỉ chạy file được nêu tên

Exit code 0 = mọi file pass.
"""
import re
import subprocess
import sys
import tempfile
from pathlib import Path

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from check_notebook import GXX, parse_notebook, resolve, template_header  # noqa: E402

TESTS = HERE / "tests"
CXXFLAGS = ["-std=c++17", "-O2", "-Wall", "-Wno-unused-variable",
            "-Wno-unused-but-set-variable", "-Wno-sign-compare", "-Wno-unused-function"]
RUN_TIMEOUT = 60  # giây mỗi test

IDS_RE = re.compile(r"^//@\s*ids:\s*(.+)$")


def build_tu(path: Path, snippets) -> tuple:
    """-> (source, [ids]) ; raise ValueError nếu thiếu dòng //@ ids:."""
    text = path.read_text(encoding="utf-8")
    lines = text.splitlines()
    m = IDS_RE.match(lines[0].strip()) if lines else None
    if not m:
        raise ValueError("dòng đầu phải là  `//@ ids: id1, id2, ...`")
    ids = [s.strip() for s in m.group(1).split(",") if s.strip()]
    seen, body = set(), []
    for sid in ids:
        if sid not in snippets:
            raise ValueError(f"id '{sid}' không có trong notebook")
        body.extend(resolve(sid, snippets, seen))
    tu = template_header() + "\n" + "\n\n".join(body) + "\n" + "\n".join(lines[1:])
    return tu, ids


def run_one(path: Path, snippets) -> tuple:
    """-> (ok, message chi tiết)."""
    try:
        tu, ids = build_tu(path, snippets)
    except ValueError as e:
        return False, str(e)
    tmp = Path(tempfile.gettempdir())
    cpp = tmp / f"nbtest_{path.stem}.cpp"
    exe = tmp / f"nbtest_{path.stem}.exe"
    cpp.write_text(tu, encoding="utf-8")
    r = subprocess.run([GXX, *CXXFLAGS, str(cpp), "-o", str(exe)],
                       capture_output=True, text=True, encoding="utf-8",
                       errors="replace")
    if r.returncode != 0:
        return False, "biên dịch lỗi:\n" + (r.stderr or "").strip()
    try:
        p = subprocess.run([str(exe)], capture_output=True, text=True,
                           timeout=RUN_TIMEOUT, encoding="utf-8", errors="replace")
    except subprocess.TimeoutExpired:
        return False, f"timeout > {RUN_TIMEOUT}s"
    if p.returncode != 0:
        out = ((p.stdout or "").strip() + "\n" + (p.stderr or "").strip()).strip()
        return False, f"exit code {p.returncode}\n{out}"
    return True, (p.stdout or "").strip().replace("\n", " | ")[:200]


def main():
    args = [a for a in sys.argv[1:]]
    files = sorted(TESTS.glob("*.cpp"))
    if args:
        files = [f for f in files if f.stem in args]
        missing = set(args) - {f.stem for f in files}
        if missing:
            print(f"[miss] không có file test: {', '.join(sorted(missing))}")
            return 1
    if not files:
        print("[miss] không có file test nào trong tools/tests/")
        return 1
    snippets = parse_notebook()
    bad = []
    for f in files:
        ok, msg = run_one(f, snippets)
        if ok:
            print(f"[ ok ] {f.stem}  {msg}")
        else:
            bad.append(f.stem)
            print(f"[FAIL] {f.stem}")
            for line in msg.splitlines()[:40]:
                print("    " + line)
    print(f"\n{len(files) - len(bad)}/{len(files)} file test pass")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
