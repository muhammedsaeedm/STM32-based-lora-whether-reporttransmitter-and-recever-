
#ifndef INC_BOARDCONFIG_H_
#define INC_BOARDCONFIG_H_

#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"
#include "stm32f446xx_i2c_driver.h"
#include "stm32f446xx_spi_driver.h"
#include "stm32f446xx_rcc_driver.h"

#define BOARD_NUCLEO_F446RE       1
#define BOARD_NUCLEO_F401RE       2
#define BOARD_STM32F4_DISCOVERY   3

#ifndef TARGET_BOARD
#define TARGET_BOARD              BOARD_NUCLEO_F446RE
#endif

#if (TARGET_BOARD == BOARD_NUCLEO_F446RE) || (TARGET_BOARD == BOARD_NUCLEO_F401RE)
  #define BOARD_NAME              "NUCLEO-F446RE / F401RE"
  #define BOARD_LED_PORT          GPIOA
  #define BOARD_LED_PIN           GPIO_PIN_NO_5
  #define BOARD_LED_ON            GPIO_PIN_SET
  #define BOARD_LED_OFF           GPIO_PIN_RESET
#elif (TARGET_BOARD == BOARD_STM32F4_DISCOVERY)
  #define BOARD_NAME              "STM32F407G-DISC1"
  #define BOARD_LED_PORT          GPIOD
  #define BOARD_LED_PIN           GPIO_PIN_NO_12
  #define BOARD_LED_ON            GPIO_PIN_SET
  #define BOARD_LED_OFF           GPIO_PIN_RESET
#endif

#if (TARGET_BOARD == BOARD_NUCLEO_F446RE) || (TARGET_BOARD == BOARD_NUCLEO_F401RE)
  #define BOARD_BUTTON_PORT       GPIOC
  #define BOARD_BUTTON_PIN        GPIO_PIN_NO_13
  #define BOARD_BUTTON_PRESSED    0
  #define BOARD_BUTTON_RELEASED   1
  #define BOARD_BUTTON_PUPD       GPIO_NO_PUPD
  #define BOARD_BUTTON_EXTI_EDGE  GPIO_MODE_IT_FT
  #define BOARD_BUTTON_IRQ_NUM    IRQ_NO_EXTI15_10
#elif (TARGET_BOARD == BOARD_STM32F4_DISCOVERY)
  #define BOARD_BUTTON_PORT       GPIOA
  #define BOARD_BUTTON_PIN        GPIO_PIN_NO_0
  #define BOARD_BUTTON_PRESSED    1
  #define BOARD_BUTTON_RELEASED   0
  #define BOARD_BUTTON_PUPD       GPIO_NO_PUPD
  #define BOARD_BUTTON_EXTI_EDGE  GPIO_MODE_IT_RT
  #define BOARD_BUTTON_IRQ_NUM    IRQ_NO_EXTI0
#endif

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

#define LM75_SLAVE_ADDR           0x48
#define LM75_REG_TEMP             0x00
#define LM75_REG_CONF             0x01
#define LM75_REG_THYS             0x02
#define LM75_REG_TOS              0x03

#define BOARD_SPI_PERIPH          SPI2
#define BOARD_SPI_GPIO_PORT       GPIOB
#define BOARD_SPI_NSS_PIN         GPIO_PIN_NO_12
#define BOARD_SPI_SCLK_PIN        GPIO_PIN_NO_13
#define BOARD_SPI_MISO_PIN        GPIO_PIN_NO_14
#define BOARD_SPI_MOSI_PIN        GPIO_PIN_NO_15
#define BOARD_SPI_AF_MODE         5
#define BOARD_SPI_SPEED           SPI_SCLK_SPEED_DIV16

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

