# 📋 Smart-Lab Sentinel - Project Roadmap

Dự án phát triển hệ thống giám sát phòng Lab thông minh sử dụng ESP32 (ESP-IDF) và mô hình Event-Driven.

---

## ✅ Phase 1: Foundation & Design (Completed)
*Mục tiêu: Thiết kế kiến trúc và chuẩn bị môi trường phát triển.*

- [x] **Technical Documentation:** Hoàn thiện tài liệu `@doc.md` chi tiết về kiến trúc.
- [x] **Project Structure:** Thiết lập cấu trúc thư mục Hybrid chuẩn ESP-IDF (main, components).
- [x] **Toolchain Setup:** Cấu hình thành công ESP-IDF v6.0 cho chip ESP32-S3 (Yolo:Bit).
- [x] **LSP Configuration:** Thiết lập `.clangd` và `.vscode` để nhận diện FreeRTOS/ESP-IDF headers.
- [x] **Hardware Mapping:** Xác định sơ đồ chân (Pinout) cho OhStem Yolo Kit / Yolo:Bit.

---

## 🚀 Phase 2: Core Logic & Firmware (Completed - Refining)
*Mục tiêu: Xây dựng "bộ não" của hệ thống và các Task FreeRTOS cơ bản.*

### 🛠 Logic Core (Cross-platform)
- [x] **Hardware Interface:** Định nghĩa `IHardware.h` (Abstraction Layer) để tách biệt logic và driver.
- [x] **Command Priority Engine:** Code logic `SmartLabCore` ưu tiên lệnh (Manual vs Auto) và cơ chế Lock-out 30s.
- [ ] **Unit Testing:** Cần cấu hình môi trường test trên Native (PC) tương ứng với cấu trúc CMake mới.

### ⚡ Firmware Implementation (ESP-IDF)
- [x] **Sense Task (P2):** Hoàn thiện Task đọc DHT22 (Nhiệt độ/Độ ẩm) và PIR (Chuyển động) theo chu kỳ 2s.
- [x] **Emergency Task (P4):** Thiết lập khung xử lý khẩn cấp (Emergency Task) trên FreeRTOS.
- [x] **Output Driver:** Triển khai điều khiển Buzzer (P3) và Log trạng thái LED RGB (GPIO 48).
- [x] **Simulation:** Chạy thành công mô phỏng toàn diện trên Wokwi (Chip ESP32-S3).
- [ ] **DHT22 Tuning:** Tối ưu timing đọc DHT22 để tránh lỗi 0/0 trên chip thực tế (Đang thực hiện).

---

## 🌐 Phase 3: Connectivity & Backend (Next Steps)
*Mục tiêu: Kết nối Cloud, lưu trữ dữ liệu và hiển thị giao diện.*

- [ ] **Wi-Fi Manager:** Triển khai kết nối Wi-Fi tự động và lưu cấu hình vào NVS.
- [ ] **MQTT Client:** Giao tiếp với Broker để gửi dữ liệu cảm biến và nhận lệnh điều khiển.
- [ ] **NeoPixel Driver:** Triển khai thư viện `led_strip` để điều khiển LED RGB trên board sáng thật sự.
- [ ] **Real-time Dashboard:** Xây dựng UI đơn giản (Web/Mobile) để hiển thị biểu đồ nhiệt độ/độ ẩm.

---

## 🔭 Phase 4: Expansion & Long-term Goals
- [ ] **TinyML Integration:** Phân tích mẫu chuyển động (PIR).
- [ ] **OTA Updates:** Nạp firmware từ xa qua Wi-Fi.
- [ ] **Security Hardening:** Áp dụng TLS/SSL cho MQTT.

---
*Cập nhật lần cuối: 29/03/2026 bởi Smart-Lab Sentinel Team.*
