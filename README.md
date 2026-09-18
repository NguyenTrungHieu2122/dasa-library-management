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
```

```
LibraryManagement/
│
├── docs/                    # Up các tài liệu D1, D2,...
│
├── src/
│   ├── main.cpp
│   │
│   ├── presentation/        # Tầng giao diện (visualization)
│   │   ├── index.html
│   │   ├── style.css
│   │   └── script.js
│   │
│   ├── dsa_core/            # Tầng xử lý nghiệp vụ + DSA + thuật toán
│   │   ├── models/
│   │   │   ├── Book.h
│   │   │   ├── Book.cpp
│   │   │   ├── Member.h
│   │   │   ├── Member.cpp
│   │   │   ├── Loan.h
│   │   │   ├── Loan.cpp
│   │   │   ├── Reservation.h
│   │   │   ├── Reservation.cpp
│   │   │   ├── Activity.h
│   │   │   └── Activity.cpp
│   │   │
│   │   ├── structures/
│   │   │   ├── Node.h
│   │   │   ├── Node.cpp
│   │   │   ├── LinkedList.h
│   │   │   ├── LinkedList.cpp
│   │   │   ├── HashTable.h
│   │   │   ├── HashTable.cpp
│   │   │   ├── Queue.h
│   │   │   ├── Queue.cpp
│   │   │   ├── MinHeap.h
│   │   │   ├── MinHeap.cpp
│   │   │   ├── CircularQueue.h
│   │   │   └── CircularQueue.cpp
│   │   │
│   │   ├── algorithms/
│   │   │   ├── Search/
│   │   │   │   ├── Search.h
│   │   │   │   └── Search.cpp
│   │   │   ├── Sort/
│   │   │   │   ├── MergeSort.h
│   │   │   │   ├── MergeSort.cpp
│   │   │   │   ├── QuickSort.h
│   │   │   │   └── QuickSort.cpp
│   │   │   └── Ranking/
│   │   │       ├── TopK.h
│   │   │       └── TopK.cpp
│   │   │
│   │   ├── repositories/
│   │   │   ├── BookRepository.h
│   │   │   ├── BookRepository.cpp
│   │   │   ├── MemberRepository.h
│   │   │   ├── MemberRepository.cpp
│   │   │   ├── LoanRepository.h
│   │   │   ├── LoanRepository.cpp
│   │   │   ├── ReservationRepository.h
│   │   │   ├── ReservationRepository.cpp
│   │   │   ├── ActivityRepository.h
│   │   │   └── ActivityRepository.cpp
│   │   │
│   │   └── services/
│   │       ├── SearchService.h
│   │       ├── SearchService.cpp
│   │       ├── BorrowService.h
│   │       ├── BorrowService.cpp
│   │       ├── ReservationService.h
│   │       ├── ReservationService.cpp
│   │       ├── StatisticService.h
│   │       ├── StatisticService.cpp
│   │       ├── ActivityService.h
│   │       └── ActivityService.cpp
│   │
│   └── persistence/         # Tầng data
│       ├── JsonDatabase.h
│       └── JsonDatabase.cpp
│
├── data/
│   └── libraries.json
│
├── tests/                   # Kiểm thử
│   ├── HashTableTest.cpp
│   ├── QueueTest.cpp
│   ├── MinHeapTest.cpp
│   ├── SearchServiceTest.cpp
│   └── Benchmark.cpp
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## 🔁 Luồng hệ thống

presentation/index.html → script.js → main.cpp / dsa\_core → services → repositories → structures/algorithms → RAM ↔ JsonDatabase ↔ libraries.json

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
Các cấu trúc như HashTable, Queue, MinHeap, CircularQueue, LinkedList, Node.

- Không chứa:
Giao diện, không chứa dữ liệu JSON, không viết logic nghiệp vụ cao cấp như “mượn sách” hay “top K”.
### src/dsa\_core/algorithms/
- Chức năng:
Chứa các thuật toán phục vụ cho việc xử lý dữ liệu và tối ưu bài toán.

- Chứa:
Các thuật toán tìm kiếm, sắp xếp, ranking, top K, xử lý truy vấn theo khoảng hoặc theo thứ tự.

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

## 👥 Phân chia công việc

### Thành viên 1

-  MC1: tra cứu theo mã sách. 
-  Phụ trách: HashTable, SearchService, BookRepository. 

### Thành viên 2

-  MC2: duyệt / tra cứu theo khoảng. 
-  Phụ trách: cấu trúc ordered / BST nếu nhóm chốt dùng, phần logic truy vấn theo khoảng. 

### Thành viên 3

-  Yêu cầu phát hiện 1: hàng đợi mượn / trả. 
-  Phụ trách: Queue, ReservationService, ReservationRepository. 

### Thành viên 4

-  Yêu cầu phát hiện 2: top sách quan tâm. 
-  Phụ trách: MinHeap, TopK, StatisticService. 

### Thành viên 5

-  Yêu cầu phát hiện 3: danh sách mượn / trả gần đây. 
-  Phụ trách: CircularQueue, ActivityService, ActivityRepository, JsonDatabase. 

---

## ✅ Kỳ vọng đầu ra

Dự án sau khi hoàn thiện phải đáp ứng:

-  chạy được luồng nghiệp vụ thư viện cơ bản, 
-  phân tầng rõ ràng, 
-  dữ liệu đọc/ghi được, 
-  có cấu trúc dữ liệu / thuật toán hợp lý, 
-  có test, 
-  dễ demo và dễ bảo vệ. 
