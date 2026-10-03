# Benchmark MC1 và MC2

Ngày đo: 03/10/2026  
Biên dịch: GCC 15.2.0, `-O2`, Windows x64 (MSYS2 UCRT64)  
Chương trình đo: `tests/Benchmark.cpp`

## MC1 — tra cứu sách theo mã

Mỗi cỡ dữ liệu dùng 1.000 lượt tìm thành công với mã được chọn ngẫu nhiên bằng seed cố định `20261003`. Linear scan và HashTable nhận cùng một dãy khóa. HashTable bắt đầu ở cấu hình mặc định 101 bucket như `BookRepository`, rồi tự tăng kích thước khi hệ số tải vượt 0,75. Thời gian tạo dữ liệu và nạp cấu trúc không nằm trong thời gian truy vấn.

| Số đầu sách | Linear scan trung bình/lượt | HashTable trung bình/lượt | Tỷ lệ thời gian |
|---:|---:|---:|---:|
| 1.000 | 1.592 µs | 0,059 µs | 27,0× |
| 10.000 | 13,369 µs | 0,095 µs | 140,7× |
| 100.000 | 174,596 µs | 0,153 µs | 1.148,7× |

Tất cả lượt tìm đều trả kết quả thành công. Thời gian trung bình của linear scan tăng theo số bản ghi; HashTable tăng chậm hơn trong lần đo này, phù hợp với kỳ vọng average-case O(1) khi hash phân phối khóa tốt. HashTable vẫn có worst-case O(n) nếu xảy ra nhiều va chạm.

## MC2 — truy vấn phiếu theo khoảng hạn trả

Mỗi cỡ dữ liệu dùng 500 lượt truy vấn trên cùng một tập Loan. Mỗi ngày có 5 phiếu; khóa hạn trả được xáo trộn bằng seed cố định trước khi dựng BST. Truy vấn lấy cùng khoảng quanh giữa miền ngày và trả 105 phiếu. Kết quả được đối chiếu với linear scan theo tập loanId và kiểm tra thứ tự hạn trả. Thời gian dựng BST được báo riêng, không cộng vào thời gian truy vấn.

| Số phiếu | Chiều cao BST | Linear scan trung bình/lượt | BST range query trung bình/lượt | Tỷ lệ thời gian | Dựng BST |
|---:|---:|---:|---:|---:|---:|
| 10.000 | 24 | 36,710 µs | 2,643 µs | 13,89× | 2,672 ms |
| 100.000 | 33 | 508,080 µs | 3,256 µs | 156,03× | 38,447 ms |

Cả hai mốc đều PASS về tính đúng. Kết quả cho thấy range query trên cây được dựng từ thứ tự chèn đã xáo trộn nhanh hơn quét tuyến tính trong thí nghiệm này. BST hiện không tự cân bằng: chiều cao và thời gian có thể tăng tới trường hợp O(n) nếu dữ liệu chèn khiến cây lệch. Số đo này không chứng minh bảo đảm worst-case logarithmic.

## Giới hạn khi diễn giải

Đây là một lần chạy trên một máy, một compiler và dữ liệu giả lập có seed cố định; không phải cam kết độ trễ cho mọi máy hoặc dữ liệu thật. MC1 đo các khóa tồn tại, không đo tỷ lệ khóa không tồn tại. MC2 dùng thứ tự insert ngẫu nhiên, nên không mô phỏng trường hợp BST lệch do thứ tự ngày tăng/giảm. Hãy chạy lại trên máy demo nếu cần báo cáo số liệu tại môi trường của nhóm và giữ nguyên cấu hình/seed để so sánh được.
