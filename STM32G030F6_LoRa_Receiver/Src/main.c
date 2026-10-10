
#include "BoardConfig.h"
#include "sx1278.h"

static void System_GPIO_Init(void)
{

    RCC->IOPENR |= (RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN);

    RX_LED_PORT->MODER   &= ~(3U << (RX_LED_PIN * 2));
    RX_LED_PORT->MODER   |=  (1U << (RX_LED_PIN * 2));
    RX_LED_PORT->OTYPER  &= ~(1U << RX_LED_PIN);
    RX_LED_PORT->OSPEEDR |=  (3U << (RX_LED_PIN * 2));

    RX_LED_PORT->BSRR = (1U << (RX_LED_PIN + 16));
}

static void System_USART2_Init(void)
{

    RCC->APBENR1 |= RCC_APBENR1_USART2EN;

    GPIOA->MODER   &= ~( (3U << (USART_TX_PIN * 2)) | (3U << (USART_RX_PIN * 2)) );
    GPIOA->MODER   |=  ( (2U << (USART_TX_PIN * 2)) | (2U << (USART_RX_PIN * 2)) );
    GPIOA->OSPEEDR |=  ( (3U << (USART_TX_PIN * 2)) | (3U << (USART_RX_PIN * 2)) );

    GPIOA->AFR[0]  &= ~( (0xFU << (USART_TX_PIN * 4)) | (0xFU << (USART_RX_PIN * 4)) );
    GPIOA->AFR[0]  |=  ( (1U   << (USART_TX_PIN * 4)) | (1U   << (USART_RX_PIN * 4)) );

    USART_PERIPH->BRR = (F_CPU + (USART_BAUDRATE / 2)) / USART_BAUDRATE;

    USART_PERIPH->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

static void UART_SendChar(char c)
{
    while (!(USART_PERIPH->ISR & USART_ISR_TXE_TXFNF));
    USART_PERIPH->TDR = (uint8_t)c;
}

static void UART_Log(const char *str)
{
    while (*str)
    {
        UART_SendChar(*str++);
    }
}

static void RX_LED_On(void)
{
    RX_LED_PORT->BSRR = (1U << RX_LED_PIN);
}

static void RX_LED_Off(void)
{
    RX_LED_PORT->BSRR = (1U << (RX_LED_PIN + 16));
}

static void RX_LED_Pulse(uint32_t ms)
{
    RX_LED_On();
    Board_DelayMs(ms);
    RX_LED_Off();
}

int main(void)
{
    char textBuf[256];
    uint8_t rxBuffer[128];
    uint32_t rxPacketCount = 0;

    System_GPIO_Init();
    System_USART2_Init();

    RX_LED_Pulse(100);
    Board_DelayMs(100);
    RX_LED_Pulse(100);

    UART_Log("\r\n=================================================================\r\n");
    UART_Log("   STM32G030F6P6 BARE-METAL LORA WEATHER STATION RECEIVER NODE   \r\n");
    UART_Log("   Board: Seven Semi STM32G030F6 Breakout | ARM Cortex-M0+ @ 16M \r\n");
    UART_Log("   Radio: Semtech SX1278 (Ra-02 @ 433 MHz via SPI1)              \r\n");
    UART_Log("   Reception Indicator: LED on PB7                               \r\n");
    UART_Log("   Serial Output: PA2 (TX) / PA3 (RX) @ 115200 Baud              \r\n");
    UART_Log("=================================================================\r\n\r\n");

    UART_Log("[INIT] Initializing Semtech SX1278 LoRa Receiver...\r\n");
    SX1278_Config_t loraCfg = {
        .frequency_hz     = LORA_FREQ_HZ,
        .power_dbm        = 17,
        .spreading_factor = LORA_SF,
        .bandwidth_hz     = LORA_BW_HZ,
        .coding_rate      = LORA_CR
    };

    bool loraOk = SX1278_Init(&loraCfg);
    uint8_t ver = SX1278_ReadVersion();

    if (loraOk)
    {
        snprintf(textBuf, sizeof(textBuf),
                 "[INIT] LoRa SX1278 Online! Silicon ID: 0x%02X | Freq: 433 MHz | SF7 | BW125k\r\n", ver);
        UART_Log(textBuf);
        UART_Log("[READY] Listening for incoming weather station telemetry packets...\r\n\r\n");
        SX1278_StartReceive();
    }
    else
    {
        snprintf(textBuf, sizeof(textBuf),
                 "[ERROR] SX1278 not detected! Silicon ID read: 0x%02X (Expected 0x12)\r\n", ver);
        UART_Log(textBuf);
        UART_Log("[CHECK] Check wiring: SCK=PA5, MISO=PA6, MOSI=PA7, NSS=PB0, RST=PA1, DIO0=PA0\r\n\r\n");
    }

    uint32_t idleCounter = 0;

    while (1)
    {
        if (loraOk && SX1278_IsPacketAvailable())
        {

            RX_LED_On();

            int16_t rssi = 0;
            int8_t  snr  = 0;
            uint8_t len = SX1278_ReceivePacket(rxBuffer, sizeof(rxBuffer) - 1, &rssi, &snr);

            if (len > 0)
            {
                rxPacketCount++;
                rxBuffer[len] = '\0';

                snprintf(textBuf, sizeof(textBuf),
                         "\r\n-----------------------------------------------------------------\r\n"
                         "[RX #%lu] %s\r\n",
                         rxPacketCount, (char *)rxBuffer);
                UART_Log(textBuf);

                snprintf(textBuf, sizeof(textBuf),
                         "Signal: RSSI = %d dBm | SNR = %d dB | Payload: %d bytes\r\n"
                         "-----------------------------------------------------------------\r\n",
                         rssi, snr, len);
                UART_Log(textBuf);

                char *tPtr   = strstr((char *)rxBuffer, "|T:");
                char *hPtr   = strstr((char *)rxBuffer, "|H:");
                char *pPtr   = strstr((char *)rxBuffer, "|P:");
                char *altPtr = strstr((char *)rxBuffer, "|ALT:");
                char *seqPtr = strstr((char *)rxBuffer, "|SEQ:");

                (void)seqPtr;

                if (tPtr && hPtr && pPtr)
                {
                    char tempStr[16]  = "N/A";
                    char humStr[16]   = "N/A";
                    char pressStr[16] = "N/A";
                    char altStr[16]   = "N/A";

                    sscanf(tPtr, "|T:%[^|]", tempStr);
                    sscanf(hPtr, "|H:%[^|]", humStr);
                    sscanf(pPtr, "|P:%[^|]", pressStr);
                    if (altPtr) sscanf(altPtr, "|ALT:%[^|]", altStr);

                    snprintf(textBuf, sizeof(textBuf),
                             " >>> Weather Report: Temp = %s | Humidity = %s | Pressure = %s | Alt = %s\r\n\r\n",
                             tempStr, humStr, pressStr, altStr);
                    UART_Log(textBuf);
                }

                Board_DelayMs(100);
                RX_LED_Off();
                idleCounter = 0;
            }
            else
            {
                RX_LED_Off();
            }
        }
        else if (!loraOk)
        {

            Board_DelayMs(1000);
            ver = SX1278_ReadVersion();
            if (ver == 0x12 || ver == 0x22)
            {
                loraOk = SX1278_Init(&loraCfg);
                if (loraOk)
                {
                    snprintf(textBuf, sizeof(textBuf),
                             "\r\n>>> [HOTPLUG] LoRa SX1278 Module Detected! (Silicon ID: 0x%02X)\r\n"
                             "[READY] Listening for weather station packets...\r\n\r\n", ver);
                    UART_Log(textBuf);
                    SX1278_StartReceive();
                }
            }
        }
        else
        {

            Board_DelayMs(10);
            idleCounter++;
            if (idleCounter % 500 == 0)
            {
                UART_Log("[RX STANDBY] Listening on 433.0 MHz (SF7/125kHz)...\r\n");
            }
        }
    }

    return 0;
}

