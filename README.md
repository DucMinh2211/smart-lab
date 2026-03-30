# 🛡️ Smart-Lab Sentinel System (Monorepo)

**Smart-Lab Sentinel** is an integrated IoT ecosystem for laboratory security. This repository follows a **Monorepo** structure, containing both the embedded firmware and the backend infrastructure.

---

## 📂 Repository Structure
```text
smart-lab/
├── firmware/          # ESP-IDF v6.0 Firmware (C++/FreeRTOS)
│   ├── main/          # Application entry point
│   ├── components/    # Modular drivers and logic
│   └── diagram.json   # Wokwi simulation
├── backend/           # [Coming Soon] Node.js, MQTT & Database logic
├── README.md          # Global documentation
└── doc.md             # Technical system architecture
```

---

## 🚀 Getting Started

### 1. Embedded Firmware
To build the firmware for **Yolo:Bit (ESP32-S3)**:
```bash
cd firmware
idf.py set-target esp32s3
idf.py build
```

### 2. Backend (In Development)
Stay tuned for the MQTT and Cloud integration phase.

---
*Developed by the Smart-Lab Sentinel Team - CS HCMUT.*
