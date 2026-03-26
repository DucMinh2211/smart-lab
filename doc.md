# Technical Documentation: Smart-Lab Sentinel System

Tài liệu này trình bày chi tiết về kiến trúc phần mềm, các quyết định thiết kế hệ thống và giải pháp cho các thách thức kỹ thuật trong dự án **Smart-Lab Sentinel**.

---

## 1. Kiến trúc Firmware (ESP-IDF & FreeRTOS)

Hệ thống được thiết kế theo mô hình **Event-Driven Multitasking** (Đa nhiệm hướng sự kiện) để đảm bảo tính thời gian thực (Real-time) và tối ưu hóa tài nguyên phần cứng của ESP32.

### 1.1. Quản lý Task và Độ ưu tiên (Priority Management)
Việc phân chia độ ưu tiên dựa trên tính cấp thiết của dữ liệu và thời gian đáp ứng (Response Time):

* **Emergency Task (P4 - Highest):**
    * **Trạng thái:** Thường trực ở chế độ *Blocked* để tiết kiệm CPU.
    * **Cơ chế:** Giải phóng bởi **Binary Semaphore** từ **ISR (Interrupt Service Routine)** của cảm biến PIR.
    * **Mục tiêu:** Phản hồi xâm nhập tức thì (<10ms), ngắt mọi tác vụ tính toán khác để ưu tiên báo động.
* **Network Task (P3 - High):**
    * **Nhiệm vụ:** Xử lý MQTT Stack, duy trì kết nối Wi-Fi và xử lý gói tin đến (Incoming messages).
    * **Mục tiêu:** Đảm bảo tính sẵn sàng cao (High Availability) để nhận lệnh điều khiển từ Cloud mà không bị nghẽn mạng.
* **Sense Task (P2 - Medium):**
    * **Nhiệm vụ:** Đọc dữ liệu từ DHT22 và Quang trở (LDR) theo chu kỳ.
    * **Đặc điểm:** Do cảm biến DHT22 có thời gian lấy mẫu chậm (~1-2s), task này được đặt ưu tiên trung bình để không gây hiện tượng *CPU Starvation* cho các tác vụ điều khiển.

### 1.2. Cơ chế truyền tin nội bộ (Inter-task Communication)

* **Queues (Hàng đợi):** Sử dụng để đóng gói dữ liệu cấu trúc `sensor_data_t` từ Task cảm biến sang Task mạng. Điều này giúp tách biệt hoàn toàn (Decouple) giữa logic xử lý phần cứng và logic truyền tin.
* **Mutex (Mutual Exclusion):** Sử dụng để quản lý quyền truy cập vào chân GPIO điều khiển LED RGB. Do LED bị tác động từ nhiều nguồn (Báo động, lệnh từ Web, Remote IR), Mutex đảm bảo không xảy ra hiện tượng *Race Condition* khi ghi dữ liệu phần cứng.

---

## 2. Kiến trúc Backend & Data Flow

### 2.1. Luồng dữ liệu (Data Pipeline)

1.  **Ingestion:** MQTT Broker (Mosquitto) tiếp nhận dữ liệu tập trung từ các Edge Nodes thông qua giao thức TCP/IP.
2.  **Processing:** Node.js Service (Backend) thực hiện "Subscribe", phân tích cú pháp (Parsing) dữ liệu và kích hoạt bộ quy tắc (Rule Engine).
3.  **Storage:**
    * **InfluxDB:** Lưu trữ dữ liệu thô dạng chuỗi thời gian (Time-series) phục vụ việc vẽ biểu đồ hiệu năng môi trường.
    * **Redis:** Lưu trữ trạng thái hiện tại (Current State) của thiết bị, phục vụ cơ chế **Device Shadow**.
4.  **Delivery:** Cập nhật trạng thái tức thời lên giao diện người dùng (React Frontend) thông qua giao thức **WebSockets (Socket.io)**.

### 2.2. Giải pháp Device Shadow
Để giải quyết vấn đề mạng IoT không ổn định (Unstable Network), hệ thống triển khai mô hình **Device Shadow**:
* Khi thiết bị Offline, mọi thay đổi trạng thái từ người dùng sẽ được lưu vào mục `desired_state` trong Redis.
* Ngay khi thiết bị Online lại (thông qua gói tin MQTT Connect), Backend sẽ tính toán sự chênh lệch (Delta) và đẩy lệnh cập nhật xuống thiết bị để đồng bộ hóa trạng thái ngay lập tức.

---

## 3. Giải quyết các thách thức kỹ thuật (Problem Solving)

### 3.1. Bài toán Race Condition (Xung đột điều khiển)
* **Vấn đề:** Hệ thống tự động (Auto-logic) ra lệnh tắt đèn đúng lúc người dùng (Manual) đang ra lệnh bật từ xa.
* **Giải pháp:** Áp dụng **Command Priority Table**. Lệnh từ người dùng (Manual) luôn được gắn trọng số ưu tiên cao hơn và có một khoảng thời gian "Lock-out" (ví dụ 30 giây), trong thời gian này các lệnh tự động từ cảm biến sẽ bị bỏ qua.

### 3.2. Tối ưu hóa năng lượng (Power Management)
* Sử dụng **Automatic Light Sleep** tích hợp trong ESP-IDF.
* Cấu hình Wi-Fi ở chế độ **Modem-sleep**: CPU sẽ giảm xung nhịp xuống mức tối thiểu giữa các chu kỳ DTIM (Delivery Traffic Indication Message).
* **Kết quả:** Giảm mức tiêu thụ dòng điện trung bình từ 80mA xuống còn ~25mA mà vẫn duy trì được kết nối Cloud ổn định.

---

## 4. Khả năng mở rộng & Triển khai (Scalability)
Toàn bộ môi trường Backend được đóng gói bằng **Docker**, giúp hệ thống:
* Triển khai nhất quán trên các môi trường Local, VPS hoặc Cloud.
* Dễ dàng mở rộng số lượng node cảm biến nhờ tính linh hoạt của Pub/Sub Pattern trong MQTT.

---

## 5. Quy trình Phát triển và Kiểm thử (Development & Deployment Workflow)

Dự án áp dụng quy trình kiểm thử phân lớp để đảm bảo tính ổn định của logic trước khi nạp vào phần cứng:

### 5.1. Giai đoạn Logic & Unit Test (Cross-platform)
*   **Môi trường:** **Termux (Android)**, Windows, Linux hoặc macOS.
*   **Cơ chế:** Sử dụng mã nguồn logic thuần C++ (không phụ thuộc SDK phần cứng).
*   **Công cụ:**
    *   **Trên PC:** Sử dụng **PlatformIO Native Environment** (`pio test -e native`).
    *   **Trên Termux:** Sử dụng **CMake** và **Clang** để biên dịch trực tiếp.
*   **Mục tiêu:** Kiểm tra 100% logic máy trạng thái (State Machine) và xử lý dữ liệu trước khi nạp vào chip.

### 5.2. Giai đoạn Mô phỏng (Simulation with Wokwi)
*   **Mục tiêu:** Kiểm tra tương tác phần cứng (Drivers, Interrupts, FreeRTOS Tasks).
*   **Cơ chế:** Sử dụng file `diagram.json` để thiết kế mạch ảo tương đương với **OhStem Yolo Kit**.
*   **Lợi ích:** 
    *   Kiểm thử logic đa nhiệm (Multitasking) của FreeRTOS mà môi trường Native (Phase 1) không làm được.
    *   Giả lập các điều kiện cảm biến (Nhiệt độ cao, có người xâm nhập) bằng thanh trượt trực quan.
    *   Xác nhận cấu hình chân Pin (GPIO Mapping) chính xác trước khi cắm dây thật.

### 5.3. Giai đoạn Triển khai Thực tế (Hardware Deployment)
*   **Thiết bị:** **OhStem Yolo Kit** (ESP32-S3).
*   **Công cụ:** PlatformIO CLI/IDE nạp qua USB-C.
*   **Mục tiêu:** Tối ưu hóa kết nối mạng thực tế và tinh chỉnh độ nhạy phần cứng.

---

## 6. Cấu trúc Dự án Kiến nghị (Hybrid Project Structure)

```text
smart-lab/
├── src/                   # Mã nguồn chính (Nạp cho chip & Wokwi)
├── include/               # Header files
├── test/                  # Unit tests (Chạy trên Termux/PC Native)
├── platformio.ini         # Cấu hình Build/Test/Upload (PC)
├── CMakeLists.txt         # Cấu hình Build Test cho Termux
├── diagram.json           # Sơ đồ mạch ảo Wokwi (Yolo Kit mapping)
└── docs/                  # Tài liệu hướng dẫn & Pinout
```

---
*Tài liệu được biên soạn bởi nhóm Smart-Lab Sentinel - CS HCMUT.*
