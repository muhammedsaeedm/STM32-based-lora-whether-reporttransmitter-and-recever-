
#include "BoardConfig.h"

#define PWM_PERIOD_US       2000
#define PWM_STEPS           100
#define STEP_TIME_US        (PWM_PERIOD_US / PWM_STEPS)

int main(void)
{
    GPIO_Handle_t GpioLed;

    GpioLed.pGPIOx = BOARD_LED_PORT;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = BOARD_LED_PIN;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(BOARD_LED_PORT, ENABLE);
    GPIO_Init(&GpioLed);

    int32_t dutyCycle = 0;
    int8_t stepDirection = 1;

    while (1)
    {

        for (uint32_t cycles = 0; cycles < 8; cycles++)
        {
            if (dutyCycle > 0)
            {

                GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_ON);
                Board_DelayUs(dutyCycle * STEP_TIME_US);
            }

            if (dutyCycle < PWM_STEPS)
            {

                GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_OFF);
                Board_DelayUs((PWM_STEPS - dutyCycle) * STEP_TIME_US);
            }
        }

        dutyCycle += stepDirection;

        if (dutyCycle >= PWM_STEPS)
        {
            dutyCycle = PWM_STEPS;
            stepDirection = -1;
            Board_DelayMs(50);
        }
        else if (dutyCycle <= 0)
        {
            dutyCycle = 0;
            stepDirection = 1;
            Board_DelayMs(100);
        }
    }

    return 0;
}

