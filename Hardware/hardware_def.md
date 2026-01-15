# **Hardware routing**

## ESP32-S3 I2C
- SDA: GPIO 8.
- SCL: GPIO 9.

## ESP32-S3 SPI
- MOSI: GPIO 11.
- MISO: GPIO 13.
- SCK: GPIO 12.

## W25Q16 Flash
- DO, DI, CLK: ESP32-S3 SPI.
- CS: GPIO 47.
- WP & RST: Pull Up.

## DS3231 RTC
- SDA & SCL: ESP32-S3 I2C.
- RST: GPIO 5.
- 32kHz & SQW: NC.

## MAX17048 Fuel Gauge
- SDA & SCL: ESP32-S3 I2C.

## SN65HVD230 CAN Bus Transceiver
- D: GPIO 17.
- R: GPIO 18.
- Rs: Pull Down.
- CAN High & CAN Low: Ext. Port.

## MAX3485 RS485 Driver
- RO: GPIO 16.
- DI: GPIO 15.
- RE & DE: GPIO 7.

## TFT RGB Display
- CLK & MOSI: ESP32-S3 SPI.
- RST: GPIO 14.
- DC: GPIO 6.
- BL: GPIO 38.
- CS: GPIO 10.

## RF Module E07-M1101D-SMA
- MISO, MOSI, CLK: ESP32-S3 SPI.
- GDO0: GPIO 2.
- GDO2: GPIO 1.
- CS: GPIO 48.

## Vibration Motor
- Control: GPIO 4.

## Push Switches
- UP: GPIO 39.
- DOWN: GPIO 40.
- LEFT: GPIO 41.
- RIGHT: GPIO 42.
- OK: GPIO 21.