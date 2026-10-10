# STM32F401RE Bare-Metal LoRa Weather Station & Telemetry Node

A complete bare-metal embedded IoT weather station engineered for the **STMicroelectronics NUCLEO-F401RE** board. It interfaces two environmental sensors over I2C, drives a 1.3-inch 128x64 OLED display, and broadcasts real-time telemetry over long-range radio (LoRa) at 433 MHz.

---

## 1. System Architecture

```
                    +-------------------------------------------------------+
                    |                 NUCLEO-F401RE (MCU)                   |
                    |             ARM Cortex-M4 @ 16 MHz HSI                |
                    +-------------------------------------------------------+
                           |                    |                     |
               I2C1 (PB8/PB9)          SPI2 (PB12-PB15)        USART2 (PA2/PA3)
                     |                          |                     |
     +---------------+---------------+          |                     v
     |               |               |          |             ST-LINK USB VCP
     v               v               v          v             (115200 baud PC)
 [ SHT3x ]      [ BMP280 ]      [ 1.3" OLED ] [ SX1278 ]
(Temp & Hum)  (Press & Alt)    (SH1106 128x64) (Ra-02 LoRa)
```

---

## 2. Hardware Wiring Guide

### A. I2C1 Bus (Sensors & OLED Display)
All 3 devices share the same 2-wire bus on the Nucleo board:

| Device | Signal | Nucleo-F401RE Pin | Notes |
|--------|--------|-------------------|-------|
| **SHT3x / BMP280 / OLED** | `VCC` | `3V3` | 3.3V Power rail |
| **SHT3x / BMP280 / OLED** | `GND` | `GND` | Ground rail |
| **SHT3x / BMP280 / OLED** | `SCL` | `PB8` | Arduino D15 (I2C1_SCL, AF4) |
| **SHT3x / BMP280 / OLED** | `SDA` | `PB9` | Arduino D14 (I2C1_SDA, AF4) |

- **SHT3x Address:** `0x44` (ADDR pin to GND)
- **BMP280 Address:** `0x76` (SDO pin to GND)
- **1.3" OLED Address:** `0x3C` (SH1106 controller with 2-pixel column offset)

### B. SPI2 Bus (Semtech SX1278 / Ra-02 LoRa Transceiver)

| LoRa Module Pin | Nucleo-F401RE Pin | Morpho / Arduino | Description |
|-----------------|-------------------|------------------|-------------|
| `VCC` | `3V3` | Power | 3.3V Power (Do NOT connect to 5V!) |
| `GND` | `GND` | Power | Ground |
| `SCK` | `PB13` | Morpho CN10-30 | SPI2 Serial Clock (AF5) |
| `MISO` | `PB14` | Morpho CN10-28 | SPI2 Master In Slave Out (AF5) |
| `MOSI` | `PB15` | Morpho CN10-26 | SPI2 Master Out Slave In (AF5) |
| `NSS` (CS) | `PB12` | Morpho CN10-16 | Chip Select (Active LOW GPIO) |
| `RST` | `PA9` | Arduino D8 | Hardware Reset (Active LOW GPIO) |
| `DIO0` | `PA10` | Arduino D2 | Packet Tx/Rx Done Interrupt |

### C. Onboard Peripherals
- **User LED (LD2):** `PA5` (Blinks during LoRa packet transmission)
- **User Button (B1):** `PC13` (Press to trigger an immediate manual broadcast)
- **Serial Telemetry:** `PA2` (TX), `PA3` (RX) via ST-LINK USB cable at **115200 baud**

---

## 3. Telemetry Packet Protocol

Every 2 seconds (or immediately when the user button is pressed), a telemetry frame is transmitted over LoRa and logged to the serial terminal:

```
NODE:01|T:24.5C|H:52.3%|P:1013.2hPa|ALT:118m|SEQ:12
```

- **Frequency:** 433.0 MHz (Ra-02 standard)
- **Bandwidth:** 125 kHz
- **Spreading Factor:** SF7
- **Coding Rate:** 4/5
- **Output Power:** +17 dBm (PA_BOOST)

---

## 4. 1.3" OLED Display Layout

The SH1106 driver renders a visual dashboard:
```
+---------------------------------------+
| WEATHER NODE #012                     | <- Inverse Title Bar
+---------------------------------------+
| Temp: 24.5 C                          | <- SHT3x Temperature
| Hum : 52.3 %                          | <- SHT3x Relative Humidity
| Pres: 1013.2 hPa                      | <- BMP280 Barometric Pressure
| Alt : 118 m                           | <- Calculated Altitude
|---------------------------------------|
| LoRa TX OK (433MHz)                   | <- Transmission Status
+---------------------------------------+
```

---

## 5. Build & Flash Instructions

### Build Project:
```bash
cd /home/saeee/STM32F401RE_LoRa_Weather_Node
make
```

### Clean Build:
```bash
make clean
```

### Flash using ST-Link Utility:
```bash
st-flash write build/lora_weather_node.bin 0x08000000
```

### Flash using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/lora_weather_node.elf verify reset exit"
```

### View Live Telemetry:
Connect your Nucleo board via USB and open a serial terminal:
```bash
minicom -D /dev/ttyACM0 -b 115200
# or
screen /dev/ttyACM0 115200
```
