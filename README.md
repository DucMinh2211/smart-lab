# 🛡️ Smart-Lab Sentinel System

**Smart-Lab Sentinel** is a high-performance, event-driven IoT system designed for laboratory security and environmental monitoring. Built on the **ESP-IDF** framework and **FreeRTOS**, the system ensures real-time responsiveness and robust data synchronization via a specialized **Device Shadow** mechanism.

---

## 🚀 Key Features
*   **Real-time Multitasking:** Priority-based task management (Emergency, Network, Sensing) using FreeRTOS.
*   **Intelligent Security:** Sub-10ms response time for intrusion detection via PIR interrupts.
*   **Hybrid Testing Workflow:** A unique 3-phase pipeline allowing development on **Termux (Android)**, PC simulators, and physical hardware.
*   **Device Shadowing:** Ensures state consistency between the cloud and edge devices even under unstable network conditions.
*   **Full-Stack IoT:** Integrated with a Node.js backend, MQTT broker, InfluxDB (Time-series), and Redis (Caching).

---

## 🛠️ Hybrid Development Pipeline (The "3-Phase" Approach)

This project follows a rigorous testing lifecycle to ensure logic stability before hardware deployment:

### 1. Phase 1: Logic & Unit Testing (Native/Mocking)
*   **Environment:** Termux (Android) or PC Terminal.
*   **Tools:** CMake, Clang/GCC.
*   **Goal:** Verify state machine logic and data processing using mocks without needing physical hardware.

### 2. Phase 2: Virtual Simulation (Wokwi)
*   **Environment:** PlatformIO + Wokwi Simulator.
*   **Goal:** Test peripheral drivers (DHT22, RGB LED), interrupts, and FreeRTOS task scheduling in a virtual ESP32-S3 environment.

### 3. Phase 3: Hardware Deployment
*   **Device:** **OhStem Yolo Kit** (ESP32-S3).
*   **Tools:** PlatformIO CLI/IDE.
*   **Goal:** Final performance tuning and real-world connectivity testing.

---

## 📂 Project Structure
```text
smart-lab/
├── src/               # Core application logic (Shared across all phases)
├── include/           # Header files
├── test/              # Unit tests (Unity/GTest)
├── platformio.ini     # Build config for PC (Native & ESP32-S3)
├── CMakeLists.txt     # Build config for Termux (Native)
├── diagram.json       # Wokwi simulation circuit mapping
└── backend/           # Node.js, MQTT, and Docker configuration
```

---

## 🚀 Quick Start

### For Termux (Phase 1):
```bash
cmake -B build
cmake --build build
./build/run_tests
```

### For PC/Hardware (Phase 2 & 3):
```bash
# Build and run native tests
pio test -e native

# Upload to OhStem Yolo Kit
pio run -e yolo_kit --target upload
```

---

## 🔧 Tech Stack
*   **Firmware:** ESP-IDF, FreeRTOS, C/C++.
*   **Infrastructure:** PlatformIO, CMake, Docker.
*   **Backend:** Node.js, Mosquitto (MQTT), InfluxDB, Redis.
*   **Hardware:** ESP32-S3 (OhStem Yolo Kit), DHT22, PIR Sensor, WS2812B RGB LED.

---
*Developed by the Smart-Lab Sentinel Team - CS HCMUT.*
