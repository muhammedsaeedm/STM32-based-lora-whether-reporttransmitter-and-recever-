# STM32F401RE Bare-Metal LoRa Weather Station Makefile

# 1. Toolchain Detection
ARM_GCC_SYSTEM := $(shell which arm-none-eabi-gcc 2>/dev/null)
ARM_GCC_PIO    := /home/saeee/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc
ARM_GCC_CUBE   := /opt/st/stm32cubeide_1.19.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.0.202410170706/tools/bin/arm-none-eabi-gcc

ifneq ($(ARM_GCC_SYSTEM),)
    TOOLCHAIN_BIN := $(dir $(ARM_GCC_SYSTEM))
else ifneq ($(wildcard $(ARM_GCC_PIO)),)
    TOOLCHAIN_BIN := $(dir $(ARM_GCC_PIO))
else ifneq ($(wildcard $(ARM_GCC_CUBE)),)
    TOOLCHAIN_BIN := $(dir $(ARM_GCC_CUBE))
else
    TOOLCHAIN_BIN :=
endif

CC      := $(TOOLCHAIN_BIN)arm-none-eabi-gcc
OBJCOPY := $(TOOLCHAIN_BIN)arm-none-eabi-objcopy
SIZE    := $(TOOLCHAIN_BIN)arm-none-eabi-size

# 2. MCU & Compiler Flags
MCU_FLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS    := $(MCU_FLAGS) -Wall -Wextra -O0 -g -I drivers/inc -I bsp/inc -I Src
LDFLAGS   := $(MCU_FLAGS) -T STM32F401RETX_FLASH.ld -Wl,--gc-sections -static --specs=nano.specs -lc -lm

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/lora_weather_node.elf
BIN       := $(BUILD_DIR)/lora_weather_node.bin

# 3. Source Files
SRCS := Startup/startup_stm32f401retx.s \
        Src/syscalls.c \
        Src/sysmem.c \
        Src/main.c \
        drivers/src/stm32f446xx_gpio_driver.c \
        drivers/src/stm32f446xx_rcc_driver.c \
        drivers/src/stm32f446xx_spi_driver.c \
        drivers/src/stm32f446xx_i2c_driver.c \
        drivers/src/stm32f446xx_usart_driver.c \
        bsp/src/sht3x.c \
        bsp/src/bmp280.c \
        bsp/src/oled_sh1106.c \
        bsp/src/sx1278.c

OBJS := $(patsubst %, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean size flash

all: $(TARGET) $(BIN) size

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@$(CC) $^ $(LDFLAGS) -Wl,-Map=$(BUILD_DIR)/lora_weather_node.map -o $@

$(BIN): $(TARGET)
	@$(OBJCOPY) -O binary $< $@

# Rule to compile C files
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule to assemble ASM files
$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	@echo "AS $<"
	@$(CC) $(MCU_FLAGS) -c $< -o $@

size: $(TARGET)
	@$(SIZE) $<

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Build cleaned."

flash: $(BIN)
	@which st-flash >/dev/null 2>&1 && st-flash write $< 0x08000000 || \
	(which openocd >/dev/null 2>&1 && openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(TARGET) verify reset exit") || \
	echo "ST-Link or OpenOCD not found. Connect board and flash manually."
