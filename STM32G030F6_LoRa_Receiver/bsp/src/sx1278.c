
#include "sx1278.h"

#define REG_FIFO                    0x00
#define REG_OP_MODE                 0x01
#define REG_FRF_MSB                 0x06
#define REG_FRF_MID                 0x07
#define REG_FRF_LSB                 0x08
#define REG_PA_CONFIG               0x09
#define REG_PA_RAMP                 0x0A
#define REG_OCP                     0x0B
#define REG_LNA                     0x0C
#define REG_FIFO_ADDR_PTR           0x0D
#define REG_FIFO_TX_BASE_ADDR       0x0E
#define REG_FIFO_RX_BASE_ADDR       0x0F
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_RX_NB_BYTES             0x13
#define REG_PKT_SNR_VALUE           0x19
#define REG_PKT_RSSI_VALUE          0x1A
#define REG_MODEM_CONFIG_1          0x1D
#define REG_MODEM_CONFIG_2          0x1E
#define REG_SYMB_TIMEOUT_LSB        0x1F
#define REG_PREAMBLE_MSB            0x20
#define REG_PREAMBLE_LSB            0x21
#define REG_PAYLOAD_LENGTH          0x22
#define REG_MODEM_CONFIG_3          0x26
#define REG_FREQ_ERROR_MSB          0x28
#define REG_RSSI_WIDEBAND           0x2C
#define REG_DETECTION_OPTIMIZE      0x31
#define REG_INVERTIQ                0x33
#define REG_DETECTION_THRESHOLD     0x37
#define REG_SYNC_WORD               0x39
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_VERSION                 0x42
#define REG_PA_DAC                  0x4D

#define MODE_LONG_RANGE_MODE        0x80
#define MODE_SLEEP                  0x00
#define MODE_STDBY                  0x01
#define MODE_TX                     0x03
#define MODE_RX_CONTINUOUS          0x05
#define MODE_RX_SINGLE              0x06

#define IRQ_RX_TIMEOUT_MASK         0x80
#define IRQ_RX_DONE_MASK            0x40
#define IRQ_PAYLOAD_CRC_ERROR_MASK  0x20
#define IRQ_VALID_HEADER_MASK       0x10
#define IRQ_TX_DONE_MASK            0x08

static inline void LoRa_NSS_Low(void)
{
    LORA_NSS_PORT->BSRR = (1U << (LORA_NSS_PIN + 16));
}

static inline void LoRa_NSS_High(void)
{
    LORA_NSS_PORT->BSRR = (1U << LORA_NSS_PIN);
}

static inline void LoRa_RST_Low(void)
{
    LORA_RST_PORT->BSRR = (1U << (LORA_RST_PIN + 16));
}

static inline void LoRa_RST_High(void)
{
    LORA_RST_PORT->BSRR = (1U << LORA_RST_PIN);
}

static uint8_t SX1278_TransferByte(uint8_t byte)
{
    while (!(SPI1->SR & SPI_SR_TXE));

    *(__IO uint8_t *)&SPI1->DR = byte;
    while (!(SPI1->SR & SPI_SR_RXNE));

    return *(__IO uint8_t *)&SPI1->DR;
}

static uint8_t SX1278_ReadReg(uint8_t reg)
{
    LoRa_NSS_Low();
    Board_DelayUs(2);

    SX1278_TransferByte(reg & 0x7F);
    uint8_t val = SX1278_TransferByte(0x00);

    while (SPI1->SR & SPI_SR_BSY);
    LoRa_NSS_High();
    Board_DelayUs(2);

    return val;
}

static void SX1278_WriteReg(uint8_t reg, uint8_t val)
{
    LoRa_NSS_Low();
    Board_DelayUs(2);

    SX1278_TransferByte(reg | 0x80);
    SX1278_TransferByte(val);

    while (SPI1->SR & SPI_SR_BSY);
    LoRa_NSS_High();
    Board_DelayUs(2);
}

static void SX1278_HardwareReset(void)
{
    LoRa_RST_Low();
    Board_DelayMs(10);
    LoRa_RST_High();
    Board_DelayMs(20);
}

uint8_t SX1278_ReadVersion(void)
{
    return SX1278_ReadReg(REG_VERSION);
}

void SX1278_SetSleep(void)
{
    SX1278_WriteReg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void SX1278_SetStandby(void)
{
    SX1278_WriteReg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

bool SX1278_Init(const SX1278_Config_t *pConfig)
{

    RCC->IOPENR  |= (RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN);
    RCC->APBENR2 |= RCC_APBENR2_SPI1EN;

    GPIOA->MODER   &= ~( (3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)) );
    GPIOA->MODER   |=  ( (2U << (5 * 2)) | (2U << (6 * 2)) | (2U << (7 * 2)) );
    GPIOA->OSPEEDR |=  ( (3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)) );
    GPIOA->PUPDR   &= ~( (3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)) );

    GPIOA->AFR[0]  &= ~( (0xFU << (5 * 4)) | (0xFU << (6 * 4)) | (0xFU << (7 * 4)) );

    GPIOB->MODER   &= ~(3U << (LORA_NSS_PIN * 2));
    GPIOB->MODER   |=  (1U << (LORA_NSS_PIN * 2));
    GPIOB->OTYPER  &= ~(1U << LORA_NSS_PIN);
    GPIOB->OSPEEDR |=  (3U << (LORA_NSS_PIN * 2));
    LoRa_NSS_High();

    GPIOA->MODER   &= ~(3U << (LORA_RST_PIN * 2));
    GPIOA->MODER   |=  (1U << (LORA_RST_PIN * 2));
    GPIOA->OTYPER  &= ~(1U << LORA_RST_PIN);
    GPIOA->OSPEEDR |=  (3U << (LORA_RST_PIN * 2));
    LoRa_RST_High();

    GPIOA->MODER   &= ~(3U << (LORA_DIO0_PIN * 2));
    GPIOA->PUPDR   &= ~(3U << (LORA_DIO0_PIN * 2));
    GPIOA->PUPDR   |=  (2U << (LORA_DIO0_PIN * 2));

    SPI1->CR1 = 0;

    SPI1->CR2 = (7U << SPI_CR2_DS_Pos) | SPI_CR2_FRXTH;

    SPI1->CR1 = SPI_CR1_MSTR | (2U << SPI_CR1_BR_Pos) | SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SPE;

    SX1278_HardwareReset();

    uint8_t ver = SX1278_ReadVersion();
    if (ver != 0x12 && ver != 0x22)
    {
        return false;
    }

    SX1278_SetSleep();

    uint64_t frf = ((uint64_t)pConfig->frequency_hz << 19) / 32000000UL;
    SX1278_WriteReg(REG_FRF_MSB, (uint8_t)(frf >> 16));
    SX1278_WriteReg(REG_FRF_MID, (uint8_t)(frf >> 8));
    SX1278_WriteReg(REG_FRF_LSB, (uint8_t)(frf));

    SX1278_WriteReg(REG_PA_CONFIG, 0xFF);
    SX1278_WriteReg(REG_OCP, 0x2B);

    SX1278_WriteReg(REG_LNA, 0x23);

    SX1278_WriteReg(REG_MODEM_CONFIG_1, 0x72);
    SX1278_WriteReg(REG_MODEM_CONFIG_2, 0x74);
    SX1278_WriteReg(REG_MODEM_CONFIG_3, 0x04);

    SX1278_WriteReg(REG_FIFO_TX_BASE_ADDR, 0x00);
    SX1278_WriteReg(REG_FIFO_RX_BASE_ADDR, 0x00);

    SX1278_WriteReg(REG_SYNC_WORD, LORA_SYNC_WORD);

    SX1278_WriteReg(REG_DIO_MAPPING_1, 0x00);

    SX1278_SetStandby();

    return true;
}

void SX1278_StartReceive(void)
{

    SX1278_WriteReg(REG_IRQ_FLAGS, 0xFF);

    SX1278_WriteReg(REG_FIFO_ADDR_PTR, SX1278_ReadReg(REG_FIFO_RX_BASE_ADDR));

    SX1278_WriteReg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

bool SX1278_IsPacketAvailable(void)
{

    if (GPIOA->IDR & (1U << LORA_DIO0_PIN))
    {
        return true;
    }

    uint8_t flags = SX1278_ReadReg(REG_IRQ_FLAGS);
    return (flags & IRQ_RX_DONE_MASK) != 0;
}

uint8_t SX1278_ReceivePacket(uint8_t *rxBuf, uint8_t maxLen, int16_t *pRSSI, int8_t *pSNR)
{
    uint8_t irqFlags = SX1278_ReadReg(REG_IRQ_FLAGS);

    if (irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK)
    {
        SX1278_WriteReg(REG_IRQ_FLAGS, 0xFF);
        return 0;
    }

    uint8_t len = SX1278_ReadReg(REG_RX_NB_BYTES);
    if (len > maxLen)
    {
        len = maxLen;
    }

    uint8_t currentAddr = SX1278_ReadReg(REG_FIFO_RX_CURRENT_ADDR);
    SX1278_WriteReg(REG_FIFO_ADDR_PTR, currentAddr);

    for (uint8_t i = 0; i < len; i++)
    {
        rxBuf[i] = SX1278_ReadReg(REG_FIFO);
    }
    rxBuf[len] = '\0';

    int8_t snrVal = (int8_t)SX1278_ReadReg(REG_PKT_SNR_VALUE);
    if (pSNR)
    {
        *pSNR = snrVal / 4;
    }

    uint8_t rssiReg = SX1278_ReadReg(REG_PKT_RSSI_VALUE);
    if (pRSSI)
    {
        if (snrVal < 0)
        {
            *pRSSI = -164 + rssiReg + (snrVal / 4);
        }
        else
        {
            *pRSSI = -164 + (int16_t)((uint16_t)rssiReg * 16 / 15);
        }
    }

    SX1278_WriteReg(REG_IRQ_FLAGS, 0xFF);

    SX1278_StartReceive();

    return len;
}

