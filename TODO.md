# 📋 Smart-Lab Sentinel - Project Roadmap

Dự án phát triển hệ thống giám sát phòng Lab thông minh sử dụng ESP32 (ESP-IDF) và mô hình Event-Driven.

---

## ✅ Phase 1: Foundation & Design (Completed)
*Mục tiêu: Thiết kế kiến trúc và chuẩn bị môi trường phát triển.*

- [x] **Technical Documentation:** Hoàn thiện tài liệu `@doc.md` chi tiết về kiến trúc.
- [x] **Project Structure:** Thiết lập cấu trúc thư mục Hybrid (src, include, test, docs).
- [x] **Toolchain Setup:** Cấu hình PlatformIO (Native & ESP32) và CMake cho Termux/Linux.
- [x] **Hardware Mapping:** Xác định sơ đồ chân (Pinout) cho OhStem Yolo Kit.

---

## 🚀 Phase 2: Core Logic & Firmware (Short-term: Week 1-2)
*Mục tiêu: Xây dựng "bộ não" của hệ thống và các Task FreeRTOS cơ bản.*

### 🛠 Logic Core (Cross-platform)
- [x] **Hardware Interface:** Định nghĩa `IHardware.h` (Abstraction Layer) để tách biệt logic và driver.
- [x] **Command Priority Engine:** Code logic ưu tiên lệnh (Manual vs Auto) và cơ chế Lock-out 30s.
- [x] **Unit Testing:** Viết và chạy thành công test logic trên môi trường Native (PC/Termux).

### ⚡ Firmware Implementation (ESP-IDF)
- [ ] **Sense Task (P2):** Driver đọc DHT22 (Nhiệt độ/Độ ẩm) và LDR (Ánh sáng) qua Queue.
- [ ] **Emergency Task (P4):** Xử lý ngắt PIR (Xâm nhập) bằng Binary Semaphore (Phản hồi <10ms).
- [ ] **Output Controller:** Điều khiển LED RGB dựa trên trạng thái (Normal, Warning, Emergency) sử dụng Mutex.

---

## 🌐 Phase 3: Connectivity & Backend (Mid-term: Week 3-4)
*Mục tiêu: Kết nối Cloud, lưu trữ dữ liệu và hiển thị giao diện.*

- [ ] **Network Task (P3):** Triển khai Wi-Fi Manager và MQTT Client trên ESP32.
- [ ] **Backend Service:** Setup Node.js (Express), Mosquitto Broker và Redis (Device Shadow).
- [ ] **Database Integration:** Đẩy dữ liệu chuỗi thời gian (Time-series) vào InfluxDB.
- [ ] **Real-time Dashboard:** Xây dựng UI React đơn giản kết nối qua Socket.io để hiển thị biểu đồ và điều khiển.
- [ ] **Power Management:** Cấu hình Modem-sleep và Light-sleep để tối ưu pin.

---

## 🔭 Phase 4: Expansion & Long-term Goals
*Mục tiêu: Nâng cấp tính năng cao cấp sau khi hệ thống cơ bản đã chạy ổn định.*

- [ ] **TinyML Integration:** Sử dụng mô hình AI nhỏ để phân tích mẫu chuyển động (PIR), phân biệt người và vật nuôi/nhiễu.
- [ ] **OTA Updates:** Triển khai nạp firmware từ xa qua Wi-Fi (Over-the-Air).
- [ ] **Multi-node Mesh:** Mở rộng hệ thống thành mạng lưới nhiều node cảm biến giao tiếp qua ESP-NOW hoặc Zigbee.
- [ ] **Security Hardening:** Áp dụng TLS/SSL cho MQTT và xác thực JWT cho Dashboard.
- [ ] **Mobile App:** Phát triển ứng dụng di động (Flutter/React Native) thay thế cho Web Dashboard.

---
*Cập nhật lần cuối: 27/03/2026 bởi Smart-Lab Sentinel Team.*
