# Tactical Cyber-Recon Architecture (Edge-to-C2)

A distributed, Wireless Intrusion Detection System (WIDS) and Signal Intelligence (SIGINT) platform. 

This project separates data acquisition from analysis. It utilizes an ESP32-S3 as a headless, RF edge sensor that streams raw 802.11 and BLE frames via high-speed telemetry to a Python-based Command & Control station for real-time processing, OSINT enrichment, and visualization.

## The Edge Node (Headless ESP32-S3)
A bare-metal, RTOS-driven sensor optimized for maximum radio duty-cycle and zero GUI overhead.

* **MCU:** ESP32-S3-WROOM-1 (Dual-Core Xtensa LX7)
* **Firmware:** PlatformIO / FreeRTOS (C++)
* **Architecture:**
  * **Core 0 (RF Engine):** Dedicated to hardware-level interrupts, 802.11 Promiscuous mode, CSI (Channel State Information) extraction, and passive BLE scanning.
  * **Core 1 (Telemetry Link):** Dedicated to consuming RTOS queues, serializing data into lightweight JSON, and streaming via UART at 921600 baud without blocking the radio.

## The Command & Control Station (Python GUI)
A desktop application for data ingestion, decoding, and tactical analysis.

* **Tech Stack:** Python 3, PyQt6 (UI/UX), PyQtGraph (Real-time data viz), PySerial (Async ingestion).
* **Key Capabilities:**
  * **OSINT Tracker:** Cross-references captured MAC addresses with local OUI databases to identify device manufacturers and track historical Wi-Fi probe requests.
  * **WIDS Dashboard:** Real-time detection of Deauthentication bursts, Rogue APs, and BLE saturation attacks.
  * **CSI Radar:** Generates live waterfall plots from 802.11 subcarrier perturbations to detect physical presence via microwave reflections.
  * **BLE Ecosystem Sniffer:** Parses raw Bluetooth Low Energy payloads to track corporate/IoT devices and exposes unencrypted GATT profiles.

## 🛠️ Development Philosophy & Post-Mortem
*Note: This architecture is a evolution from an initial monolithic design. By pivoting to a headless, distributed model, the system bypasses the processing and memory constraints of local displays (LVGL) and transceivers. This shift dedicates 100% of the ESP32’s compute power and RAM to raw RF acquisition, while offloading heavy OSINT and data visualization workloads to the Command & Control station.*

## ⚠️ Disclaimer
This tool is strictly for educational research, Functional Safety (FuSa) testing, and authorized auditing of Industrial Control Systems (ICS) and corporate networks.
