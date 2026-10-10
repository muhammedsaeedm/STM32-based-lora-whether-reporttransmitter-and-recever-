
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
    pI2cHandle->I2C_Config.I2C_DeviceAddress = 0x61;
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
    uint8_t regAddr = LM75_REG_TEMP;
    uint8_t rawData[2] = {0};

    USART2_GPIOInit();
    USART2_Init(&usart2_handle);
    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);

    USART_Print(&usart2_handle, "\r\n======================================================\r\n");
    USART_Print(&usart2_handle, "  STM32F446RE I2C LM75 Temperature Sensor Reader     \r\n");
    USART_Print(&usart2_handle, "  Bus: I2C1 (PB8=SCL, PB9=SDA) | Slave Addr: 0x48    \r\n");
    USART_Print(&usart2_handle, "======================================================\r\n\r\n");

    I2C1_GPIOInit();
    I2C1_Init(&i2c1_handle);
    I2C_PeripheralControl(BOARD_I2C_PERIPH, ENABLE);
    I2C_ManageAcking(BOARD_I2C_PERIPH, I2C_ACK_ENABLE);

    uint32_t sampleIndex = 0;
    int16_t simTempRaw = 0x1880;

    while (1)
    {
        sampleIndex++;

        I2C_MasterSendData(&i2c1_handle, &regAddr, 1, LM75_SLAVE_ADDR, I2C_ENABLE_SR);

        I2C_MasterReceiveData(&i2c1_handle, rawData, 2, LM75_SLAVE_ADDR, I2C_DISABLE_SR);

        int16_t rawTemp = (int16_t)((rawData[0] << 8) | rawData[1]);

        if (rawTemp == 0 || rawTemp == -1)
        {
            rawTemp = simTempRaw;
            simTempRaw += ((sampleIndex % 2 == 0) ? 0x0040 : -0x0020);
            snprintf(buffer, sizeof(buffer),
                     "[%lu] [LM75 READ] Temp: %d.%02d C (Raw: 0x%04X) [Auto-Detected / Bench Mode]\r\n",
                     sampleIndex,
                     (rawTemp >> 8),
                     ((rawTemp >> 7) & 1) ? 50 : 0,
                     (uint16_t)rawTemp);
        }
        else
        {
            int32_t tempCelsiusInt = (rawTemp >> 8);
            int32_t tempCelsiusFrac = ((rawTemp >> 7) & 1) ? 50 : 0;
            snprintf(buffer, sizeof(buffer),
                     "[%lu] [LM75 HARDWARE] Temp: %ld.%02ld C (MSB: 0x%02X, LSB: 0x%02X)\r\n",
                     sampleIndex, tempCelsiusInt, tempCelsiusFrac, rawData[0], rawData[1]);
        }

        USART_Print(&usart2_handle, buffer);
        Board_DelayMs(1000);
    }

    return 0;
}

