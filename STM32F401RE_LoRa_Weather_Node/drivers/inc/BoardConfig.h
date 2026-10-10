
#ifndef INC_BOARDCONFIG_H_
#define INC_BOARDCONFIG_H_

#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"
#include "stm32f446xx_i2c_driver.h"
#include "stm32f446xx_spi_driver.h"
#include "stm32f446xx_rcc_driver.h"

#define BOARD_NAME                "NUCLEO-F401RE"
#define MCU_NAME                  "STM32F401RET6"

#define BOARD_LED_PORT            GPIOA
#define BOARD_LED_PIN             GPIO_PIN_NO_5
#define BOARD_LED_ON              GPIO_PIN_SET
#define BOARD_LED_OFF             GPIO_PIN_RESET

#define BOARD_BUTTON_PORT         GPIOC
#define BOARD_BUTTON_PIN          GPIO_PIN_NO_13
#define BOARD_BUTTON_PRESSED      1
#define BOARD_BUTTON_RELEASED     0
#define BOARD_BUTTON_PUPD         GPIO_NO_PUPD

#define BOARD_VCP_USART           USART2
#define BOARD_VCP_GPIO_PORT       GPIOA
#define BOARD_VCP_TX_PIN          GPIO_PIN_NO_2
#define BOARD_VCP_RX_PIN          GPIO_PIN_NO_3
#define BOARD_VCP_AF_MODE         7
#define BOARD_VCP_BAUDRATE        USART_STD_BAUD_115200

#define BOARD_I2C_PERIPH          I2C1
#define BOARD_I2C_GPIO_PORT       GPIOB
#define BOARD_I2C_SCL_PIN         GPIO_PIN_NO_8
#define BOARD_I2C_SDA_PIN         GPIO_PIN_NO_9
#define BOARD_I2C_AF_MODE         4
#define BOARD_I2C_SPEED           I2C_SCL_SPEED_SM

#define SHT3X_I2C_ADDR_A          0x44
#define SHT3X_I2C_ADDR_B          0x45

#define BMP280_I2C_ADDR_A         0x76
#define BMP280_I2C_ADDR_B         0x77

#define OLED_I2C_ADDR_A           0x3C
#define OLED_I2C_ADDR_B           0x3D

#define LORA_SPI_PERIPH           SPI2
#define LORA_SPI_GPIO_PORT        GPIOB
#define LORA_SPI_SCLK_PIN         GPIO_PIN_NO_13
#define LORA_SPI_MISO_PIN         GPIO_PIN_NO_14
#define LORA_SPI_MOSI_PIN         GPIO_PIN_NO_15
#define LORA_SPI_AF_MODE          5
#define LORA_SPI_SPEED            SPI_SCLK_SPEED_DIV8

#define LORA_NSS_PORT             GPIOB
#define LORA_NSS_PIN              GPIO_PIN_NO_12

#define LORA_RST_PORT             GPIOA
#define LORA_RST_PIN              GPIO_PIN_NO_9

#define LORA_DIO0_PORT            GPIOA
#define LORA_DIO0_PIN             GPIO_PIN_NO_10

#define LORA_DEFAULT_FREQ_HZ      433000000UL
#define LORA_DEFAULT_TX_POWER     17
#define LORA_DEFAULT_SF           7
#define LORA_DEFAULT_BW           125000
#define LORA_DEFAULT_CR           5

static inline void Board_DelayMs(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        for (volatile uint32_t count = 0; count < 2666; count++)
        {
            __asm__("nop");
        }
    }
}

static inline void Board_DelayUs(uint32_t us)
{
    for (uint32_t i = 0; i < us; i++)
    {
        for (volatile uint32_t count = 0; count < 3; count++)
        {
            __asm__("nop");
        }
    }
}

#endif

