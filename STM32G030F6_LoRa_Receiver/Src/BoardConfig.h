
#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "stm32g030xx.h"

#define BOARD_NAME              "Seven Semi STM32G030F6 Breakout"
#define MCU_NAME                "STM32G030F6P6"
#define F_CPU                   16000000UL

#define RX_LED_PORT             GPIOB
#define RX_LED_PIN              7

#define LED_PORT                RX_LED_PORT
#define LED_PIN                 RX_LED_PIN

#define LORA_NSS_PORT           GPIOB
#define LORA_NSS_PIN            0

#define LORA_RST_PORT           GPIOA
#define LORA_RST_PIN            1

#define LORA_DIO0_PORT          GPIOA
#define LORA_DIO0_PIN           0

#define LORA_SPI_PORT           GPIOA
#define LORA_SPI_SCK_PIN        5
#define LORA_SPI_MISO_PIN       6
#define LORA_SPI_MOSI_PIN       7

#define USART_PERIPH            USART2
#define USART_PORT              GPIOA
#define USART_TX_PIN            2
#define USART_RX_PIN            3
#define USART_BAUDRATE          115200

#define LORA_FREQ_HZ            433000000UL
#define LORA_BW_HZ              125000UL
#define LORA_SF                 7
#define LORA_CR                 5
#define LORA_SYNC_WORD          0x12

static inline void Board_DelayCycles(volatile uint32_t cycles)
{
    while (cycles--) { __NOP(); }
}

static inline void Board_DelayMs(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        Board_DelayCycles(3200);
    }
}

static inline void Board_DelayUs(uint32_t us)
{
    Board_DelayCycles((us * 16) / 5);
}

#endif

