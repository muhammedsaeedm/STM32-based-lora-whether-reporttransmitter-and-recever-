# STM32-Based LoRa Weather Station & Telemetry System

A complete embedded IoT weather station and long-range telemetry system utilizing STM32 microcontrollers, environmental sensors, OLED display, and SX1278 LoRa transceivers (433 MHz).

---

## 📁 Repository Overview

```
├── STM32F401RE_LoRa_Weather_Node/   # Transmitter Node (NUCLEO-F401RE)
│   ├── bsp/                         # Board support packages (BMP280, SHT3x, SH1106 OLED, SX1278)
│   ├── drivers/                     # Bare-metal peripheral drivers (GPIO, I2C, SPI, USART)
│   └── Src/                         # Main transmitter firmware
│
├── STM32G030F6_LoRa_Receiver/       # Receiver Node (STM32G030F6)
│   ├── bsp/                         # LoRa SX1278 receiver driver
│   ├── CMSIS/                       # ARM CMSIS device headers
│   └── Src/                         # Main receiver firmware with UART telemetry output
│
├── STM32F446RE_Drivers/             # Custom Bare-Metal Peripheral Drivers (STM32F446RE)
│   ├── drivers/                     # GPIO, I2C, SPI, USART driver implementations
│   ├── examples/                    # Driver usage examples & test benches
│   └── docs/                        # Driver development guides & analysis reports
│
├── datasheets/                      # Sensor, MCU & LoRa module datasheets
├── demo vids/                       # Working system demo videos
├── lora_monitor.html                # Web-based serial/telemetry dashboard
└── STM32_Peripheral_Drivers_Analysis_Report.pdf
```

---

## 🌟 Key Features

- **Environmental Sensing**: Real-time temperature, humidity, and barometric pressure monitoring using **SHT3x** and **BMP280** sensors via I2C.
- **Local Visualization**: 1.3-inch SH1106 OLED display updated over SPI/I2C.
- **Long-Range Telemetry**: **SX1278 LoRa** transceiver operating at 433 MHz with configurable spreading factor, bandwidth, and coding rate.
- **Dual STM32 Architecture**:
  - **Transmitter (Tx)**: STMicroelectronics NUCLEO-F401RE (ARM Cortex-M4 @ 16 MHz).
  - **Receiver (Rx)**: STM32G030F6 (ARM Cortex-M0+), forwarding packets over UART to PC/dashboard.
- **Bare-Metal Drivers**: Custom peripheral drivers written from scratch for register-level control without bloated HAL dependencies.
- **Web Telemetry Dashboard**: `lora_monitor.html` for real-time serial packet visualization and graphing.

---

## 🛠️ Hardware Requirements

| Component | Description | Interface |
|---|---|---|
| **MCU 1 (Transmitter)** | STM32 NUCLEO-F401RE | - |
| **MCU 2 (Receiver)** | STM32G030F6 | - |
| **LoRa Transceiver** | Semtech SX1278 (433 MHz) | SPI |
| **Temperature/Humidity** | Sensirion SHT3x | I2C |
| **Pressure/Temperature** | Bosch BMP280 | I2C |
| **Display** | 1.3" 128x64 OLED (SH1106) | SPI/I2C |

---

## 🚀 Building & Flashing

### Transmitter (`STM32F401RE_LoRa_Weather_Node`)
```bash
cd STM32F401RE_LoRa_Weather_Node
make
# Flash using OpenOCD or STM32CubeProgrammer
```

### Receiver (`STM32G030F6_LoRa_Receiver`)
```bash
cd STM32G030F6_LoRa_Receiver
make
# Flash using OpenOCD or STM32CubeProgrammer
```

### Importing into STM32CubeIDE
Each project folder contains `.project` and `.cproject` files and can be directly imported via:
`File` -> `Import...` -> `General` -> `Existing Projects into Workspace`.
