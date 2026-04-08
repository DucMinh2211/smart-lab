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

### 1.3. Dual-Mode Operation (Custom Mode)
Hệ thống hỗ trợ 2 chế độ hoạt động có thể chuyển đổi động qua ứng dụng di động:

#### 🏠 Guest-taking Mode (Chế độ Phục vụ)
*Tối ưu cho sử dụng thường ngày với các tính năng tiện ích tự động:*
*   **Chiếu sáng tự động:** Bật đèn LED khi phát hiện chuyển động (cảm biến PIR).
*   **Thông gió thông minh:** Kích hoạt quạt/buzzer khi nhiệt độ vượt ngưỡng (VD: >30°C).
*   **Phản hồi nhẹ nhàng:** Hiệu ứng LED RGB dịu nhẹ cho biết hoạt động bình thường.
*   **Ghi log dữ liệu:** Liên tục gửi thông tin môi trường về app.

#### 🛡️ Sentinel Mode (Chế độ Canh gác)
*Chế độ bảo mật cao cho thời gian ngoài giờ hoặc khu vực hạn chế:*
*   **Cảnh báo xâm nhập:** Kích hoạt còi báo động và LED khẩn cấp khi phát hiện chuyển động.
*   **Cảnh báo môi trường:** Thông báo khi nhiệt độ/độ ẩm bất thường.
*   **Xử lý ưu tiên:** Emergency Task xử lý sự kiện bảo mật với độ ưu tiên cao nhất.
*   **Điều khiển từ xa:** Cho phép override thủ công và chuyển mode từ app.

---

## 2. Giải quyết các thách thức kỹ thuật

### 2.1. Bài toán Ưu tiên lệnh (Command Priority)
*   **Thách thức:** Xung đột giữa lệnh tự động từ cảm biến và lệnh điều khiển thủ công từ người dùng.
*   **Giải pháp:** Triển khai cơ chế **Manual Override** trong `SmartLabCore`. Khi nhận lệnh thủ công, hệ thống kích hoạt trạng thái "Lock-out" trong 30 giây, bỏ qua các thay đổi tự động từ cảm biến để đảm bảo ý chí của người dùng được ưu tiên.

### 2.2. Quản lý chế độ hoạt động (Mode Management)
*   **Thách thức:** Chuyển đổi hành vi hệ thống giữa 2 mode (Guest-taking vs Sentinel) mà không cần reset thiết bị và phải ghi nhớ mode sau khi mất điện.
*   **Giải pháp:** 
    *   Sử dụng **State Machine** trong `SmartLabCore` để quản lý mode hiện tại.
    *   **NVS Storage (`ModeStorage`):** Sử dụng phân vùng Non-Volatile Storage để lưu trữ `OperationMode` và các tham số cấu hình (như `autoLightDuration`). Hệ thống sẽ tự động khôi phục mode gần nhất khi khởi động lại.
    *   **Cơ chế kích hoạt:**
        *   **Nút bấm vật lý (ISR):** Sử dụng ngắt cạnh xuống (Falling Edge) trên nút A (GPIO 0) để chuyển đổi mode tức thì. Semaphore được dùng để đánh thức `mode_button_task` xử lý debounce và chuyển đổi mode an toàn.
        *   **Serial Interface:** Hỗ trợ tập lệnh Serial (`guest`, `sentinel`, `timer <sec>`) giúp kỹ thuật viên cấu hình hệ thống nhanh chóng qua cổng USB-C.
        *   **MQTT (Sắp ra mắt):** Cho phép thay đổi mode từ xa qua ứng dụng di động.

### 2.3. Độ ổn định và Phản hồi thời gian thực
*   **Thách thức:** Đảm bảo phản hồi xâm nhập (PIR) đạt độ trễ thấp nhất trong khi vẫn duy trì độ ổn định của driver DHT22 vốn rất nhạy cảm với thời gian.
*   **Giải pháp:** 
    *   Sử dụng `esp_timer_get_time()` để đo thời gian phản hồi của bit DHT22 chính xác đến từng micro giây.
    *   Sử dụng **Hardware Interrupt (ISR)** cho cảm biến PIR. Thay vì polling, ISR sẽ giải phóng một Semaphore để đánh thức `emergency_task` (độ ưu tiên cao nhất - P5), đảm bảo còi báo động vang lên gần như tức thì khi có xâm nhập.

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

## 5. Sơ đồ chân (Pinout)
| Tên chân (Board) | Yolo:Bit V2 (ESP32) | Yolo:Bit S3 (ESP32-S3) | Chức năng chính |
| ---------------- | ------------------- | ---------------------- | --------------- |
| P0               | GPIO 26             | GPIO 1                 | ADC / Touch     |
| P1               | GPIO 25             | GPIO 2                 | ADC / Touch     |
| P2               | GPIO 34             | GPIO 3                 | ADC             |
| P8               | GPIO 5              | GPIO 10                | Digital         |
| P10              | GPIO 27             | GPIO 12                | PWM (Servo)     |
| P12              | GPIO 18             | GPIO 13                | Digital         |
| P13              | GPIO 19             | GPIO 14                | Digital         |
| P14              | GPIO 23             | GPIO 15                | Digital         |
| P15              | GPIO 15             | GPIO 16                | Digital         |
| P16              | GPIO 13             | GPIO 17                | Digital         |
| SDA (I2C)        | GPIO 21             | GPIO 41                | I2C Data        |
| SCL (I2C)        | GPIO 22             | GPIO 42                | I2C Clock       |
| Ma trận LED      | GPIO 4              | GPIO 48                | 25 LED RGB      |
| Buzzer           | GPIO 14             | GPIO 38                | Âm thanh        |
| Nút A / B        | 35 / 0              | 0 / 47                 | Input           |

### Ohstem Pinout (Dành cho Wokwi)

|Tên chân (OhStem)|GPIO (Số dùng trong code/Wokwi)|Chức năng / Ghi chú                            |
|-----------------|-------------------------------|-----------------------------------------------|
|pin0             |32                             |Analog In / Digital                            |
|pin1             |33                             |Analog In / Digital                            |
|pin2             |27 (hoặc 39)                   |Thường dùng cho Servo, LED                     |
|pin3             |2                              |Digital / Chân LED xanh dương trên ESP32 DevKit|
|pin4             |15                             |Digital                                        |
|pin5             |35                             |Nút nhấn A (Chỉ Input - không xuất Output được)|
|pin6             |12                             |Nút nhấn B (Input/Output)                      |
|pin7             |25                             |Thường dùng cho DHT / Cảm biến độ ẩm đất       |
|pin10            |26                             |Digital                                        |
|pin11            |14                             |Buzzer (Còi báo động onboard)                  |
|pin12            |13                             |Digital                                        |
|pin13            |18                             |Digital / Chân SCK của SPI                     |
|pin14            |19                             |Digital / Chân MISO của SPI                    |
|pin15            |23                             |Digital / Chân MOSI của SPI                    |
|pin16            |5                              |Digital / CS của SPI                           |
|pin19            |22                             |SCL (Chuẩn I2C - Màn hình OLED)                |
|pin20            |21                             |SDA (Chuẩn I2C - Màn hình OLED)                |

|Tên chân|Bản V4 trở lên (GPIO)|Bản cũ (GPIO)                                  |
|--------|---------------------|-----------------------------------------------|
|pin8    |18 (giống pin13)     |17                                             |
|pin9    |23 (giống pin15)     |16                                             |
---
*Tài liệu được biên soạn bởi đội ngũ Smart-Lab Sentinel - CS HCMUT.*
