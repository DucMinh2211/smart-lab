# 📋 Smart-Lab Sentinel - Project Roadmap

Dự án phát triển hệ thống giám sát phòng Lab thông minh sử dụng ESP32 (ESP-IDF) và mô hình Event-Driven.

---

## ✅ Phase 1: Foundation & Design (Completed)
*Mục tiêu: Thiết kế kiến trúc và chuẩn bị môi trường phát triển.*

- [x] **Technical Documentation:** Hoàn thiện tài liệu `@doc.md` và `@README.md` (English version).
- [x] **Monorepo Structure:** Tái cấu trúc dự án thành mô hình chuyên nghiệp (`firmware/`, `backend/`).
- [x] **Toolchain Setup:** Cấu hình thành công ESP-IDF v6.0 cho chip ESP32-S3 (Yolo:Bit).
- [x] **LSP Configuration:** Thiết lập `.clangd` vạn năng cho Neovim và `.vscode` cho VS Code.
- [x] **Hardware Mapping:** Xác định sơ đồ chân (Pinout) chuẩn cho OhStem Yolo:Bit.

---

## ✅ Phase 2: Core Logic & Firmware (Completed)
*Mục tiêu: Xây dựng "bộ não" của hệ thống và các Task FreeRTOS cơ bản.*

### 🛠 Logic Core (Cross-platform)
- [x] **Hardware Interface:** Định nghĩa `IHardware.h` (HAL) tách biệt hoàn toàn logic và driver.
- [x] **Command Priority Engine:** Logic `SmartLabCore` xử lý ưu tiên lệnh Manual và Auto Lock-out.

### ⚡ Firmware Implementation (ESP-IDF)
- [x] **Sense Task (P2):** Hoàn thiện Task đọc cảm biến theo chu kỳ 2s.
- [x] **Emergency Task (P4):** Thiết lập khung xử lý khẩn cấp trên FreeRTOS.
- [x] **Output Driver:** Triển khai điều khiển Buzzer (P3).
- [x] **NeoPixel Driver:** Tích hợp thư viện `led_strip` điều khiển 4 LED RGB trên board (GPIO 48).
- [x] **DHT22 Tuning:** Tối ưu hóa timing đọc dữ liệu bằng `esp_timer` cho độ chính xác cao.
- [x] **Simulation:** Chạy thành công mô phỏng toàn diện trên Wokwi.

---

## 🚀 Phase 3: Connectivity & Backend (Next Steps)
*Mục tiêu: Kết nối Cloud, lưu trữ dữ liệu và hiển thị giao diện.*

- [ ] **Wi-Fi Manager:** Triển khai kết nối Wi-Fi tự động và cơ chế tự phục hồi (Auto-reconnect).
- [ ] **MQTT Client:** Giao tiếp với Broker để gửi dữ liệu cảm biến và nhận lệnh điều khiển.
- [ ] **Backend Development:** Khởi tạo Node.js/Express service trong thư mục `backend/`.
- [ ] **Database Integration:** Thiết lập InfluxDB và Redis (Device Shadow).
- [ ] **Real-time Dashboard:** Xây dựng giao diện web hiển thị thông số môi trường.

---

## 🔭 Phase 4: Expansion & Long-term Goals
- [ ] **TinyML Integration:** Phân tích mẫu chuyển động từ PIR.
- [ ] **OTA Updates:** Nạp firmware từ xa qua Wi-Fi.
- [ ] **Security Hardening:** Áp dụng TLS/SSL cho kết nối MQTT.

---
*Cập nhật lần cuối: 30/03/2026 bởi Smart-Lab Sentinel Team.*
