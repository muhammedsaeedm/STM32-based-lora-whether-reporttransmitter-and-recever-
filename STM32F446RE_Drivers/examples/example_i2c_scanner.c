
#include <stdio.h>
#include <string.h>
#include "BoardConfig.h"

static void USART2_GPIOInit(void)
{
    GPIO_Handle_t usart_pins;

    usart_pins.pGPIOx = BOARD_VCP_GPIO_PORT;
    usart_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_pins.GPIO_PinConfig.GPIO_PinAltFunMode = BOARD_VCP_AF_MODE;

    GPIO_PeriClockControl(BOARD_VCP_GPIO_PORT, ENABLE);

    usart_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_VCP_TX_PIN;
    GPIO_Init(&usart_pins);

    usart_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_VCP_RX_PIN;
    GPIO_Init(&usart_pins);
}

static void USART2_Init(USART_Handle_t *pUsartHandle)
{
    pUsartHandle->pUSARTx = BOARD_VCP_USART;
    pUsartHandle->USART_Config.USART_Baud = BOARD_VCP_BAUDRATE;
    pUsartHandle->USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    pUsartHandle->USART_Config.USART_Mode = USART_MODE_ONLY_TX;
    pUsartHandle->USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    pUsartHandle->USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    pUsartHandle->USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(pUsartHandle);
}

static void I2C1_GPIOInit(void)
{
    GPIO_Handle_t i2c_pins;

    i2c_pins.pGPIOx = BOARD_I2C_GPIO_PORT;
    i2c_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    i2c_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    i2c_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    i2c_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    i2c_pins.GPIO_PinConfig.GPIO_PinAltFunMode = BOARD_I2C_AF_MODE;

    GPIO_PeriClockControl(BOARD_I2C_GPIO_PORT, ENABLE);

    i2c_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_I2C_SCL_PIN;
    GPIO_Init(&i2c_pins);

    i2c_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_I2C_SDA_PIN;
    GPIO_Init(&i2c_pins);
}

static void I2C1_Init(I2C_Handle_t *pI2cHandle)
{
    pI2cHandle->pI2Cx = BOARD_I2C_PERIPH;
    pI2cHandle->I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
    pI2cHandle->I2C_Config.I2C_DeviceAddress = 0x50;
    pI2cHandle->I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    pI2cHandle->I2C_Config.I2C_SCLSpeed = BOARD_I2C_SPEED;

    I2C_Init(pI2cHandle);
}

static void USART_Print(USART_Handle_t *pUsartHandle, const char *msg)
{
    USART_SendData(pUsartHandle, (uint8_t *)msg, strlen(msg));
}

int main(void)
{
    USART_Handle_t usart2_handle;
    I2C_Handle_t i2c1_handle;
    char buffer[128];

    USART2_GPIOInit();
    USART2_Init(&usart2_handle);
    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);

    I2C1_GPIOInit();
    I2C1_Init(&i2c1_handle);
    I2C_PeripheralControl(BOARD_I2C_PERIPH, ENABLE);

    USART_Print(&usart2_handle, "\r\n======================================================\r\n");
    USART_Print(&usart2_handle, "      STM32F446RE Bare-Metal I2C Bus Scanner          \r\n");
    USART_Print(&usart2_handle, "      Probing 7-bit addresses (0x01 to 0x7F)...      \r\n");
    USART_Print(&usart2_handle, "======================================================\r\n\r\n");

    USART_Print(&usart2_handle, "     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");

    for (uint8_t row = 0; row < 0x80; row += 16)
    {
        snprintf(buffer, sizeof(buffer), "%02X: ", row);
        USART_Print(&usart2_handle, buffer);

        for (uint8_t col = 0; col < 16; col++)
        {
            uint8_t addr = row + col;

            if (addr == 0x00 || addr >= 0x78)
            {

                USART_Print(&usart2_handle, "   ");
                continue;
            }

            BOARD_I2C_PERIPH->CR1 |= (1 << I2C_CR1_START);

            uint32_t timeout = 10000;
            while (!(BOARD_I2C_PERIPH->SR1 & (1 << I2C_SR1_SB)) && --timeout);

            if (timeout == 0)
            {
                USART_Print(&usart2_handle, "TO ");
                continue;
            }

            BOARD_I2C_PERIPH->DR = (addr << 1);

            timeout = 10000;
            uint8_t detected = 0;
            while (--timeout)
            {
                if (BOARD_I2C_PERIPH->SR1 & (1 << I2C_SR1_ADDR))
                {
                    detected = 1;

                    volatile uint32_t dummy = BOARD_I2C_PERIPH->SR1;
                    dummy = BOARD_I2C_PERIPH->SR2;
                    (void)dummy;
                    break;
                }
                if (BOARD_I2C_PERIPH->SR1 & (1 << I2C_SR1_AF))
                {

                    BOARD_I2C_PERIPH->SR1 &= ~(1 << I2C_SR1_AF);
                    break;
                }
            }

            BOARD_I2C_PERIPH->CR1 |= (1 << I2C_CR1_STOP);
            Board_DelayUs(100);

            if (detected)
            {
                snprintf(buffer, sizeof(buffer), "%02X ", addr);
                USART_Print(&usart2_handle, buffer);
            }
            else
            {
                USART_Print(&usart2_handle, "-- ");
            }
        }
        USART_Print(&usart2_handle, "\r\n");
    }

    USART_Print(&usart2_handle, "\r\n[SCAN COMPLETE] Test complete.\r\n");

    while (1)
    {
        Board_DelayMs(1000);
    }

    return 0;
}

