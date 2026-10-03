# Sử dụng Ubuntu làm môi trường chạy
FROM ubuntu:22.04

# Cài đặt các công cụ biên dịch (g++, cmake, make)
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

# Đặt thư mục làm việc trong container
WORKDIR /app

# Copy toàn bộ mã nguồn vào container
COPY . .

# Biên dịch dự án bằng CMake
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --config Release

# Mở cổng (Port mặc định của Render cấp qua biến môi trường PORT)
EXPOSE 8080

# Lệnh chạy ứng dụng sau khi build xong (thay library_management bằng tên file thực thi của bạn)
CMD ["./build/library_management"]
