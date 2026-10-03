# TRD — Notebook thi đấu ICPC

Sổ tay cá nhân: **10 file nội dung** (mô tả tiếng Việt, code C++17) + **1 file danh sách thuật toán**. Mỗi mục có cấu trúc:

> **Tên → Mục đích → Điều kiện sử dụng → Độ phức tạp (Time + Space) → `**Dependency:**` → Code**

Đoạn code luôn đánh dấu `// id: <tên>` để tra cứu và ghép tự động.

## Mục lục

| File | Chủ đề chính | Snippet |
|---|---|---|
| [00-Template.md](00-Template.md) | Template đầu bài, lệnh biên dịch, bit hacks, checklist submit | 2 |
| [01-Math.md](01-Math.md) | Đại số tuyến tính (ma trận, Gaussian, GF(2)), phương trình, hệ tuyến tính, lượng giác, tổng–Taylor, xác suất, Markov, ma trận | 5 |
| [02-Number-Theory.md](02-Number-Theory.md) | Mod, Euclid, CRT, sieve mảng + **sieve đoạn [L, R]**, Miller–Rabin, Pollard Rho, mod sqrt, BSGS, φ/μ, floor sum | 12 |
| [03-Combinatorics.md](03-Combinatorics.md) | nCr, Lucas, multinomial, Catalan, Stirling/Eulerian/Bell, Burnside–Pólya, Inclusion–Exclusion | 7 |
| [04-Data-Structures.md](04-Data-Structures.md) | Segtree, lazy (mảng), segtree 2D, persistent, prefix 2D, DSU (+rollback), sparse table, treap, order statistic tree, **Mo**, **motree**, nén tọa độ | 13 |
| [05-Graph.md](05-Graph.md) | Topo, Bellman, Floyd, SCC, 2-SAT, **bridge + block-cut (1 note)**, **Euler tour 3 loại** (loại 3 đặt tại 04), LCA, HLD, **MST + query trên MST**, flow/matching | 19 |
| [06-DP-Optimization.md](06-DP-Optimization.md) | Binary/ternary search, CHT, divide & conquer, Knuth, SOS, LIS, bitmask, **parallel binary search** | 9 |
| [07-Strings.md](07-Strings.md) | KMP, Z-function, Manacher, hash, trie, suffix array + LCP, Aho-Corasick, Booth | 8 |
| [08-Geometry.md](08-Geometry.md) | Point, khoảng cách/giao, hull + đường kính, đa giác, closest pair, đường tròn, lineHull, KD-tree | 12 |
| [09-Contest.md](09-Contest.md) | Mod ops nhanh, sqrt decomposition (mảng chặn), fractional cascading | 3 |
| [10-Algorithm-List.md](10-Algorithm-List.md) | **109 tên** thuật toán ICPC cơ bản → nâng cao, mỗi tên 1 dòng, không code | — |

**Bold** = các mục bổ sung theo yêu cầu (lazy mảng, gỡ BIT, bridge/block-cut gộp, euler tour phân vị trí, MST + query, segsieve [L, R], parallel binary search).

## Tra cứu nhanh trong thi

- Ctrl+F `// id: <tên>` → nhảy thẳng đoạn code (vd. `// id: lazysegtree`).
- `**Dependency:**` liệt kê snippet phải ghép cùng (ngoài `template.cpp`).
- Ghép snippet = dán các block theo dependency; `tools/tests/*.cpp` là ví dụ ghép thật, chạy được ngay.

## Trạng thái kiểm chứng

Chạy từ thư mục gốc repo (cần `g++` — đường dẫn khai báo trong `tools/check_notebook.py`):

| Lệnh | Kết quả thật |
|---|---|
| `python tools\check_notebook.py` | **90/90 khối biên dịch OK** — mỗi `// id:` kèm transitive dependency, g++ `-std=c++17 -fsyntax-only -Wall` |
| `python tools\run_tests.py` | **13/13 file test pass** — `ds1 ds2 dp contest graph1 graph2 strings linear geometry geometry2 math_mod math_big comb`, g++ `-std=c++17 -O2`, chạy thật, exit code ≠ 0 → fail |

- Phủ: **88/88 id** khai báo trong `//@ ids:` của test; 2 block còn lại là `template.cpp` (header của mọi TU test). Test dùng `mt19937_64` **có seed cố định** → chạy lại cho kết quả giống hệt.
- Test đối chiếu bằng **brute force / reference độc lập** (O(n²)–O(n³), exhaustive nhỏ), không phải "chạy không crash là pass".
- **Mutation test** cho `05-Graph.md`: mỗi 1 snippet bị chèn 1 lỗi vào TU rồi chạy → **19/19 lỗi bị assert bắt** (không lỗi nào lọt).
- Nhánh không cover: đường **no-solution** của `crt`/`modsqrt` — snippet dùng `assert` nên dừng chương trình; chỉ test nghiệm tồn tại/đúng.

## Lỗi tìm thấy qua test và đã sửa

| Mục | Lỗi | Fix |
|---|---|---|
| `phi` | sai giá trị φ: thiếu `phi[i] = i-1` ở số nguyên tố, thiếu `×p` khi `p \| i` | sửa 2 nhánh; test assert φ toàn mảng $10^6$ |
| `ternarySearch` | code tìm **MAX** nhưng comment mô tả MIN (khác `golden()` bên dưới) | đổi chiều so sánh về MIN |
| `pbs` | doc ghi `>=` — sai off-by-one (code tính mid đầu tiên mà `kq = false` → phải là `>`) | sửa doc 4 chỗ |
| `trie` | `if (!v \|\| ...)` với root `v = 0` → `countPrefix/countExact` luôn trả 0 | bỏ điều kiện `!v` |
| `aho.add` | `int& u` đọc lại **sau** `trie.emplace_back()` → con trỏ treo nếu vector realloc | ghi index vào node trước khi thêm |
| `lineDist` | code trả khoảng cách **có dấu**, doc ghi công thức tuyệt đối | bọc `fabs(...)` khớp doc |
| `bicomps` | code truyền rvalue `vi(...)` → ví dụ doc `bicomps([&](vi& c){...})` không compile | dựng `vi comp(...)` lvalue rồi `f(comp)` — nhận `vi&`/`const vi&`/`vi` đều được |

Test cũng sửa theo: bật lại assert `trie` counts, tăng assert φ (xem `tools/tests/strings.cpp`, `math_mod.cpp`).

## Quy ước nội dung

- **Không `do-while`** (chỉ `for`/`while`); **không Fenwick/BIT** — mọi bài BIT cũ đã chuyển sang segment tree; **Dijkstra** bỏ khỏi notebook (chỉ còn tên trong danh sách).
- Công thức toán viết bằng `$...$` / `$$...$$`; identifier/biểu thức code giữ trong `` `...` ``. Ký tự ngay trước `$` mở phải là space / đầu dòng / `(` thì mới render (vd đúng: `~ $10^9$`, `$O(1)$ / $O(n)$`; sai: `~$10^9$`, `$O(1)$/$O(n)$` — sẽ hiện thô); block `$$..$$` cần dòng trống trước và sau (rule đã probe qua GitHub Markdown API).
- Snippet tối giản, trừu tượng hóa tối thiểu; ý trọng yếu của observation cũ nằm trong code comment — copy-and-go khi thi.
- **Mục đích** = dùng để làm gì + tính năng gì (ngắn gọn); **Điều kiện sử dụng** = tổng quát hóa (tính chất update/op phải thỏa mãn, dạng bài tổng quát).
