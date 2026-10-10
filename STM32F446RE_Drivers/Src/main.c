
#include <stdio.h>
#include <string.h>
#include "BoardConfig.h"

static USART_Handle_t g_usart2_handle;
static SPI_Handle_t   g_spi2_handle;
static I2C_Handle_t   g_i2c1_handle;
static GPIO_Handle_t  g_led_handle;
static GPIO_Handle_t  g_btn_handle;

static void System_GPIO_Init(void)
{

    g_led_handle.pGPIOx = BOARD_LED_PORT;
    g_led_handle.GPIO_PinConfig.GPIO_PinNumber = BOARD_LED_PIN;
    g_led_handle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    g_led_handle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    g_led_handle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    g_led_handle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(BOARD_LED_PORT, ENABLE);
    GPIO_Init(&g_led_handle);
    GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_OFF);

    g_btn_handle.pGPIOx = BOARD_BUTTON_PORT;
    g_btn_handle.GPIO_PinConfig.GPIO_PinNumber = BOARD_BUTTON_PIN;
    g_btn_handle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    g_btn_handle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    g_btn_handle.GPIO_PinConfig.GPIO_PinPuPdControl = BOARD_BUTTON_PUPD;

    GPIO_PeriClockControl(BOARD_BUTTON_PORT, ENABLE);
    GPIO_Init(&g_btn_handle);
}

static void System_USART2_Init(void)
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

    g_usart2_handle.pUSARTx = BOARD_VCP_USART;
    g_usart2_handle.USART_Config.USART_Baud = BOARD_VCP_BAUDRATE;
    g_usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    g_usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    g_usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    g_usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    g_usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&g_usart2_handle);
    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);
}

static void System_SPI2_Init(void)
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

    spi_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    spi_pins.GPIO_PinConfig.GPIO_PinNumber = BOARD_SPI_NSS_PIN;
    GPIO_Init(&spi_pins);
    GPIO_WriteToOutputPin(BOARD_SPI_GPIO_PORT, BOARD_SPI_NSS_PIN, GPIO_PIN_SET);

    g_spi2_handle.pSPIx = BOARD_SPI_PERIPH;
    g_spi2_handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
    g_spi2_handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
    g_spi2_handle.SPIConfig.SPI_SclkSpeed = BOARD_SPI_SPEED;
    g_spi2_handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
    g_spi2_handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
    g_spi2_handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
    g_spi2_handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

    SPI_Init(&g_spi2_handle);
    SPI_SSIConfig(BOARD_SPI_PERIPH, ENABLE);
}

static void System_I2C1_Init(void)
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

    g_i2c1_handle.pI2Cx = BOARD_I2C_PERIPH;
    g_i2c1_handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
    g_i2c1_handle.I2C_Config.I2C_DeviceAddress = 0x61;
    g_i2c1_handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    g_i2c1_handle.I2C_Config.I2C_SCLSpeed = BOARD_I2C_SPEED;

    I2C_Init(&g_i2c1_handle);
    I2C_PeripheralControl(BOARD_I2C_PERIPH, ENABLE);
    I2C_ManageAcking(BOARD_I2C_PERIPH, I2C_ACK_ENABLE);
}

static void UART_Log(const char *msg)
{
    USART_SendData(&g_usart2_handle, (uint8_t *)msg, strlen(msg));
}

int main(void)
{
    char logBuf[128];
    uint32_t cycleCount = 0;
    uint8_t prevBtnState = BOARD_BUTTON_RELEASED;
    uint8_t ledState = 0;

    System_GPIO_Init();
    System_USART2_Init();
    System_SPI2_Init();
    System_I2C1_Init();

    UART_Log("\r\n=================================================================\r\n");
    UART_Log("       STM32 BARE-METAL MULTI-PERIPHERAL DRIVER SHOWCASE         \r\n");
    UART_Log("       Target: " BOARD_NAME " (ARM Cortex-M4 @ 16 MHz)           \r\n");
    UART_Log("       Peripherals: GPIO | USART | SPI | I2C Active              \r\n");
    UART_Log("=================================================================\r\n\r\n");

    const uint8_t spiPreamble[] = {0xA5, 0x5A, 0x01, 0x02, 0x03, 0x04};

    while (1)
    {
        cycleCount++;

        uint8_t currBtnState = GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN);
        if (prevBtnState == BOARD_BUTTON_RELEASED && currBtnState == BOARD_BUTTON_PRESSED)
        {
            Board_DelayMs(20);
            if (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
            {
                ledState = !ledState;
                GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, ledState ? BOARD_LED_ON : BOARD_LED_OFF);
                UART_Log("[GPIO EVENT] User Button Pressed -> Toggled LD2 LED!\r\n");
                while (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
                {
                    Board_DelayMs(10);
                }
            }
        }
        prevBtnState = currBtnState;

        if (cycleCount % 100 == 0)
        {

            GPIO_ToggleOutputPin(BOARD_LED_PORT, BOARD_LED_PIN);

            SPI_PeripheralControl(BOARD_SPI_PERIPH, ENABLE);
            GPIO_WriteToOutputPin(BOARD_SPI_GPIO_PORT, BOARD_SPI_NSS_PIN, GPIO_PIN_RESET);
            Board_DelayUs(5);
            SPI_SendData(BOARD_SPI_PERIPH, (uint8_t *)spiPreamble, sizeof(spiPreamble));
            while (SPI_GetFlagStatus(BOARD_SPI_PERIPH, SPI_BUSY_FLAG));
            Board_DelayUs(5);
            GPIO_WriteToOutputPin(BOARD_SPI_GPIO_PORT, BOARD_SPI_NSS_PIN, GPIO_PIN_SET);
            SPI_PeripheralControl(BOARD_SPI_PERIPH, DISABLE);

            snprintf(logBuf, sizeof(logBuf),
                     "[SYSTEM TICK #%lu] LED: %s | SPI: Frame Tx OK | I2C: Ready\r\n",
                     cycleCount / 100,
                     (GPIO_ReadFromInputPin(BOARD_LED_PORT, BOARD_LED_PIN) == BOARD_LED_ON) ? "ON" : "OFF");
            UART_Log(logBuf);
        }

        Board_DelayMs(10);
    }

    return 0;
}

