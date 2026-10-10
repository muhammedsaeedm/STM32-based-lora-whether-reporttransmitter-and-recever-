
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "BoardConfig.h"
#include "sht3x.h"
#include "bmp280.h"
#include "oled_sh1106.h"
#include "sx1278.h"

static USART_Handle_t g_usart2;
static I2C_Handle_t   g_i2c1;
static SPI_Handle_t   g_spi2;
static GPIO_Handle_t  g_led;
static GPIO_Handle_t  g_btn;

static void System_GPIO_Init(void)
{

    g_led.pGPIOx = BOARD_LED_PORT;
    g_led.GPIO_PinConfig.GPIO_PinNumber = BOARD_LED_PIN;
    g_led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    g_led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    g_led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    g_led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(BOARD_LED_PORT, ENABLE);
    GPIO_Init(&g_led);
    GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_OFF);

    g_btn.pGPIOx = BOARD_BUTTON_PORT;
    g_btn.GPIO_PinConfig.GPIO_PinNumber = BOARD_BUTTON_PIN;
    g_btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    g_btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    g_btn.GPIO_PinConfig.GPIO_PinPuPdControl = BOARD_BUTTON_PUPD;

    GPIO_PeriClockControl(BOARD_BUTTON_PORT, ENABLE);
    GPIO_Init(&g_btn);
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

    g_usart2.pUSARTx = BOARD_VCP_USART;
    g_usart2.USART_Config.USART_Baud = BOARD_VCP_BAUDRATE;
    g_usart2.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    g_usart2.USART_Config.USART_Mode = USART_MODE_TXRX;
    g_usart2.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    g_usart2.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    g_usart2.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&g_usart2);
    USART_PeripheralControl(BOARD_VCP_USART, ENABLE);
}

static void System_I2C1_Init(uint8_t use_pb6_pb7)
{
    GPIO_Handle_t i2c_pins;

    i2c_pins.pGPIOx = GPIOB;
    i2c_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    i2c_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    i2c_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    i2c_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    i2c_pins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

    GPIO_PeriClockControl(GPIOB, ENABLE);

    if (use_pb6_pb7)
    {

        i2c_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
        GPIO_Init(&i2c_pins);
        i2c_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
        GPIO_Init(&i2c_pins);
    }
    else
    {

        i2c_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
        GPIO_Init(&i2c_pins);
        i2c_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
        GPIO_Init(&i2c_pins);
    }

    g_i2c1.pI2Cx = I2C1;
    g_i2c1.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
    g_i2c1.I2C_Config.I2C_DeviceAddress = 0x61;
    g_i2c1.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    g_i2c1.I2C_Config.I2C_SCLSpeed = BOARD_I2C_SPEED;

    I2C_Init(&g_i2c1);
    I2C_PeripheralControl(I2C1, ENABLE);
    I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);
}

static void System_SPI2_Init(void)
{
    GPIO_Handle_t spi_pins;

    spi_pins.pGPIOx = LORA_SPI_GPIO_PORT;
    spi_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    spi_pins.GPIO_PinConfig.GPIO_PinAltFunMode = LORA_SPI_AF_MODE;
    spi_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    spi_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    spi_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

    GPIO_PeriClockControl(LORA_SPI_GPIO_PORT, ENABLE);

    spi_pins.GPIO_PinConfig.GPIO_PinNumber = LORA_SPI_SCLK_PIN;
    GPIO_Init(&spi_pins);
    spi_pins.GPIO_PinConfig.GPIO_PinNumber = LORA_SPI_MISO_PIN;
    GPIO_Init(&spi_pins);
    spi_pins.GPIO_PinConfig.GPIO_PinNumber = LORA_SPI_MOSI_PIN;
    GPIO_Init(&spi_pins);

    spi_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    spi_pins.GPIO_PinConfig.GPIO_PinNumber = LORA_NSS_PIN;
    GPIO_Init(&spi_pins);
    GPIO_WriteToOutputPin(LORA_NSS_PORT, LORA_NSS_PIN, GPIO_PIN_SET);

    g_spi2.pSPIx = LORA_SPI_PERIPH;
    g_spi2.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
    g_spi2.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
    g_spi2.SPIConfig.SPI_SclkSpeed = LORA_SPI_SPEED;
    g_spi2.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
    g_spi2.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
    g_spi2.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
    g_spi2.SPIConfig.SPI_SSM = SPI_SSM_EN;

    SPI_Init(&g_spi2);
    SPI_SSIConfig(LORA_SPI_PERIPH, ENABLE);
    SPI_PeripheralControl(LORA_SPI_PERIPH, ENABLE);
}

static SPI_Handle_t g_spi1;

static void System_SPI1_Init(void)
{
    GPIO_Handle_t spi_pins;
    memset(&spi_pins, 0, sizeof(spi_pins));

    spi_pins.pGPIOx = GPIOA;
    spi_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    spi_pins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
    spi_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    spi_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    spi_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

    GPIO_PeriClockControl(GPIOA, ENABLE);

    spi_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GPIO_Init(&spi_pins);
    spi_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&spi_pins);
    spi_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&spi_pins);

    GPIO_Handle_t cs_pin;
    memset(&cs_pin, 0, sizeof(cs_pin));
    cs_pin.pGPIOx = GPIOB;
    cs_pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    cs_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    cs_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    cs_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    cs_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_PeriClockControl(GPIOB, ENABLE);
    GPIO_Init(&cs_pin);
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_6, GPIO_PIN_SET);

    g_spi1.pSPIx = SPI1;
    g_spi1.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
    g_spi1.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
    g_spi1.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;
    g_spi1.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
    g_spi1.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
    g_spi1.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
    g_spi1.SPIConfig.SPI_SSM = SPI_SSM_EN;

    SPI_Init(&g_spi1);
    SPI_SSIConfig(SPI1, ENABLE);
    SPI_PeripheralControl(SPI1, ENABLE);
}

static uint8_t SPI1_ReadReg(uint8_t reg)
{
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_6, GPIO_PIN_RESET);
    Board_DelayUs(5);

    while (!SPI_GetFlagStatus(SPI1, SPI_TXE_FLAG));
    SPI1->DR = (reg & 0x7F);
    while (!SPI_GetFlagStatus(SPI1, SPI_RXNE_FLAG));
    (void)SPI1->DR;

    while (!SPI_GetFlagStatus(SPI1, SPI_TXE_FLAG));
    SPI1->DR = 0x00;
    while (!SPI_GetFlagStatus(SPI1, SPI_RXNE_FLAG));
    uint8_t val = (uint8_t)SPI1->DR;

    while (SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG));
    GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_6, GPIO_PIN_SET);
    Board_DelayUs(5);

    return val;
}

static void UART_Log(const char *msg)
{
    USART_SendData(&g_usart2, (uint8_t *)msg, strlen(msg));
}

static void System_I2C1_Scan(void)
{
    char buf[128];
    uint8_t found = 0;
    uint8_t start_err = 0;

    System_I2C1_Init(0);
    uint8_t pb8_val = (GPIOB->IDR >> 8) & 0x01;
    uint8_t pb9_val = (GPIOB->IDR >> 9) & 0x01;
    snprintf(buf, sizeof(buf), "[I2C SCAN] Probing Bus A (PB8=SCL [%d], PB9=SDA [%d])...\r\n", pb8_val, pb9_val);
    UART_Log(buf);

    for (uint8_t addr = 1; addr < 128; addr++)
    {
        uint8_t res = I2C_DeviceReady(&g_i2c1, addr);
        if (res == 1)
        {
            found++;
            snprintf(buf, sizeof(buf), "[I2C SCAN] >>> Found device at 7-bit addr: 0x%02X (%d)\r\n", addr, addr);
            UART_Log(buf);
        }
        else if (res == 2)
        {
            start_err = 1;
            break;
        }
    }

    if (start_err)
    {
        UART_Log("[I2C SCAN] Bus A START timed out: SCL/SDA lines held LOW or missing pull-ups.\r\n");
    }
    else if (found > 0)
    {
        snprintf(buf, sizeof(buf), "[I2C SCAN] Selected PB8/PB9. Total %d devices active.\r\n\r\n", found);
        UART_Log(buf);
        return;
    }

    System_I2C1_Init(1);
    uint8_t pb6_val = (GPIOB->IDR >> 6) & 0x01;
    uint8_t pb7_val = (GPIOB->IDR >> 7) & 0x01;
    snprintf(buf, sizeof(buf), "[I2C SCAN] Probing Bus B (PB6=SCL [%d], PB7=SDA [%d])...\r\n", pb6_val, pb7_val);
    UART_Log(buf);

    found = 0;
    start_err = 0;
    for (uint8_t addr = 1; addr < 128; addr++)
    {
        uint8_t res = I2C_DeviceReady(&g_i2c1, addr);
        if (res == 1)
        {
            found++;
            snprintf(buf, sizeof(buf), "[I2C SCAN] >>> Found device at 7-bit addr: 0x%02X (%d)\r\n", addr, addr);
            UART_Log(buf);
        }
        else if (res == 2)
        {
            start_err = 1;
            break;
        }
    }

    if (start_err)
    {
        UART_Log("[I2C SCAN] Bus B START timed out: SCL/SDA lines held LOW or missing pull-ups.\r\n");
    }
    else if (found > 0)
    {
        snprintf(buf, sizeof(buf), "[I2C SCAN] Selected PB6/PB7. Total %d devices active.\r\n\r\n", found);
        UART_Log(buf);
        return;
    }

    UART_Log("[I2C SCAN] Check wiring: VCC (3.3V), GND, SCL (PB8 or PB6), SDA (PB9 or PB7).\r\n\r\n");

    System_I2C1_Init(0);
}

int main(void)
{
    char textBuf[160];
    char loraPkt[96];
    uint32_t packetSeq = 0;
    uint8_t prevBtnState = BOARD_BUTTON_RELEASED;

    System_GPIO_Init();
    System_USART2_Init();
    System_I2C1_Init(0);
    System_SPI2_Init();
    System_SPI1_Init();

    UART_Log("\r\n=================================================================\r\n");
    UART_Log("   STM32F401RE BARE-METAL LORA WEATHER STATION & TELEMETRY NODE  \r\n");
    UART_Log("   Board: NUCLEO-F401RE | ARM Cortex-M4 @ 16 MHz HSI             \r\n");
    UART_Log("   Sensors: SHT3x (I2C) + BMP280 (I2C) + 1.3\" SH1106 OLED (I2C)  \r\n");
    UART_Log("   Radio: Semtech SX1278 (Ra-02 @ 433 MHz via SPI2)              \r\n");
    UART_Log("=================================================================\r\n\r\n");

    System_I2C1_Scan();

    UART_Log("[INIT] Initializing 1.3\" SH1106 OLED Display...\r\n");
    bool oledOk = OLED_Init(&g_i2c1, OLED_I2C_ADDR_A, true);
    OLED_SetColumnOffset(4);

    if (oledOk)
    {
        OLED_Clear();
        OLED_FillRect(0, 0, 128, 12, OLED_COLOR_WHITE);
        OLED_DrawString(8, 2, "STM32F401 LORA NODE", OLED_COLOR_BLACK);
        OLED_DrawString(4, 20, "Sensors Initializing:", OLED_COLOR_WHITE);
        OLED_DrawString(8, 34, "* SHT3x Temp/Hum", OLED_COLOR_WHITE);
        OLED_DrawString(8, 44, "* BMP280 Barometer", OLED_COLOR_WHITE);
        OLED_DrawProgressBar(10, 54, 108, 7, 30);
        OLED_Update();
        Board_DelayMs(600);
    }

    UART_Log("[INIT] Initializing Sensirion SHT3x Sensor...\r\n");
    bool sht3xOk = SHT3x_Init(&g_i2c1, SHT3X_I2C_ADDR_A);
    UART_Log("[INIT] Initializing Bosch BMP280 Sensor...\r\n");
    bool bmp280Ok = BMP280_Init(&g_i2c1, BMP280_I2C_ADDR_A);

    UART_Log("[INIT] Checking Semtech SX1278 LoRa Modem...\r\n");
    SX1278_Config_t loraCfg = {
        .frequency_hz     = LORA_DEFAULT_FREQ_HZ,
        .power_dbm        = LORA_DEFAULT_TX_POWER,
        .spreading_factor = LORA_DEFAULT_SF,
        .bandwidth_hz     = LORA_DEFAULT_BW,
        .coding_rate      = LORA_DEFAULT_CR
    };
    bool loraOk = false;
    SPI_Handle_t *pActiveSpi = NULL;

    SX1278_SetNssPin(GPIOB, GPIO_PIN_NO_6);
    uint8_t loraVer = SX1278_ReadVersion(&g_spi1);
    if (loraVer == 0x12 || loraVer == 0x22)
    {
        pActiveSpi = &g_spi1;
        loraOk = SX1278_Init(&g_spi1, &loraCfg);
        snprintf(textBuf, sizeof(textBuf), "[INIT] LoRa Found on SPI1 (Arduino D10-D13)! Silicon ID: 0x%02X (%s)\r\n",
                 loraVer, (loraVer == 0x12) ? "SX1276/78" : "SX1272");
        UART_Log(textBuf);
    }
    else
    {

        SX1278_SetNssPin(GPIOB, GPIO_PIN_NO_12);
        loraVer = SX1278_ReadVersion(&g_spi2);
        if (loraVer == 0x12 || loraVer == 0x22)
        {
            pActiveSpi = &g_spi2;
            loraOk = SX1278_Init(&g_spi2, &loraCfg);
            snprintf(textBuf, sizeof(textBuf), "[INIT] LoRa Found on SPI2 (Morpho CN10)! Silicon ID: 0x%02X (%s)\r\n",
                     loraVer, (loraVer == 0x12) ? "SX1276/78" : "SX1272");
            UART_Log(textBuf);
        }
        else
        {
            snprintf(textBuf, sizeof(textBuf), "[INIT] LoRa not found (SPI1=0x%02X, SPI2=0x%02X)\r\n", SPI1_ReadReg(0x42), loraVer);
            UART_Log(textBuf);
        }
    }

    snprintf(textBuf, sizeof(textBuf), "[HW DIAG] OLED: %s | SHT3x: %s | BMP280: %s | LoRa: %s\r\n\r\n",
             oledOk ? "READY" : "ERR",
             sht3xOk ? "ONLINE" : "BENCH_SIM",
             bmp280Ok ? "ONLINE" : "BENCH_SIM",
             loraOk ? "ONLINE" : "BENCH_SIM");
    UART_Log(textBuf);

    SHT3x_Data_t  shtData;
    BMP280_Data_t bmpData;

    while (1)
    {
        packetSeq++;

        SHT3x_ReadMeasurement(&g_i2c1, SHT3X_I2C_ADDR_A, &shtData);

        BMP280_ReadData(&g_i2c1, BMP280_I2C_ADDR_A, &bmpData);

        snprintf(loraPkt, sizeof(loraPkt),
                 "NODE:01|T:%d.%01dC|H:%d.%01d%%|P:%d.%01dhPa|ALT:%dm|SEQ:%lu",
                 (int)shtData.temperature_c, (int)(shtData.temperature_c * 10) % 10,
                 (int)shtData.humidity_rh,   (int)(shtData.humidity_rh * 10) % 10,
                 (int)bmpData.pressure_hpa,  (int)(bmpData.pressure_hpa * 10) % 10,
                 (int)bmpData.altitude_m,
                 packetSeq);

        bool txStatus = false;
        if (loraOk && pActiveSpi != NULL)
        {
            GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_ON);
            txStatus = SX1278_SendPacket(pActiveSpi, (uint8_t *)loraPkt, strlen(loraPkt));
            GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_OFF);
        }
        else
        {

            GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_ON);
            Board_DelayMs(40);
            GPIO_WriteToOutputPin(BOARD_LED_PORT, BOARD_LED_PIN, BOARD_LED_OFF);
        }

        OLED_Clear();

        OLED_FillRect(0, 0, 128, 11, OLED_COLOR_WHITE);
        snprintf(textBuf, sizeof(textBuf), "WEATHER NODE #%03lu", packetSeq % 1000);
        OLED_DrawString(4, 2, textBuf, OLED_COLOR_BLACK);

        snprintf(textBuf, sizeof(textBuf), "Temp: %d.%01d C", (int)shtData.temperature_c, (int)(shtData.temperature_c * 10) % 10);
        OLED_DrawString(4, 13, textBuf, OLED_COLOR_WHITE);
        snprintf(textBuf, sizeof(textBuf), "Hum : %d.%01d %%", (int)shtData.humidity_rh, (int)(shtData.humidity_rh * 10) % 10);
        OLED_DrawString(4, 23, textBuf, OLED_COLOR_WHITE);

        snprintf(textBuf, sizeof(textBuf), "Pres: %d.%01d hPa", (int)bmpData.pressure_hpa, (int)(bmpData.pressure_hpa * 10) % 10);
        OLED_DrawString(4, 33, textBuf, OLED_COLOR_WHITE);
        snprintf(textBuf, sizeof(textBuf), "Alt : %d m", (int)bmpData.altitude_m);
        OLED_DrawString(4, 43, textBuf, OLED_COLOR_WHITE);

        OLED_DrawHLine(0, 52, 128, OLED_COLOR_WHITE);
        if (loraOk && txStatus)
        {
            OLED_DrawString(4, 55, "LoRa TX OK (433MHz)", OLED_COLOR_WHITE);
        }
        else if (loraOk)
        {
            OLED_DrawString(4, 55, "LoRa Beacon Active", OLED_COLOR_WHITE);
        }
        else
        {
            OLED_DrawString(4, 55, "LoRa Offline (NC)", OLED_COLOR_WHITE);
        }

        if (!oledOk && (packetSeq % 3 == 0))
        {
            if (I2C_DeviceReady(&g_i2c1, 0x3C) || I2C_DeviceReady(&g_i2c1, 0x3D))
            {
                UART_Log("\r\n>>> [HOTPLUG] OLED Display detected on I2C bus! Initializing...\r\n");
                oledOk = OLED_Init(&g_i2c1, OLED_I2C_ADDR_A, true);
                OLED_SetColumnOffset(4);
            }
        }

        if (!loraOk && (packetSeq % 2 == 0))
        {
            uint8_t v2 = SX1278_ReadVersion(&g_spi2);
            uint8_t v1 = SPI1_ReadReg(0x42);
            if (v2 == 0x12 || v2 == 0x22)
            {
                pActiveSpi = &g_spi2;
                loraOk = SX1278_Init(&g_spi2, &loraCfg);
                snprintf(textBuf, sizeof(textBuf), "\r\n>>> [HOTPLUG] LoRa SX1278 Detected on SPI2 (Morpho)! Silicon ID: 0x%02X\r\n", v2);
                UART_Log(textBuf);
            }
            else if (v1 == 0x12 || v1 == 0x22)
            {
                pActiveSpi = &g_spi1;
                loraOk = SX1278_Init(&g_spi1, &loraCfg);
                snprintf(textBuf, sizeof(textBuf), "\r\n>>> [HOTPLUG] LoRa SX1278 Detected on SPI1 (Arduino Headers)! Silicon ID: 0x%02X\r\n", v1);
                UART_Log(textBuf);
            }
            else
            {
                snprintf(textBuf, sizeof(textBuf), "[HW PROBE] LoRa Reg 0x42: SPI2(Morpho)=0x%02X | SPI1(Arduino)=0x%02X\r\n", v2, v1);
                UART_Log(textBuf);
            }
        }

        OLED_Update();

        snprintf(textBuf, sizeof(textBuf),
                 "[TX #%lu] %s | RF: %s\r\n",
                 packetSeq, loraPkt,
                 loraOk ? (txStatus ? "SUCCESS" : "TIMEOUT") : "STANDBY (NC)");
        UART_Log(textBuf);

        for (uint32_t ms = 0; ms < 2000; ms += 20)
        {
            uint8_t currBtnState = GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN);
            if (prevBtnState == BOARD_BUTTON_RELEASED && currBtnState == BOARD_BUTTON_PRESSED)
            {
                Board_DelayMs(20);
                if (GPIO_ReadFromInputPin(BOARD_BUTTON_PORT, BOARD_BUTTON_PIN) == BOARD_BUTTON_PRESSED)
                {
                    prevBtnState = BOARD_BUTTON_PRESSED;

                    uint8_t nextOffset = (OLED_GetColumnOffset() == 4) ? 2 : ((OLED_GetColumnOffset() == 2) ? 0 : 4);
                    OLED_SetColumnOffset(nextOffset);
                    snprintf(textBuf, sizeof(textBuf), ">>> [BUTTON] Column Offset cycled to %d px -> Transmitting...\r\n", nextOffset);
                    UART_Log(textBuf);
                    break;
                }
            }
            prevBtnState = currBtnState;
            Board_DelayMs(20);
        }
    }

    return 0;
}

