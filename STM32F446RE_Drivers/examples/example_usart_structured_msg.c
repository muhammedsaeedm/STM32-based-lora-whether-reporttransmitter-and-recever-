
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

static void Button_GPIOInit(void)
{
    GPIO_Handle_t btn;
    btn.pGPIOx = BOARD_BUTTON_PORT;
    btn.GPIO_PinConfig.GPIO_PinNumber = BOARD_BUTTON_PIN;
    btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    btn.GPIO_PinConfig.GPIO_PinPuPdControl = BOARD_BUTTON_PUPD;

    GPIO_PeriClockControl(BOARD_BUTTON_PORT, ENABLE);
    GPIO_Init(&btn);
}

static void LED_GPIOInit(void)
{
    GPIO_Handle_t led;
    led.pGPIOx = BOARD_LED_PORT;
    led.GPIO_PinConfig.GPIO_PinNumber = BOARD_LED_PIN;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(BOARD_LED_PORT, ENABLE);
    GPIO_Init(&led);
}

static void USART_PrintString(USART_Handle_t *pUsartHandle, const char *msg)
{
    USART_SendData(pUsartHandle, (uint8_t *)msg, strlen(msg));
}

int main(void)
{
    USART_Handle_t usart2_handle;
    char buffer[128];
    uint32_t uptimeSeconds = 0;
    uint32_t buttonPressCount = 0;
    uint8_t prevButtonState = BOARD_BUTTON_RELEASED;
    uint8_t ledState = 0;

    USART2_GPIOInit();
    USART2_Init(&usart2_handle);
    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);

    Button_GPIOInit();
    LED_GPIOInit();

    USART_PrintString(&usart2_handle, "\r\n[SYSTEM] Nucleo Telemetry System Initialized Successfully.\r\n");
    USART_PrintString(&usart2_handle, "[SYSTEM] Board: " BOARD_NAME "\r\n");
    USART_PrintString(&usart2_handle, "[SYSTEM] Monitoring User Button (PC13) and Heartbeat...\r\n\r\n");

    while (1)
    {

        uint8_t currentButtonState = GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN);

        if (prevButtonState == BOARD_BUTTON_RELEASED && currentButtonState == BOARD_BUTTON_PRESSED)
        {
            Board_DelayMs(25);
            if (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
            {
                buttonPressCount++;
                ledState = !ledState;
                GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, ledState ? BOARD_LED_ON : BOARD_LED_OFF);

                snprintf(buffer, sizeof(buffer),
                         "[EVENT] User Button PRESSED! Count = %lu | LED = %s\r\n",
                         buttonPressCount, ledState ? "ON" : "OFF");
                USART_PrintString(&usart2_handle, buffer);

                while (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
                {
                    Board_DelayMs(10);
                }
            }
        }
        prevButtonState = currentButtonState;

        static uint32_t tickMs = 0;
        Board_DelayMs(10);
        tickMs += 10;

        if (tickMs >= 1000)
        {
            tickMs = 0;
            uptimeSeconds++;
            snprintf(buffer, sizeof(buffer),
                     "[HEARTBEAT] Uptime = %lu s | Status: NORMAL | Total Presses = %lu\r\n",
                     uptimeSeconds, buttonPressCount);
            USART_PrintString(&usart2_handle, buffer);
        }
    }

    return 0;
}

