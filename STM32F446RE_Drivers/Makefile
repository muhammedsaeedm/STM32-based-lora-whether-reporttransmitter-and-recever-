# STM32F446RE Bare-Metal Drivers Makefile
# Automatically detects available ARM toolchain on system

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

# 2. Target MCU & Flags
MCU_FLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS    := $(MCU_FLAGS) -Wall -Wextra -O0 -g -I drivers/inc -I Src
LDFLAGS   := $(MCU_FLAGS) -T STM32F446RETX_FLASH.ld -Wl,--gc-sections -static --specs=nano.specs -lc -lm

BUILD_DIR := build

# 3. Common Driver & Core Sources
CORE_SRCS := Startup/startup_stm32f446retx.s \
             Src/syscalls.c \
             Src/sysmem.c \
             drivers/src/stm32f446xx_gpio_driver.c \
             drivers/src/stm32f446xx_rcc_driver.c \
             drivers/src/stm32f446xx_spi_driver.c \
             drivers/src/stm32f446xx_i2c_driver.c \
             drivers/src/stm32f446xx_usart_driver.c

CORE_OBJS := $(patsubst %, $(BUILD_DIR)/%.o, $(CORE_SRCS))

# 4. Applications & Examples
TARGET_MAIN := $(BUILD_DIR)/main.elf

EXAMPLES := gpio_button \
            gpio_pwm \
            usart_echo \
            usart_msg \
            i2c_temp \
            i2c_scanner \
            spi_test \
            spi_loopback

EXAMPLE_SRC_gpio_button  := examples/example_gpio_button_led.c
EXAMPLE_SRC_gpio_pwm     := examples/example_gpio_pwm_fade.c
EXAMPLE_SRC_usart_echo   := examples/example_usart_echo.c
EXAMPLE_SRC_usart_msg    := examples/example_usart_structured_msg.c
EXAMPLE_SRC_i2c_temp     := examples/example_i2c_lm75_uart.c
EXAMPLE_SRC_i2c_scanner  := examples/example_i2c_scanner.c
EXAMPLE_SRC_spi_test     := examples/example_spi_test_pattern.c
EXAMPLE_SRC_spi_loopback := examples/example_spi_loopback_verify.c

.PHONY: all clean size flash all_examples $(EXAMPLES)

all: $(TARGET_MAIN) $(BUILD_DIR)/main.bin size

$(BUILD_DIR)/main.elf: $(CORE_OBJS) $(BUILD_DIR)/Src/main.c.o
	@echo "Linking $@..."
	@$(CC) $^ $(LDFLAGS) -Wl,-Map=$(BUILD_DIR)/main.map -o $@

$(BUILD_DIR)/main.bin: $(TARGET_MAIN)
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

# Rules for building individual examples
define BUILD_EXAMPLE_RULE
$(1): $(BUILD_DIR)/example_$(1).elf $(BUILD_DIR)/example_$(1).bin
	@echo "Built example: $(1)"
	@$$(SIZE) $(BUILD_DIR)/example_$(1).elf

$$(BUILD_DIR)/example_$(1).elf: $(CORE_OBJS) $$(BUILD_DIR)/$$(EXAMPLE_SRC_$(1)).o
	@echo "Linking $$@..."
	@$$(CC) $$^ $$(LDFLAGS) -Wl,-Map=$$(BUILD_DIR)/example_$(1).map -o $$@

$$(BUILD_DIR)/example_$(1).bin: $$(BUILD_DIR)/example_$(1).elf
	@$$(OBJCOPY) -O binary $$< $$@
endef

$(foreach ex,$(EXAMPLES),$(eval $(call BUILD_EXAMPLE_RULE,$(ex))))

all_examples: all $(EXAMPLES)
	@echo "============================================="
	@echo "All applications & examples built successfully!"
	@echo "============================================="

size: $(TARGET_MAIN)
	@$(SIZE) $<

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned."

flash: $(BUILD_DIR)/main.bin
	@which st-flash >/dev/null 2>&1 && st-flash write $< 0x08000000 || \
	(which openocd >/dev/null 2>&1 && openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(TARGET_MAIN) verify reset exit") || \
	echo "ST-Link or OpenOCD not found. Connect board and flash manually."
