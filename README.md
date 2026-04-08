# 🛡️ Smart-Lab Sentinel System

**Smart-Lab Sentinel** is a professional-grade, event-driven IoT ecosystem designed for laboratory security and environmental monitoring. Built on the **ESP-IDF v6.0** framework and **FreeRTOS**, the system prioritizes real-time responsiveness, modular architecture, and high reliability.

![Project Demo](demo/wokwi-demo.gif)

---

## 🚀 Key Features

*   **Real-time Multitasking:** Leverages **FreeRTOS** for priority-based task scheduling (Emergency, Sensing, and System Management).
*   **Ultra-low Latency Response:** Utilizes **Hardware Interrupts (ISR)** and **Binary Semaphores** for sub-1ms intrusion detection.
*   **Hardware Abstraction Layer (HAL):** Decouples business logic (`SmartLabCore`) from vendor-specific SDKs using a clean C++ Interface (`IHardware.h`).
*   **Dual-Mode Operation:** Seamlessly switch between **Guest-taking Mode** (comfort automation) and **Sentinel Mode** (security monitoring) via physical button (ISR) or Serial command.
*   **State Persistence:** Integrated **NVS (Non-Volatile Storage)** to remember operation modes and user configurations across reboots.
*   **Smart Logic Engine:** Automatic sensor-based control with **Manual Override** priority and a 30-second safety lock-out mechanism.
*   **Intelligent Driver Fallback:** Seamlessly toggles between I2C (DHT20) and 1-Wire (DHT22) sensors based on hardware availability.
*   **Visual Status Indicators:** Integrated 4-zone RGB NeoPixel (WS2812) feedback with distinct patterns for Normal, Warning, Emergency, and Lighting states.

---

## 📂 Monorepo Structure

```text
smart-lab/
├── firmware/              # ESP32-S3 Firmware (C++/FreeRTOS)
│   ├── main/              # Application entry and Task orchestration
│   ├── components/
│   │   ├── logic/         # Intelligence engine (Hardware Independent)
│   │   └── esp32/         # Physical drivers (DHT, PIR, Buzzer, LED Strip)
│   ├── include/           # HAL Interface definitions
│   ├── test/              # Unit tests for business logic
│   ├── diagram.json       # Wokwi simulation circuit
│   └── sdkconfig          # ESP-IDF system configuration
├── backend/               # [Upcoming] Node.js, MQTT Broker, and InfluxDB
├── README.md              # Global project documentation
└── doc.md                 # Detailed technical architecture (Vietnamese)
```

---

## 🛠️ Build Requirements

### Prerequisites
*   **ESP-IDF v6.0** (or compatible latest version).
*   **Python 3.11+** and **CMake 3.16+**.
*   **Target Hardware:** ESP32-S3 (Specifically optimized for **OhStem Yolo:Bit V2/V3**).

### Build Instructions
1.  **Environment Setup:**
    ```bash
    # Windows
    . $HOME/esp/v6.0/esp-idf/export.ps1
    # Linux/MacOS
    . $HOME/esp/v6.0/esp-idf/export.sh
    ```

2.  **Clone and Navigate:**
    ```bash
    git clone https://github.com/DucMinh2211/smart-lab.git
    cd smart-lab/firmware
    ```

3.  **Install Dependencies:**
    The project requires the official Espressif `led_strip` component.
    ```bash
    idf.py add-dependency "espressif/led_strip^3.0.0"
    ```

4.  **Build and Flash:**
    ```bash
    idf.py set-target esp32s3
    idf.py build
    # To flash (Replace COMx with your actual port)
    idf.py -p COMx flash monitor
    ```

---

## 🎯 Operating Modes

The system supports two distinct operational modes, switchable via the physical button (A), Serial commands, or the mobile app:

### 🏠 Guest-taking Mode (Comfort Automation)
*Designed for daily lab usage with automatic convenience features:*
*   **Auto-Lighting:** Automatically turns on LED strip when motion is detected (via PIR sensor).
*   **Smart Ventilation:** Activates buzzer/fan when temperature exceeds threshold (e.g., >30°C).
*   **Gentle Feedback:** Soft RGB lighting patterns indicate normal operation.
*   **Manual Toggle:** Press Button A or type `guest` in Serial to activate.

### 🛡️ Sentinel Mode (Security Monitoring)
*High-security mode for after-hours or restricted access scenarios:*
*   **Intrusion Alert:** Triggers loud alarm and emergency LED patterns when motion is detected (via ISR for <1ms latency).
*   **Environmental Warnings:** Alerts on abnormal temperature/humidity levels.
*   **Priority Response:** Emergency Task handles security events with highest priority.
*   **Manual Toggle:** Press Button A or type `sentinel` in Serial to activate.

Both modes support bidirectional communication with the mobile app for real-time monitoring and control.

---

## 🔧 Technology Stack

*   **Firmware:** ESP-IDF v6.0, FreeRTOS, C++17.
*   **Hardware:** ESP32-S3, WS2812B NeoPixel, I2C, DHT20/22, PIR Sensor.
*   **LSP/IDE:** Neovim (Clangd), VS Code (Official ESP-IDF Extension).
*   **Simulation:** Wokwi Simulator.

---

## Video Demo
### Wokwi Simulation

https://github.com/user-attachments/assets/af5ed64a-705d-4830-b602-be91f1737e7b


---
*Developed with ❤️ by the Smart-Lab Sentinel Team - CS HCMUT.*
