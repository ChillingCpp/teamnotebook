# teamnotebook

Sửa đổi giúp tôi những điều sau : 
- lazy segment tree không nên implement dạng pointer mà nên dùng dạng array
- Xóa toàn bộ code của Fenwick Tree/BIT hay sự kết hợp của các thuật toán trên, thay vào đó là Segment Tree nếu chưa có.
- Cần thêm 1 file markdown chỉ dùng để liệt kê toàn bộ thuật toán có thể dùng để đi thi ICPC từ cơ bản đến nâng cao
    - nếu quá mức nâng cao, khó và không có trong TRD thì không viết vào
    - còn lại thì viết vào, chỉ viết tên thuật toán trên 1 dòng xong sau đó xuống hàng, không liệt kê gì thêm.
- phần code : 
    - không dùng vòng lặp do - while, chỉ dùng vòng lặp for, while
- phần công thức toán học :
    - chỉnh sửa lại toàn bộ các phần công thức toán học thành dạng latex markdown, sử dụng 2 loại kí hiệu là $ $ hay $$ $$ thay vì ` `
- thiếu hoặc do tôi kiểm tra chưa kĩ :
    - bridge edge tree : bạn được phép gom cả bridge edge tree và block cut tree vào 1 note chung, cần comment code nào để tạo bridge edge và code nào tạo block cut
    - euler tour : trong note cũ của tôi thì có 3 loại euler tour và tôi cũng có note rõ về 3 loại đó dùng cho mục đích nào, hãy tự phân nó vào đúng vị trí nên dùng
    - có thể thêm note về xây MST, query có tính chất gì, có thể có bài mà phải xây MST và thực hiện query trên đó
    - sieve eratosthenes phân đoạn [L, R]
    - parallel binary search
- loại bỏ :
    - Dijkstra ( quá cơ bản )