
#include "BoardConfig.h"

int main(void)
{
    GPIO_Handle_t GpioLed;
    GPIO_Handle_t GpioBtn;

    GpioLed.pGPIOx = BOARD_LED_PORT;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = BOARD_LED_PIN;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(BOARD_LED_PORT, ENABLE);
    GPIO_Init(&GpioLed);

    GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_OFF);

    GpioBtn.pGPIOx = BOARD_BUTTON_PORT;
    GpioBtn.GPIO_PinConfig.GPIO_PinNumber = BOARD_BUTTON_PIN;
    GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = BOARD_BUTTON_PUPD;

    GPIO_PeriClockControl(BOARD_BUTTON_PORT, ENABLE);
    GPIO_Init(&GpioBtn);

    uint8_t prevButtonState = BOARD_BUTTON_RELEASED;

    while (1)
    {
        uint8_t currentButtonState = GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN);

        if (prevButtonState == BOARD_BUTTON_RELEASED && currentButtonState == BOARD_BUTTON_PRESSED)
        {

            Board_DelayMs(30);

            if (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
            {

                GPIO_ToggleOutputPin(BOARD_LED_PORT, BOARD_LED_PIN);

                while (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
                {
                    Board_DelayMs(10);
                }
            }
        }

        prevButtonState = currentButtonState;
        Board_DelayMs(10);
    }

    return 0;
}

