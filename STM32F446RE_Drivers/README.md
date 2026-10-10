# STM32 Bare-Metal Peripheral Drivers Library & Applications
### STM32F446RE / STM32F401RE Peripheral Drivers (GPIO, USART, I2C, SPI, RCC)

A production-grade, bare-metal peripheral driver suite written completely from scratch in C for the **STM32F446RE** and **STM32F401RE** ARM Cortex-M4 microcontrollers, developed based on the **RM0390 Reference Manual** and Cortex-M4 Architecture specifications.

---

## 1. Project Overview

This library implements register-level hardware abstraction layers (HAL) for primary embedded communication and control peripherals:
- **GPIO**: Mode selection, speeds, push-pull/open-drain, internal pull-up/down, alternate functions, and EXTI interrupt handling.
- **USART**: Asynchronous/synchronous communication, custom baud rate calculation with oversampling (OVER8 / OVER16), hardware parity, frame lengths (8/9-bit), polling and interrupt-driven transfers.
- **I2C**: Master/Slave modes, standard (100 kHz) and fast mode (400 kHz), clock stretching, repeated start condition, bus error handling, polling and interrupt-driven state machines.
- **SPI**: Full-duplex, half-duplex, simplex RX, 8-bit / 16-bit data frames, software/hardware slave management (SSM/SSI), baud prescalers from DIV2 to DIV256, polling and interrupt transfers.
- **RCC**: APB1, APB2, and AHB clock frequency calculation engine.
- **Board Support Layer (`BoardConfig.h`)**: Modular pin abstraction for NUCLEO-F446RE, NUCLEO-F401RE, and STM32F4-Discovery boards.

---

## 2. Peripheral Driver Status Matrix

| Peripheral | Status | Core Capabilities |
|------------|--------|-------------------|
| **GPIO**   | ✅ Complete | Input, Output, Alt-Fn (AF0-AF15), Analog, EXTI Rising/Falling/Both, NVIC priority |
| **USART**  | ✅ Complete | TX, RX, TXRX, 1200..921600+ baud, 8/9-bit, Odd/Even parity, 1/1.5/2 Stop bits, Polling + IT |
| **I2C**    | ✅ Complete | Master TX/RX, 100k/400k SCL, 7-bit addressing, repeated start, ACK management, Polling + IT |
| **SPI**    | ✅ Complete | Master/Slave, FD/HD/Simplex, 8/16-bit DFF, CPOL/CPHA modes 0-3, SSM/SSI, Polling + IT |
| **RCC**    | ✅ Complete | Dynamic PCLK1 / PCLK2 bus frequency calculation from CFGR prescalers |

---

## 3. Directory Structure

```
STM32F446RE_Drivers/
├── Makefile                                # Multi-target bare-metal Makefile
├── STM32F446RETX_FLASH.ld                  # Linker script for Flash execution
├── STM32F446RETX_RAM.ld                    # Linker script for RAM execution
├── drivers/
│   ├── inc/
│   │   ├── BoardConfig.h                   # Board Pinout & Peripheral Mapping
│   │   ├── stm32f446xx.h                   # MCU register definitions & base addresses
│   │   ├── stm32f446xx_gpio_driver.h       # GPIO driver header & APIs
│   │   ├── stm32f446xx_usart_driver.h      # USART driver header & APIs
│   │   ├── stm32f446xx_i2c_driver.h        # I2C driver header & APIs
│   │   ├── stm32f446xx_spi_driver.h        # SPI driver header & APIs
│   │   └── stm32f446xx_rcc_driver.h        # RCC clock driver header
│   └── src/
│       ├── stm32f446xx_gpio_driver.c       # GPIO driver implementation
│       ├── stm32f446xx_usart_driver.c      # USART driver implementation
│       ├── stm32f446xx_i2c_driver.c        # I2C driver implementation
│       ├── stm32f446xx_spi_driver.c        # SPI driver implementation
│       └── stm32f446xx_rcc_driver.c        # RCC clock calculation implementation
├── examples/
│   ├── example_gpio_button_led.c           # GPIO: Debounced button toggles LD2 LED
│   ├── example_gpio_pwm_fade.c             # GPIO: Software PWM smooth LED brightness fade
│   ├── example_usart_echo.c                # USART: Interactive serial echo server @ 115200 baud
│   ├── example_usart_structured_msg.c      # USART: Telemetry frames & button event logging
│   ├── example_i2c_lm75_uart.c             # I2C: LM75 temperature reader with UART telemetry
│   ├── example_i2c_scanner.c               # I2C: 7-bit bus scanner & address mapping grid
│   ├── example_spi_test_pattern.c          # SPI: Logic analyzer test pattern generator
│   └── example_spi_loopback_verify.c       # SPI: Full-duplex loopback data integrity test
├── docs/
│   ├── Driver_Analysis_Report.pdf          # 10-page Comprehensive Driver Analysis Report
│   ├── STM32_Peripheral_Drivers_Analysis_Report.md # Full Report Markdown Source
│   ├── GPIO_Driver_Development_Guide.md
│   ├── I2C_Driver_Development_Guide.md
│   └── SPI_Driver_Development_Guide.md
├── Src/
│   ├── main.c                              # Master multi-peripheral showcase application
│   ├── syscalls.c                          # Minimal standard C library syscall stubs
│   └── sysmem.c                            # Memory management & heap boundary
└── Startup/
    └── startup_stm32f446retx.s             # Vector table & reset handler
```

---

## 4. Hardware Pin Mapping (Nucleo-F446RE / Nucleo-F401RE)

Configured via `drivers/inc/BoardConfig.h`:

| Peripheral | Board Pin / Function | Pin Name | Alternate Function | Notes |
|------------|----------------------|----------|--------------------|-------|
| **LED**    | User LED LD2 (Green) | `PA5`    | GPIO Output        | Active HIGH |
| **Button** | User Button B1 (Blue)| `PC13`   | GPIO Input         | Active LOW, internal pull-up |
| **USART2** | ST-LINK VCP TX       | `PA2`    | AF7                | 115200 baud, 8N1 |
| **USART2** | ST-LINK VCP RX       | `PA3`    | AF7                | Virtual COM Port to PC |
| **I2C1**   | SCL (Clock)          | `PB8`    | AF4                | Open-Drain, Pull-up, 100 kHz |
| **I2C1**   | SDA (Data)           | `PB9`    | AF4                | Open-Drain, Pull-up |
| **SPI2**   | Chip Select (NSS)    | `PB12`   | GPIO Output        | Manual software active-LOW CS |
| **SPI2**   | SCLK (Serial Clock)  | `PB13`   | AF5                | Up to 8 MHz (DIV2) |
| **SPI2**   | MISO (Master In)     | `PB14`   | AF5                | Data input from slave |
| **SPI2**   | MOSI (Master Out)    | `PB15`   | AF5                | Data output to slave |

*Note on Hardware Conflict Resolution*: `SPI1` uses `PA5` for `SPI1_SCK`, which is physically shared with `LD2` LED on Nucleo boards. By selecting `SPI2` (`PB12`..`PB15`), the SPI bus and the User LED operate completely independently without bus contention.

---

## 5. Build & Compilation Instructions

The project includes an intelligent `Makefile` that automatically detects the installed ARM GCC toolchain (PlatformIO, STM32CubeIDE, or system PATH):

### Build Targets:
```bash
# Build master application (main.elf & main.bin)
make all

# Build individual examples:
make gpio_button       # GPIO button input & LED toggle
make gpio_pwm          # GPIO software PWM LED fade
make usart_echo        # USART interactive character echo
make usart_msg         # USART structured telemetry frames
make i2c_temp          # I2C LM75 sensor reading + UART
make i2c_scanner       # I2C 7-bit bus scanner
make spi_test          # SPI logic analyzer test pattern
make spi_loopback      # SPI full-duplex loopback verification

# Build ALL examples in one command:
make all_examples

# Clean build artifacts:
make clean
```

All compiled binaries (`.elf`, `.bin`, `.map`) are placed in the `build/` directory.

---

## 6. Flashing & Running

### Using ST-Link Utility:
```bash
st-flash write build/main.bin 0x08000000
# or for an example:
st-flash write build/example_i2c_temp.bin 0x08000000
```

### Using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/main.elf verify reset exit"
```

### Serial Terminal Connection:
Open any serial terminal (e.g., `minicom`, `screen`, `putty`, or VS Code Serial Monitor):
- **Port**: `/dev/ttyACM0` (Linux) or `COMx` (Windows)
- **Baud Rate**: `115200`
- **Data Bits**: `8`
- **Parity**: `None`
- **Stop Bits**: `1`
- **Flow Control**: `None`
