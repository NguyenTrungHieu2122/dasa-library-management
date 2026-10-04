# LibraryManagement

---

## Chạy ứng dụng C++ trong VS Code

Ứng dụng có bản console C++17 và giao diện web cục bộ. Cả hai đọc/ghi các tệp JSON trong thư mục `data/`; chỉ chạy một bản tại một thời điểm để tránh ghi dữ liệu đồng thời.

### Yêu cầu

- VS Code
- GCC/MinGW-w64 có trong `PATH` (cấu hình build có sẵn dùng GCC)
- Tiện ích Microsoft C/C++ trong VS Code; cần `gdb` có trong `PATH` để gỡ lỗi bằng F5

### Cách chạy

1. Mở thư mục gốc `dasa-library-management` trong VS Code.
2. Nhấn `Ctrl+Shift+B` để biên dịch.
3. Chạy bằng **Terminal → Run Task → Run Library Management**. Để gỡ lỗi bằng `F5`, cài GDB (MSYS2 UCRT64: `pacman -S mingw-w64-ucrt-x86_64-gdb`) và đảm bảo `gdb.exe` nằm trong `PATH`.

`CMakeLists.txt` cũng có sẵn nếu muốn dùng tiện ích CMake Tools thay cho build task MinGW.

## Chạy giao diện web cục bộ

- Cài Node.js nếu máy chưa có lệnh `node`.
- Cài GCC/MinGW-w64 và bảo đảm `g++` nằm trong `PATH`; task web sẽ biên dịch DSA Core trước khi mở máy chủ.
- Trong VS Code chọn **Terminal → Run Task… → Start Library Web (localhost)**.
- Mở `http://127.0.0.1:4173` trên chính máy này. Giữ terminal của web server mở khi dùng trang; nhấn `Ctrl+C` để dừng.
- Giao diện web dùng cùng các tệp JSON trong `data/` với ứng dụng console. Không chạy console và web cùng lúc để tránh hai chương trình ghi dữ liệu đồng thời.
- Các thao tác mượn, trả, đặt/hủy chờ, danh sách đến hạn và Top 5 trên dashboard được xử lý qua DSA Core C++.
- Khi có bản sao được trả trong lúc có người chờ, hệ thống chuyển quyền nhận cho từng người đầu hàng phù hợp theo số bản khả dụng, chuyển họ ra khỏi hàng chờ và giữ từng bản sao trong 48 giờ. Nếu hết hạn hoặc người được giữ hủy lượt, quyền nhận sách chuyển cho người tiếp theo đủ điều kiện. Giao diện hiển thị người đang được giữ riêng với những người còn chờ.
- Mục lịch sử hoạt động cho phép chọn số hoạt động gần nhất cần xem, tối đa 1.000.
- Server chỉ lắng nghe trên máy cục bộ này; người khác trên mạng chưa truy cập được.

> Chạy chương trình từ thư mục gốc dự án để chương trình tìm thấy `data/`. Có thể truyền đường dẫn thư mục dữ liệu làm tham số dòng lệnh nếu cần.

Các thao tác console hiện có: xem/tìm sách, xem thành viên, mượn/trả sách, đăng ký hàng đợi, xem top sách được mượn, tra cứu hạn trả và xem hoạt động gần đây. Dữ liệu được lưu lại vào các tệp JSON sau mỗi thao tác thay đổi.

---

# LibraryManagement

---

## 📖 Giới thiệu

LibraryManagement là đồ án môn DSA của nhóm Que Cay, xây dựng hệ thống quản lý thư viện.

Dự án được tổ chức theo nhiều tầng:

-  Presentation: giao diện người dùng / visualization. 
-  DSA Core: mô hình dữ liệu, cấu trúc dữ liệu, thuật toán, repository và service. 
-  Persistence: đọc/ghi dữ liệu. 
-  Data: dữ liệu JSON của hệ thống. 

---

## ⚙️ Mục tiêu chức năng

Dự án được xây dựng để đáp ứng các yêu cầu chính sau:

-  Tra cứu chính xác một bản ghi theo mã định danh. 
-  Tra cứu tài liệu, thành viên và phiếu mượn. 
-  Quản lý mượn/trả sách. 
-  Quản lý hàng đợi đặt chỗ theo từng tài liệu. 
-  Thống kê top K sách được mượn nhiều nhất trong N ngày gần nhất. 
-  Hiển thị các hoạt động giao dịch gần đây nhất. 
-  Lưu trữ dữ liệu bằng JSON. 

**Phạm vi MC1:** Bằng chứng tra cứu chính xác theo mã và benchmark MC1 áp dụng cho đầu sách (`bookId`) qua HashTable tự cài đặt. Tra cứu thành viên và phiếu mượn vẫn phục vụ luồng nghiệp vụ, nhưng không được tuyên bố là phần MC1 đã được tối ưu/đo benchmark.

**Top K:** Số lượt quan tâm được tính từ các phiếu mượn có ngày mượn nằm trong N ngày gần nhất; giao diện cho chọn N, console nhận K và N. Hoạt động gần đây giữ tối đa 1.000 sự kiện mới nhất trong bộ nhớ và JSON.

Để dựng benchmark ở cấu hình Release: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`, sau đó `cmake --build build --target dsa_benchmark`. Kết quả lần đo đã ghi trong `docs/Benchmark Results.md`.

---

## 🧭 Chức năng chính của hệ thống

### 1. Tra cứu theo mã

-  Tìm sách theo mã. 
-  Tìm thành viên theo mã. 
-  Tìm phiếu mượn theo mã. 

### 2. Duyệt / tra cứu theo khoảng

-  Hỗ trợ truy vấn dữ liệu có thứ tự. 
-  Phục vụ cho các báo cáo và truy vấn theo phạm vi. 

### 3. Hàng đợi mượn / trả

-  Thành viên có thể đăng ký chờ mượn khi sách đã hết. 
-  Hệ thống xử lý theo thứ tự hàng đợi. 

### 4. Top sách quan tâm

-  Thống kê top K sách được mượn nhiều nhất trong một khoảng thời gian N ngày gần nhất. 

### 5. Danh sách mượn / trả gần đây

-  Hiển thị K giao dịch gần đây nhất. 
-  Mới nhất đứng đầu. 

---

## 🗂️ Cấu trúc project

```
LibraryManagement/
│
├── docs/		# Up các tài liệu D1, D2,...
│
├── src/
│   ├── main.cpp
│   │
│   ├── presentation/              #Tầng giao diện (visualization)
│   │   ├── index.html
│   │   ├── style.css
│   │   └── script.js
│   │
│   ├── dsa_core/                  #Tầng Xử lý nghiệp vụ + DSA + Thuật toán
│   │    |
│   │   ├── models/ 
│   │   │    |
│   │   │   ├── Book.h
│   │   │   ├── Book.cpp
│   │   │   ├── Member.h
│   │   │   ├── Member.cpp
│   │   │   ├── Loan.h
│   │   │   ├── Loan.cpp
│   │   │   ├── Reservation.h
│   │   │   ├── Activity.h
│   │   │
│   │   ├── structures/
│   │   │    |
│   │   │   ├── Node.h
│   │   │   ├── LinkedList.h
│   │   │   ├── LinkedList.cpp
│   │   │   ├── HashTable.h
│   │   │   ├── HashTable.cpp
│   │   │   ├── BST.h
│   │   │   ├── BST.cpp
│   │   │   ├── Queue.h              
│   │   │   ├── MinHeap.h
│   │   │   └── MinHeap.cpp
│   │   │
│   │   ├── algorithms/
│   │   │   |
│   │   │   └── Ranking/
│   │   │       ├── TopK.h
│   │   │       └── TopK.cpp
│   │   │
│   │   ├── repositories/
│   │   │   |
│   │   │   ├── BookRepository.h
│   │   │   ├── BookRepository.cpp
│   │   │   ├── LoanRepository.h
│   │   │   ├── LoanRepository.cpp
│   │   │   ├── ReservationRepository.h
│   │   │   ├── ReservationRepository.cpp
│   │   │   ├── ActivityRepository.h
│   │   │   └── ActivityRepository.cpp
│   │   │
│   │   └── services/
│   │       |
│   │       ├── BorrowService.h
│   │       ├── BorrowService.cpp
│   │       ├── ReservationService.h
│   │       ├── ReservationService.cpp
│   │       ├── StatisticService.h
│   │       ├── StatisticService.cpp
│   │       ├── ActivityService.h
│   │       └── ActivityService.cpp
│   │
│   └── persistence/                    		  #Tầng data
│       ├── JsonDatabase.h
│       └── JsonDatabase.cpp
│
├── data/  #đã có
│    ├── books.json 
│    ├── members.json 
│    ├── loans.json 
│    ├── reservations.json 
│    └── activities.json 
│   
├── tests/					                            #Kiểm thử
│   ├── HashTableTest.cpp
│   ├── QueueTest.cpp    #đã có
│   ├── MinHeapTest.cpp
│   ├── BstTest.cpp
│   ├── BookTest.cpp
│   ├── BorrowServiceTest.cpp
│   ├── BookRepositoryTest.cpp
│   ├── MemberTest.cpp
│   ├── SearchServiceTest.cpp
│   └── Benchmark.cpp
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```
---

## 🔁 Luồng hệ thống

Presentation → main.cpp → Service → Repository → Persistence (JsonDatabase) → JSON
---

## 📁 Ý nghĩa từng thư mục

### docs/

- Chức năng:
Lưu toàn bộ tài liệu của dự án và phần thuyết minh đi kèm.

- Chứa:
Tài liệu D1, D2, D3, D4, sơ đồ thiết kế, báo cáo, ghi chú phân công và các tài liệu mô tả hệ thống.

- Không chứa gì:
Không chứa mã nguồn C++, không chứa giao diện, không chứa dữ liệu JSON chạy thật của hệ thống.
### src/presentation/
- Chức năng:
Là tầng giao diện của hệ thống, nơi người dùng tương tác với chương trình.

- Chứa :
Các thành phần hiển thị và điều khiển giao diện như HTML, CSS, JavaScript, nút bấm, form nhập liệu, khu vực hiển thị kết quả.

- Không chứa:
Logic nghiệp vụ như mượn/trả, đặt chỗ, thống kê; không chứa cấu trúc dữ liệu tự cài; không đọc/ghi JSON trực tiếp.
### src/dsa\_core/models/
- Chức năng:
Định nghĩa các thực thể dữ liệu trong hệ thống.

- Chứa:
Các class mô tả sách, thành viên, phiếu mượn, đặt chỗ, lịch sử hoạt động.

- Không chứa:
Thuật toán, không chứa logic nghiệp vụ, không đọc/ghi file, không xử lý giao diện.

### src/dsa\_core/structures/
- Chức năng:
Cài đặt các cấu trúc dữ liệu tự xây dựng cho bài toán.

- Chứa:
Các cấu trúc như HashTable, Queue, MinHeap, LinkedList, Node.

- Không chứa:
Giao diện, không chứa dữ liệu JSON, không viết logic nghiệp vụ cao cấp như “mượn sách” hay “top K”.
### src/dsa\_core/algorithms/
- Chức năng:
Chứa các thuật toán phục vụ cho việc xử lý dữ liệu và tối ưu bài toán.

- Chứa:
Các thuật toán tìm kiếm, ranking, xử lý truy vấn theo khoảng hoặc theo thứ tự.

- Không chứa:
Model, không chứa code giao diện, không chứa phần đọc/ghi file.

### src/dsa\_core/repositories/
- Chức năng:
Là lớp truy cập dữ liệu cho từng thực thể của hệ thống.

- Chứa:
Các hàm lấy, thêm, sửa, xóa, tìm kiếm dữ liệu cho sách, thành viên, phiếu mượn, đặt chỗ, lịch sử.

- Không chứa:
Thuật toán phức tạp, không tự xử lý giao diện, không điều khiển luồng chương trình.

### src/dsa\_core/services/
- Chức năng:
Xử lý nghiệp vụ chính của hệ thống.

- Chứa:
Các chức năng như tra cứu, mượn/trả, đặt chỗ, thống kê top K, danh sách hoạt động gần đây.

- Không chứa:
Đọc/ghi JSON trực tiếp, không cài chi tiết cấu trúc dữ liệu, không làm việc giao diện.

### src/persistence/
- Chức năng:
Quản lý việc đọc và ghi dữ liệu giữa chương trình và file JSON.

- Chứa:
Các lớp hoặc hàm làm nhiệm vụ nạp dữ liệu từ JSON, lưu dữ liệu ra JSON, chuyển đổi giữa object và dữ liệu file.

- Không chứa:
Nghiệp vụ như mượn/trả hay thống kê; không chứa giao diện; không chứa các cấu trúc dữ liệu như HashTable hay Queue.

### data/
- Chức năng:
Lưu dữ liệu thật mà hệ thống đang sử dụng.

- Chứa:
File JSON của thư viện, ví dụ dữ liệu sách, thành viên, phiếu mượn, đặt chỗ, lịch sử hoạt động.

- Không chứa:
Code C++, không chứa giao diện, không chứa tài liệu thuyết minh.
### tests/
- Chức năng:
Kiểm thử các phần quan trọng của dự án.

- Chứa:
Các file test cho cấu trúc dữ liệu, service, benchmark và các kiểm tra hiệu năng cơ bản.

- Không chứa:
Mã nguồn chạy chính thức của hệ thống, không chứa giao diện, không chứa dữ liệu thật của người dùng.
### CMakeLists.txt

- Chức năng:
Cấu hình cách build và biên dịch dự án C++.

- Chứa:
Khai báo file nguồn, thư mục include, thư viện cần liên kết, cấu hình build.

- Không chứa:
Logic nghiệp vụ, không chứa dữ liệu JSON, không chứa giao diện.
---
## Cấu trúc dữ liệu sử dụng
- HashTable: Tra cứu theo mã
- BST Truy vấn theo khoảng
- Queue Danh sách chờ
- MinHeap Top K
- LinkedList Danh sách dữ liệu
---
## Ngôn ngữ: C++17, JSON, HTML/CSS/JS, CMake
## ✅ Kỳ vọng đầu ra
Dự án sau khi hoàn thiện phải đáp ứng:

-  chạy được luồng nghiệp vụ thư viện cơ bản 
-  phân tầng rõ ràng 
-  dữ liệu đọc/ghi được
-  đầy đủ chức năng được ghi trong D2
-  có cấu trúc dữ liệu và thuật toán hợp lý
-  có test 
-  dễ demo
