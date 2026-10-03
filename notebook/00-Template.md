# 00 — Template & công cụ

## template.cpp

**Mục đích:** Khởi động nhanh mỗi bài: I/O nhanh, macro và typedef dùng chung cho mọi snippet trong TRD. Dán nguyên khối này vào đầu file bài giải.

**Ý tưởng / Observation:**
- `cin.tie(0)->sync_with_stdio(0)` rồi dùng `cin/cout` là đủ nhanh cho hầu hết bài (dưới ~ $2 \times 10^6$ số).
- `cin.exceptions(cin.failbit)` → fail ngay khi đọc thiếu input/cạn EOF, chuyển RE thành lỗi rõ ràng thay vì WA im lặng. Bỏ dòng này nếu không chắc input đọc hết (vd. interactive).
- Muốn debug có kiểm soát: chỉ định nghĩa `LOCAL` khi biên dịch local.

**Điều kiện sử dụng:**
- g++ ≥ C++17. Mọi snippet trong TRD giả sử đã có khối này (xem `**Dependency:**` từng mục).

**Độ phức tạp:**
- Time: I/O $O(bytes input)$.
- Space: $O(1)$.

**Dependency:** —

```cpp
#include <bits/stdc++.h>
using namespace std;

#define rep(i, a, b) for (int i = (a); i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()

using ll = long long;
using pii = pair<int, int>;
using vi = vector<int>;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin.exceptions(cin.failbit); // không dùng nếu có interactive
}
```

## Debug (dbg)

**Mục đích:** In biến ra stderr kèm tên + số dòng khi test local; biến mất hoàn toàn khi nộp (không định nghĩa `LOCAL`).

**Ý tưởng / Observation:**
- Dùng `ostringstream` nên `dbg` hoạt động với mọi kiểu có `operator<<` (int, string, pair, vector, …).
- `dbg` là macro: truyền biểu thức thuần, không đặt trong biểu thức sẽ chạy thật (vd. `dbg(v[i])` vẫn tính `v[i]` khi nộp). Chỉ dùng để debug, gỡ trước khi nộp nếu không chắc.

**Điều kiện sử dụng:**
- Biên dịch local kèm `-DLOCAL`. Khi nộp, KHÔNG define `LOCAL` → macro rỗng, không in gì.

**Dependency:** template.cpp

```cpp
#ifdef LOCAL
template <class T> string toStr(T v);  // forward
template <class A, class B> string toStr(pair<A, B> p);
template <class A> string toStr(vector<A> v);
template <class T> string toStr(T v) {
    ostringstream os;
    os << boolalpha << v;
    return os.str();
}
template <class A, class B> string toStr(pair<A, B> p) {
    return "(" + toStr(p.first) + "," + toStr(p.second) + ")";
}
template <class A> string toStr(vector<A> v) {
    string r = "{";
    for (size_t i = 0; i < v.size(); ++i) { if (i) r += ","; r += toStr(v[i]); }
    return r + "}";
}
#define dbg(x) cerr << __LINE__ << ": " << #x << " = " << toStr(x) << "\n"
#else
#define dbg(x) ((void)0)
#endif
```

## Lệnh biên dịch & test local

**Mục đích:** Lệnh g++ dùng khi thi/local, sanitizer bắt RE âm thầm (tràn, chia 0, out-of-bound).

**Ý tưởng / Observation:**
- `-fsanitize=address,undefined` chậm ~10-20× → chỉ chạy local trên case nhỏ.
- Sinh ngẫu nhiên: `gen > in.txt && ./main < in.txt > out.txt`, so `out.txt` với brute.

**Điều kiện sử dụng:**
- judge dùng g++; một số judge không bật sanitizer (không ảnh hưởng — chỉ lệnh local).

```bash
# Nộp (nhanh):
g++ -std=c++17 -O2 -Wall -o main main.cpp

# Test local (debug + bắt lỗi bộ nhớ):
g++ -std=c++17 -O2 -DLOCAL -fsanitize=address,undefined -g -o main main.cpp

./main < in.txt          # chạy với input
./main < in.txt > out.txt && diff out.txt ans.txt
```

## Bit hacks

**Mục đích:** Các trick bit dùng trực tiếp trong code, không cần cấu trúc dữ liệu.

**Ý tưởng / Observation:**
- `x & -x` → bit 1 thấp nhất của `x` (check lẻ, tách nhị phân).
- Duyệt mọi subset khác rỗng của `m`: `for (int s = m; s; s = (s - 1) & m)` (giảm dần theo giá trị).
- `__builtin_popcount(x)` = số bit 1; `31 - __builtin_clz(x)` = $\lfloor\log_2 x\rfloor$ (`63-` với `ll`); `__builtin_ctz(x)` = số bit 0 đầu.
- XOR: `x ^ x = 0`, đổi 2 biến không cần biến tạm: `a ^= b; b ^= a; a ^= b;`.
- Đếm bit 1 có parity: dùng `__builtin_parity`.
- Chia $2^n$: `x >> n` ($x \ge 0$). Nhân $2^n$: `x << n` — cẩn thận tràn `int`.

**Điều kiện sử dụng:**
- Bit operations trên `int`/`ll` đã signed → dãy bit bổ sung có thể lấy kết quả lạ nếu tràn quá 63 bit.


## Danh sách kiểm tra trước khi submit

**Mục đích:** Checklist 30 giây trước khi bấm submit (WA/RE/TLE/MLE ngu nhất đều nằm trong đây).

**Ý tưởng / Observation:**
- Trước submit: chạy lại sample; sinh case max nếu TL sát; kiểm tra tràn (`int` khi tổng $\le 10^{18}$?); nộp đúng file; format output đúng (space/endl, "YES"/"NO" chữ hoa).
- **WA:** in debug (local) / thêm `dbg`; clear toàn bộ DS giữa các test case; đọc lại đề — 0-based hay 1-based, có test đặc biệt nào chưa xử lý; xài lại STL có đúng ý không (vd. `lower_bound` vs `upper_bound`); thêm `assert` rồi submit lại.
- **RE:** truy cập vượt `vector` (vòng lặp `<= n`?); chia 0 / `% 0`; đệ quy quá sâu (stack ~8MB → tăng mảng hoặc iterative); biến chưa khởi tạo; iterator bị invalidate sau `erase` trong vòng lặp.
- **TLE:** phức tạp có đúng không (check vòng lặp lẫn index); copy thừa (`vector` truyền giá trị → truyền reference); `map/set` chậm → `unordered_map`/mảng; TLE sát → tắt debug in, đổi `cin` → fast input.
- **MLE:** clear DS giữa các test case; mảng static to quá → động hoặc `short`; đệ quy segtree tạo node thay vì mảng 4N? (giữ 4N nếu chắc chắn).
- Giải thích thuật toán cho đồng đội / đứng dậy đi 2 phút — phát hiện sai sót nhanh nhất là khi nhìn lại bằng mắt thường.
