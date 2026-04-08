# 📋 Smart-Lab Sentinel - Lộ trình dự án

Dự án phát triển hệ thống giám sát phòng thí nghiệm thông minh sử dụng ESP32 (ESP-IDF) và mô hình hướng sự kiện (Event-Driven).

---

## ✅ Phase 1: Nền tảng & Thiết kế (Hoàn thành)
*Mục tiêu: Thiết kế kiến trúc và chuẩn bị môi trường phát triển.*

- [x] **Tài liệu kỹ thuật:** Hoàn thiện tài liệu chi tiết tại `@doc.md` và `@README.md`.
- [x] **Cấu trúc Monorepo:** Tái cấu trúc dự án theo mô hình chuyên nghiệp (`firmware/`, `backend/`).
- [x] **Thiết lập Toolchain:** Cấu hình thành công ESP-IDF v6.0 cho chip ESP32-S3 (Yolo:Bit).
- [x] **Cấu hình LSP:** Thiết lập `.clangd` vạn năng cho Neovim và `.vscode` cho VS Code.
- [x] **Sơ đồ chân (Pinout):** Xác định sơ đồ chân chuẩn cho OhStem Yolo:Bit.

---

## ✅ Phase 2: Logic Core & Firmware (Hoàn thành)
*Mục tiêu: Xây dựng "bộ não" của hệ thống và các Task FreeRTOS cơ bản.*

### 🛠 Logic Core (Đa nền tảng)
- [x] **Hardware Interface:** Định nghĩa `IHardware.h` (HAL) tách biệt hoàn toàn logic và driver.
- [x] **Engine ưu tiên lệnh:** Logic `SmartLabCore` xử lý ưu tiên lệnh Manual và Auto Lock-out.

### ⚡ Firmware Implementation (ESP-IDF)
- [x] **Sense Task (P2):** Hoàn thiện Task đọc cảm biến định kỳ (2 giây).
- [x] **Emergency Task (P4):** Thiết lập khung xử lý khẩn cấp độ ưu tiên cao trên FreeRTOS.
- [x] **Output Driver:** Triển khai điều khiển còi báo (Buzzer - P3).
- [x] **NeoPixel Driver:** Tích hợp thư viện `led_strip` điều khiển 4 LED RGB trên board (GPIO 48/P0).
- [x] **Tối ưu DHT22/20:** Sử dụng `esp_timer` để đạt độ chính xác cao khi đọc dữ liệu.
- [x] **Mô phỏng:** Chạy thành công mô phỏng toàn diện trên Wokwi.

---

## ✅ Phase 3: Custom Mode & Dual Operation (Hoàn thành)
*Mục tiêu: Triển khai 2 chế độ hoạt động (Guest-taking & Sentinel) có thể chuyển đổi động.*

### 🛠 Firmware Enhancement
- [x] **State Machine:** Xây dựng Mode State Machine trong `SmartLabCore` (Guest vs Sentinel).
- [x] **Guest Mode Logic:** Triển khai logic tự động bật đèn (PIR) và quạt (nhiệt độ cao).
- [x] **Sentinel Mode Logic:** Giữ nguyên logic cảnh báo xâm nhập hiện tại.
- [x] **Mode Transition:** Xử lý chuyển đổi mode an toàn với Semaphore và Task.
- [x] **NVS Storage:** Lưu mode hiện tại và cấu hình timer vào NVS để khôi phục sau reset.
- [x] **LED Feedback:** Thiết kế hiệu ứng LED khác biệt cho từng mode (Lighting, Warning, Emergency, Normal).
- [x] **Manual Control:** Hỗ trợ chuyển mode qua nút bấm vật lý (ISR) và Serial command.

---

## 🚀 Phase 4: Connectivity & Mobile App (Đang thực hiện)
*Mục tiêu: Kết nối hệ thống với Internet và xây dựng giao diện điều khiển.*

### 📡 Connectivity (Firmware)
- [ ] **Wi-Fi Manager:** Triển khai kết nối Wi-Fi tự động và cơ chế tự động kết nối lại.
- [ ] **MQTT Client:** Giao tiếp với Broker để:
    - **Publish:** Gửi dữ liệu cảm biến + trạng thái mode hiện tại.
    - **Subscribe:** Nhận lệnh chuyển mode và điều khiển từ app.
- [ ] **Command Handler:** Xử lý MQTT commands (`SET_MODE`, `MANUAL_OVERRIDE`, etc.).

### 🌐 Backend & Mobile App
- [ ] **Phát triển Backend:** Khởi tạo dịch vụ Node.js/Express trong thư mục `backend/`.
- [ ] **Tích hợp Database:** Thiết lập InfluxDB (dữ liệu thời gian) và Redis (Device Shadow).
- [ ] **Mobile App:** Xây dựng giao diện điều khiển (React Native/Flutter):
    - Hiển thị mode hiện tại và dữ liệu real-time.
    - Nút chuyển đổi mode (Guest ↔ Sentinel).
    - Biểu đồ nhiệt độ/độ ẩm theo thời gian.
- [ ] **Real-time Communication:** WebSocket hoặc MQTT over WebSocket cho app.

---

## 🔭 Phase 5: Mở rộng & Mục tiêu dài hạn
- [ ] **Tích hợp TinyML:** Phân tích mẫu chuyển động từ cảm biến PIR.
- [ ] **Cập nhật OTA:** Nạp firmware từ xa thông qua Wi-Fi.
- [ ] **Bảo mật hệ thống:** Áp dụng TLS/SSL cho các kết nối MQTT.

---
*Cập nhật lần cuối: 07/04/2026 bởi Smart-Lab Sentinel Team.*
