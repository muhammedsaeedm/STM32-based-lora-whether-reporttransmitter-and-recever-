
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

static void SPI2_GPIOInit(void)
{
    GPIO_Handle_t spi_pins;

    spi_pins.pGPIOx = BOARD_SPI_GPIO_PORT;
    spi_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    spi_pins.GPIO_PinConfig.GPIO_PinAltFunMode = BOARD_SPI_AF_MODE;
    spi_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    spi_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    spi_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

    GPIO_PeriClockControl(BOARD_SPI_GPIO_PORT, ENABLE);

    spi_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_SPI_SCLK_PIN;
    GPIO_Init(&spi_pins);

    spi_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_SPI_MOSI_PIN;
    GPIO_Init(&spi_pins);

    spi_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_SPI_MISO_PIN;
    GPIO_Init(&spi_pins);
}

static void SPI2_Init(SPI_Handle_t *pSpiHandle)
{
    pSpiHandle->pSPIx = BOARD_SPI_PERIPH;
    pSpiHandle->SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
    pSpiHandle->SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
    pSpiHandle->SPIConfig.SPI_SclkSpeed = BOARD_SPI_SPEED;
    pSpiHandle->SPIConfig.SPI_DFF = SPI_DFF_8BITS;
    pSpiHandle->SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
    pSpiHandle->SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
    pSpiHandle->SPIConfig.SPI_SSM = SPI_SSM_EN;

    SPI_Init(pSpiHandle);
}

static void USART_Print(USART_Handle_t *pUsartHandle, const char *msg)
{
    USART_SendData(pUsartHandle, (uint8_t *)msg, strlen(msg));
}

int main(void)
{
    USART_Handle_t usart2_handle;
    SPI_Handle_t spi2_handle;
    char buffer[128];

    USART2_GPIOInit();
    USART2_Init(&usart2_handle);
    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);

    SPI2_GPIOInit();
    SPI2_Init(&spi2_handle);
    SPI_SSIConfig(BOARD_SPI_PERIPH, ENABLE);

    USART_Print(&usart2_handle, "\r\n======================================================\r\n");
    USART_Print(&usart2_handle, "     STM32F446RE SPI Full-Duplex Loopback Test        \r\n");
    USART_Print(&usart2_handle, "     Connect Jumper: PB15 (MOSI) <--> PB14 (MISO)     \r\n");
    USART_Print(&usart2_handle, "======================================================\r\n\r\n");

    const char testPayload[] = "STM32 Bare-Metal SPI Loopback Success!";
    uint32_t len = strlen(testPayload);
    uint8_t rxBuffer[64] = {0};
    uint32_t iteration = 0;

    while (1)
    {
        iteration++;
        memset(rxBuffer, 0, sizeof(rxBuffer));

        SPI_PeripheralControl(BOARD_SPI_PERIPH, ENABLE);

        for (uint32_t i = 0; i < len; i++)
        {

            while (!SPI_GetFlagStatus(BOARD_SPI_PERIPH, SPI_TXE_FLAG));
            BOARD_SPI_PERIPH->DR = testPayload[i];

            while (!SPI_GetFlagStatus(BOARD_SPI_PERIPH, SPI_RXNE_FLAG));
            rxBuffer[i] = (uint8_t)BOARD_SPI_PERIPH->DR;
        }

        while (SPI_GetFlagStatus(BOARD_SPI_PERIPH, SPI_BUSY_FLAG));
        SPI_PeripheralControl(BOARD_SPI_PERIPH, DISABLE);

        uint8_t match = (memcmp(testPayload, rxBuffer, len) == 0);

        if (match)
        {
            snprintf(buffer, sizeof(buffer),
                     "[TEST #%lu PASS] Verified %lu bytes: \"%s\"\r\n",
                     iteration, len, rxBuffer);
        }
        else
        {
            snprintf(buffer, sizeof(buffer),
                     "[TEST #%lu FAIL/WAIT] Mismatch! (Check PB15 <-> PB14 jumper wire)\r\n",
                     iteration);
        }

        USART_Print(&usart2_handle, buffer);
        Board_DelayMs(1000);
    }

    return 0;
}

