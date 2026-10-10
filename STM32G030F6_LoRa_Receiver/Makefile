# STM32G030F6P6 Bare-Metal LoRa Receiver Makefile
# Target Board: Seven Semi STM32G030F6 Breakout (TSSOP-20)

# 1. Toolchain Detection
ARM_GCC_SYSTEM := $(shell which arm-none-eabi-gcc 2>/dev/null)
ARM_GCC_PIO    := /home/saeee/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc
ARM_GCC_CUBE   := $(wildcard /opt/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*/tools/bin/arm-none-eabi-gcc)

ifneq ($(ARM_GCC_SYSTEM),)
    TOOLCHAIN_BIN := $(dir $(ARM_GCC_SYSTEM))
else ifneq ($(wildcard $(ARM_GCC_PIO)),)
    TOOLCHAIN_BIN := $(dir $(ARM_GCC_PIO))
else ifneq ($(ARM_GCC_CUBE),)
    TOOLCHAIN_BIN := $(dir $(firstword $(ARM_GCC_CUBE)))
else
    TOOLCHAIN_BIN :=
endif

CC      := $(TOOLCHAIN_BIN)arm-none-eabi-gcc
OBJCOPY := $(TOOLCHAIN_BIN)arm-none-eabi-objcopy
SIZE    := $(TOOLCHAIN_BIN)arm-none-eabi-size

# 2. MCU & Compiler Flags
MCU_FLAGS := -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft
DEFINES   := -DSTM32G030xx
INCLUDES  := -I CMSIS/Device -I CMSIS/Include -I bsp/inc -I Src

CFLAGS    := $(MCU_FLAGS) $(DEFINES) $(INCLUDES) -Wall -Wextra -Og -g -ffunction-sections -fdata-sections
LDFLAGS   := $(MCU_FLAGS) -T STM32G030F6PX_FLASH.ld -Wl,--gc-sections -static --specs=nano.specs -lc -lm

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/lora_receiver.elf
BIN       := $(BUILD_DIR)/lora_receiver.bin

# 3. Source Files
SRCS := Startup/startup_stm32g030xx.s \
        CMSIS/Device/system_stm32g0xx.c \
        Src/syscalls.c \
        Src/sysmem.c \
        Src/main.c \
        bsp/src/sx1278.c

OBJS := $(patsubst %, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean size flash

all: $(TARGET) $(BIN) size

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@mkdir -p $(dir $@)
	@$(CC) $^ $(LDFLAGS) -Wl,-Map=$(BUILD_DIR)/lora_receiver.map -o $@

$(BIN): $(TARGET)
	@$(OBJCOPY) -O binary $< $@

# Rule to compile C files
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule to assemble S files
$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	@echo "AS $<"
	@$(CC) $(MCU_FLAGS) -c $< -o $@

size: $(TARGET)
	@echo "--- Firmware Memory Usage ---"
	@$(SIZE) --format=berkeley $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

flash: $(BIN)
	@echo "Flashing $(BIN) to STM32G030F6P6 @ 0x08000000 via ST-Link..."
	st-flash write $(BIN) 0x08000000
