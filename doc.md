# Tài liệu kỹ thuật: Hệ thống Smart-Lab Sentinel

Tài liệu này trình bày chi tiết về kiến trúc phần mềm, các quyết định thiết kế hệ thống và giải pháp cho các thách thức kỹ thuật trong dự án **Smart-Lab Sentinel**.

---

## 1. Kiến trúc Firmware (ESP-IDF & FreeRTOS)

Hệ thống được thiết kế theo mô hình **Event-Driven Multitasking** (Đa nhiệm hướng sự kiện) và kiến trúc **Layered** (Phân lớp) để tối ưu hóa tính bảo trì và khả năng mở rộng.

### 1.1. Lớp trừu tượng phần cứng (HAL)
Trái tim của hệ thống là Interface `IHardware.h`. Lớp này giúp tách biệt hoàn toàn Logic nghiệp vụ (`SmartLabCore`) khỏi các thư viện SDK cấp thấp của ESP-IDF.
*   **Lợi ích:** Cho phép thay thế driver ESP32 bằng driver giả lập trên PC (Mock) để chạy Unit Test cực nhanh mà không cần phần cứng thật.

### 1.2. Quản lý Task FreeRTOS
*   **Sense Task (P2):** Đọc dữ liệu từ cảm biến DHT22/20 và PIR mỗi 2 giây. Sử dụng các cơ chế đo thời gian chính xác để xử lý tín hiệu Bit-banging của DHT22.
*   **Emergency Task (P4):** Task có độ ưu tiên cao nhất, sẵn sàng xử lý các tình huống khẩn cấp (như xâm nhập) ngay khi nhận được tín hiệu thông qua Semaphore.
*   **Main Task:** Khởi tạo hệ thống và thiết lập liên kết giữa Logic và Hardware thông qua cơ chế Dependency Injection.

---

## 2. Giải quyết các thách thức kỹ thuật

### 2.1. Bài toán Ưu tiên lệnh (Command Priority)
*   **Thách thức:** Xung đột giữa lệnh tự động từ cảm biến và lệnh điều khiển thủ công từ người dùng.
*   **Giải pháp:** Triển khai cơ chế **Manual Override** trong `SmartLabCore`. Khi nhận lệnh thủ công, hệ thống kích hoạt trạng thái "Lock-out" trong 30 giây, bỏ qua các thay đổi tự động từ cảm biến để đảm bảo ý chí của người dùng được ưu tiên.

### 2.2. Độ ổn định của Driver DHT22
*   **Thách thức:** Tín hiệu DHT22 cực kỳ nhạy cảm với thời gian (tính bằng micro giây).
*   **Giải pháp:** Sử dụng `esp_timer_get_time()` để đo thời gian phản hồi của bit thay vì các vòng lặp `while` đơn thuần, giúp driver hoạt động ổn định trên chip ESP32-S3 có xung nhịp cao.

---

## 3. Quy trình phát triển Hybrid (Cải tiến)

Dự án sử dụng file cấu hình `.clangd` và `compile_commands.json` để đồng bộ hóa trải nghiệm lập trình trên nhiều môi trường:

1.  **Neovim (LSP Clangd):** Được cấu hình với `query-driver` để nhận diện toàn bộ Header hệ thống của ESP-IDF, đảm bảo không có lỗi đỏ khi viết code.
2.  **VS Code:** Sử dụng cấu hình `configurationProvider` từ Extension ESP-IDF chính thức để hỗ trợ IntelliSense tối đa.
3.  **Wokwi:** Sử dụng `diagram.json` khớp hoàn toàn với sơ đồ chân của Yolo:Bit (ESP32-S3), cho phép kiểm thử tương tác phần cứng ngay trên trình duyệt.

---

## 4. Cấu trúc Linh kiện (Components)

Dự án tuân thủ nghiêm ngặt mô hình Component của ESP-IDF:
*   **`components/logic`**: Chứa các thuật toán điều khiển thông minh.
*   **`components/esp32`**: Chứa driver cụ thể cho phần cứng (DHT22, DHT20, Buzzer, LED Strip).
*   **`include`**: Chứa các Interface dùng chung để kết nối các thành phần.

---
*Tài liệu được biên soạn bởi đội ngũ Smart-Lab Sentinel - CS HCMUT.*
