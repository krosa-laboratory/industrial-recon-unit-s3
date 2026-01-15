# Power Budget Analysis & Battery Strategy
**Project:** Industrial Protocol Auditor
**Revision:** v1.1 (BOM Integration)
**Date:** 12/2025
**Status:** Verified against BOM

## 1. System Power Architecture
The system uses a 3-stage power conversion topology:
1.  **Input Stage (HV DC):** **U2 (LM2596S-5)** steps down external DC (e.g., 12V/24V) to 5V.
2.  **Battery Management:** **U1 (BQ24074)** handles Li-Po charging and **Power Path** management (prioritizes system load over battery charging).
3.  **System Rail:** **U4 (TPS63001)** Buck-Boost converter provides a stable 3.3V regardless of battery voltage (3.0V - 4.2V).

## 2. Component Current Consumption Table (3.3V Rail)

| Subsystem | Component | State | Current (Typ) | Current (Max/Peak) | Notes |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Core** | **U8** ESP32-S3 | Active (No WiFi) | 65 mA | 80 mA | 240MHz Dual Core |
| | | WiFi Tx (Burst) | 240 mA | 350 mA | RF Bursts (Short duration) |
| **Display** | **J6** TFT 1.44" RGB | Backlight 100% | 30 mA | 50 mA | *Corrected from 2.4"* |
| **RF Sub-GHz** | **J7** E07-M1101D | Rx Mode | 15 mA | 20 mA | CC1101 Module |
| | | Tx Mode (+10dBm)| 30 mA | 35 mA | |
| **Industrial** | **U6** SN65HVD230 (CAN) | Dominant (Tx) | 45 mA | 70 mA | Includes 60Ω Term. Load |
| | **U3** MAX3485 (RS485) | Transmitting | 30 mA | 50 mA | Includes 54Ω Term. Load |
| **Storage** | **U9** W25Q16 Flash | Write | 10 mA | 15 mA | SPI Active |
| **Sensors** | MAX17048 + DS3231 | Active | 0.05 mA | 0.1 mA | Negligible |
| **Actuators** | **M1** Motor DC 3V3 | Start-up / Active | 70 mA | 150 mA | **High in-rush current** |
| **TOTAL** | **(Worst Case Scenario)** | | **~535 mA** | **~790 mA** | **All Peripherals Active** |

> **Note:** The "Max/Peak" calculation assumes a momentary overlap of WiFi Tx + Motor Start + Industrial Tx. This determines the limit for U4 (TPS63001).

## 3. Operational Scenarios (Average Consumption)

Estimations based on a **2000 mAh** Li-Po Battery (Reference Value).

### Scenario A: Idle / Menu Navigation
*Screen ON (1.44" @ 50% Brightness), WiFi Idle, No Bus activity.*
- **Est. Current:** ~100 - 120 mA
- **Autonomy:** ~16 - 18 Hours

### Scenario B: Field Auditing (High Load)
*Screen ON (100%), Sniffing CAN Bus (Rx), WiFi Logging active to Cloud.*
- **Calculation:** ESP32 (100mA avg) + Screen (30mA) + CAN Rx (20mA) + Flash/Log (10mA).
- **Est. Current:** ~160 - 200 mA
- **Autonomy:** ~10 - 12 Hours

### Scenario C: Deep Sleep
*System OFF, U5 (RTC) Active, U7 (Fuel Gauge) Active, U8 (ESP32) Deep Sleep.*
- **Est. Current:** ~25 µA (ESP32) + ~5 µA (Sensors) + ~30 µA (Quiescent current of regulators)
- **Total Sleep Current:** ~60 µA
- **Shelf Life:** > 1 Year (limited by battery self-discharge).

## 4. Regulator Sizing Verification

### U4: TPS63001 (3.3V System Rail)
* **Requirement:** Must supply ~790mA peak.
* **Capability:**
    * **Buck Mode (Vin > 3.6V):** > 1000 mA output. ✅ OK.
    * **Boost Mode (Vin < 3.3V):** ~800 mA output (Derated). ⚠️ **Critical**.
* **Analysis:** The regulator is sufficient, but operates at **~98% capacity** in the worst-case scenario (Low Battery + Motor Start + WiFi Tx).
* **Firmware Mitigation:** Implement a lockout in firmware to prevent starting the **Motor (M1)** simultaneously with a **WiFi transmission** if Battery < 20%.

### U2: LM2596S-5 (Input Stage)
* **Requirement:** Supply U1 (Charger) + System Load.
* **Capability:** 3.0 A.
* **Status:** ✅ **Safe**. Plenty of headroom.

## 5. Thermal Considerations
1.  **U2 (LM2596S-5):** This component is a TO-263 package. If input voltage is high (24V) and system load is high (charging battery @ 1A + System active), it will dissipate significant heat.
    * *Action:* Ensure large copper area on PCB layer Top/Bottom connected to the tab.
2.  **U4 (TPS63001):** High current density in a small QFN/SON package.
    * *Action:* Thermal vias on the central pad are mandatory.

## 6. Layout Recommendations (Power)
* **C2 (680uF) & C5 (220uF):** Place C2 close to U2 input and C5 close to U2 output to handle ripple.
* **Motor (M1):** Ensure traces for the motor are kept away from the RF antenna path (ESP32 and CC1101) to minimize noise injection during commutation.