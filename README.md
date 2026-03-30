# 🛡️ Smart-Lab Sentinel System

**Smart-Lab Sentinel** is a high-performance, event-driven IoT system designed for laboratory security and environmental monitoring. Built on the **ESP-IDF v6.0** framework and **FreeRTOS**, it ensures real-time responsiveness and professional software architecture.

---

## 🚀 Key Features
*   **Real-time Multitasking:** Priority-based task management (Emergency, Sensing, Network) using FreeRTOS.
*   **HAL Architecture (Hardware Abstraction Layer):** Complete separation of application logic and hardware drivers, enabling easy device switching or running Unit Tests on a PC.
*   **Ultra-fast Response:** Intrusion detection (PIR) processing with ultra-low latency via hardware interrupts and high-priority tasks.
*   **Intelligent Logic Engine:** Automatic sensor-based control with Manual Override priority and a 30s safety lock-out mechanism.
*   **Environmental Monitoring:** Accurate real-time tracking of temperature/humidity (DHT22) and motion status.

---

## 🛠️ Hybrid Development Workflow

The project follows a rigorous 3-phase testing lifecycle to ensure maximum stability:

### 1. Phase 1: Logic & Unit Testing (Native)
*   **Goal:** Verify state machine logic (`SmartLabCore`) on a PC (Linux/Windows) using Mock Hardware without requiring a physical chip.
*   **Tools:** CMake, GCC/Clang.

### 2. Phase 2: Virtual Simulation (Wokwi)
*   **Goal:** Test peripheral drivers, interrupts, and FreeRTOS task scheduling in a virtual ESP32-S3 environment.
*   **Tools:** Wokwi Simulator (Web/CLI), `diagram.json`.

### 3. Phase 3: Hardware Deployment
*   **Device:** **Yolo:Bit V2/V3** (ESP32-S3 Chip).
*   **Tools:** ESP-IDF Framework (`idf.py`).

---

## 📂 Project Structure (ESP-IDF Component-based)
```text
smart-lab/
├── main/              # app_main() entry point and Task orchestration
├── components/        # Modularized system components:
│   ├── logic/         # Intelligence engine (SmartLabCore) - Hardware independent
│   ├── esp32/         # Physical drivers for ESP32-S3 (DHT22, GPIO, Timers)
│   └── include/       # IHardware.h Interface (The HAL Bridge)
├── build/             # Compiled artifacts and firmware binaries
├── diagram.json       # Wokwi virtual circuit mapping
└── sdkconfig          # ESP-IDF system configuration file
```

---

## 🚀 Quick Start

### Environment Setup:
```bash
# Activate ESP-IDF (Windows example)
. $HOME/esp/v6.0/esp-idf/export.ps1
```

### Build and Flash:
```bash
# Set target chip to ESP32-S3
idf.py set-target esp32s3

# Build the project
idf.py build

# Flash to device and open Monitor (Replace COMx with your port)
idf.py -p COMx flash monitor
```

---

## 🔧 Tech Stack
*   **Firmware:** ESP-IDF v6.0, FreeRTOS, C++17.
*   **LSP/IDE:** Neovim (Clangd), VS Code (ESP-IDF Extension).
*   **Simulation:** Wokwi Simulator.
*   **Hardware:** ESP32-S3 (Yolo:Bit), DHT22, PIR Sensor, Buzzer.

---
*Developed by the Smart-Lab Sentinel Team - CS HCMUT.*
