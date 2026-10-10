
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
    pUsartHandle->USART_Config.USART_Mode = USART_MODE_TXRX;
    pUsartHandle->USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    pUsartHandle->USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    pUsartHandle->USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(pUsartHandle);
}

static void USART_PrintString(USART_Handle_t *pUsartHandle, const char *msg)
{
    USART_SendData(pUsartHandle, (uint8_t *)msg, strlen(msg));
}

int main(void)
{
    USART_Handle_t usart2_handle;
    uint8_t rxChar;

    USART2_GPIOInit();

    USART2_Init(&usart2_handle);

    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);

    USART_PrintString(&usart2_handle, "\r\n");
    USART_PrintString(&usart2_handle, "====================================================\r\n");
    USART_PrintString(&usart2_handle, "  STM32F446RE Bare-Metal USART Driver Echo Server   \r\n");
    USART_PrintString(&usart2_handle, "  Baudrate: 115200 bps | Config: 8N1 | Target: VCP  \r\n");
    USART_PrintString(&usart2_handle, "====================================================\r\n");
    USART_PrintString(&usart2_handle, "Type any character below to test round-trip echo:\r\n> ");

    while (1)
    {

        USART_ReceiveData(&usart2_handle, &rxChar, 1);

        if (rxChar == '\r' || rxChar == '\n')
        {
            USART_PrintString(&usart2_handle, "\r\n> ");
        }
        else
        {

            USART_SendData(&usart2_handle, &rxChar, 1);
        }
    }

    return 0;
}

