# Industrial Recon Unit (ESP32-S3)

Portable Cyber-Physical Systems (CPS) auditing tool.

## Hardware Specs
* **MCU:** ESP32-S3-WROOM-1 (N16R8)
* **Display:** 1.44" TFT SPI (ST7735/ILI9341)
* **Interfaces:** CAN (Transceiver TBD), RS485 (Auto-flow), SD Card (SPI/SDMMC).

## Architecture
* **Core 0:** Protocol Sniffing (High Priority, Real-time).
* **Core 1:** User Interface (LVGL) & SD Storage (Background).
* **Memory:** 8MB Octal PSRAM used as a giant Ring Buffer.

## Disclaimer
This tool is for educational and authorized auditing purposes only.
