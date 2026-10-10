
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

#define IRQ_TX_DONE_MASK            0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK  0x20
#define IRQ_RX_DONE_MASK            0x40

static GPIO_RegDef_t *g_nss_port = GPIOB;
static uint8_t        g_nss_pin  = GPIO_PIN_NO_6;

void SX1278_SetNssPin(GPIO_RegDef_t *pPort, uint8_t pin)
{
    g_nss_port = pPort;
    g_nss_pin  = pin;
}

static uint8_t SX1278_TransferByte(SPI_RegDef_t *pSPIx, uint8_t byte)
{
    while (!SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG));
    pSPIx->DR = byte;
    while (!SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG));
    return (uint8_t)pSPIx->DR;
}

static uint8_t SX1278_ReadReg(SPI_Handle_t *pSpiHandle, uint8_t reg)
{
    GPIO_WriteToOutputPin(g_nss_port, g_nss_pin, GPIO_PIN_RESET);
    Board_DelayUs(2);

    SX1278_TransferByte(pSpiHandle->pSPIx, reg & 0x7F);
    uint8_t val = SX1278_TransferByte(pSpiHandle->pSPIx, 0x00);

    while (SPI_GetFlagStatus(pSpiHandle->pSPIx, SPI_BUSY_FLAG));
    GPIO_WriteToOutputPin(g_nss_port, g_nss_pin, GPIO_PIN_SET);
    Board_DelayUs(2);

    return val;
}

static void SX1278_WriteReg(SPI_Handle_t *pSpiHandle, uint8_t reg, uint8_t val)
{
    GPIO_WriteToOutputPin(g_nss_port, g_nss_pin, GPIO_PIN_RESET);
    Board_DelayUs(2);

    SX1278_TransferByte(pSpiHandle->pSPIx, reg | 0x80);
    SX1278_TransferByte(pSpiHandle->pSPIx, val);

    while (SPI_GetFlagStatus(pSpiHandle->pSPIx, SPI_BUSY_FLAG));
    GPIO_WriteToOutputPin(g_nss_port, g_nss_pin, GPIO_PIN_SET);
    Board_DelayUs(2);
}

static void SX1278_HardwareReset(void)
{
    GPIO_WriteToOutputPin(LORA_RST_PORT, LORA_RST_PIN, GPIO_PIN_RESET);
    Board_DelayMs(10);
    GPIO_WriteToOutputPin(LORA_RST_PORT, LORA_RST_PIN, GPIO_PIN_SET);
    Board_DelayMs(15);
}

uint8_t SX1278_ReadVersion(SPI_Handle_t *pSpiHandle)
{
    return SX1278_ReadReg(pSpiHandle, REG_VERSION);
}

void SX1278_SetSleep(SPI_Handle_t *pSpiHandle)
{
    SX1278_WriteReg(pSpiHandle, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void SX1278_SetStandby(SPI_Handle_t *pSpiHandle)
{
    SX1278_WriteReg(pSpiHandle, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

bool SX1278_Init(SPI_Handle_t *pSpiHandle, const SX1278_Config_t *pConfig)
{

    GPIO_Handle_t rst_pin, dio0_pin;

    rst_pin.pGPIOx = LORA_RST_PORT;
    rst_pin.GPIO_PinConfig.GPIO_PinNumber = LORA_RST_PIN;
    rst_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    rst_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    rst_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    rst_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(LORA_RST_PORT, ENABLE);
    GPIO_Init(&rst_pin);

    dio0_pin.pGPIOx = LORA_DIO0_PORT;
    dio0_pin.GPIO_PinConfig.GPIO_PinNumber = LORA_DIO0_PIN;
    dio0_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    dio0_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    dio0_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

    GPIO_PeriClockControl(LORA_DIO0_PORT, ENABLE);
    GPIO_Init(&dio0_pin);

    if (pSpiHandle->pSPIx == SPI1)
    {
        g_nss_port = GPIOB;
        g_nss_pin  = GPIO_PIN_NO_6;
    }
    else
    {
        g_nss_port = GPIOB;
        g_nss_pin  = GPIO_PIN_NO_12;
    }

    SX1278_HardwareReset();

    SPI_PeripheralControl(pSpiHandle->pSPIx, ENABLE);

    uint8_t version = SX1278_ReadVersion(pSpiHandle);
    if (version != 0x12 && version != 0x22)
    {

        return false;
    }

    SX1278_SetSleep(pSpiHandle);

    uint64_t frf = ((uint64_t)pConfig->frequency_hz << 19) / 32000000UL;
    SX1278_WriteReg(pSpiHandle, REG_FRF_MSB, (uint8_t)(frf >> 16));
    SX1278_WriteReg(pSpiHandle, REG_FRF_MID, (uint8_t)(frf >> 8));
    SX1278_WriteReg(pSpiHandle, REG_FRF_LSB, (uint8_t)(frf));

    SX1278_WriteReg(pSpiHandle, REG_PA_CONFIG, 0xFF);
    SX1278_WriteReg(pSpiHandle, REG_OCP, 0x2B);

    SX1278_WriteReg(pSpiHandle, REG_LNA, 0x23);

    SX1278_WriteReg(pSpiHandle, REG_MODEM_CONFIG_1, 0x72);

    SX1278_WriteReg(pSpiHandle, REG_MODEM_CONFIG_2, 0x74);

    SX1278_WriteReg(pSpiHandle, REG_MODEM_CONFIG_3, 0x04);

    SX1278_WriteReg(pSpiHandle, REG_FIFO_TX_BASE_ADDR, 0x00);
    SX1278_WriteReg(pSpiHandle, REG_FIFO_RX_BASE_ADDR, 0x00);

    SX1278_WriteReg(pSpiHandle, REG_SYNC_WORD, 0x12);

    SX1278_SetStandby(pSpiHandle);

    return true;
}

bool SX1278_SendPacket(SPI_Handle_t *pSpiHandle, const uint8_t *payload, uint8_t len)
{

    SX1278_SetStandby(pSpiHandle);

    SX1278_WriteReg(pSpiHandle, REG_FIFO_ADDR_PTR, 0x00);

    for (uint8_t i = 0; i < len; i++)
    {
        SX1278_WriteReg(pSpiHandle, REG_FIFO, payload[i]);
    }

    SX1278_WriteReg(pSpiHandle, REG_PAYLOAD_LENGTH, len);

    SX1278_WriteReg(pSpiHandle, REG_IRQ_FLAGS, 0xFF);

    SX1278_WriteReg(pSpiHandle, REG_DIO_MAPPING_1, 0x40);

    SX1278_WriteReg(pSpiHandle, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

    uint32_t timeout = 2000;
    while ((SX1278_ReadReg(pSpiHandle, REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0)
    {
        Board_DelayMs(1);
        if (--timeout == 0)
        {
            SX1278_SetStandby(pSpiHandle);
            return false;
        }
    }

    SX1278_WriteReg(pSpiHandle, REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);

    SX1278_SetStandby(pSpiHandle);

    return true;
}

bool SX1278_IsPacketAvailable(SPI_Handle_t *pSpiHandle)
{
    uint8_t flags = SX1278_ReadReg(pSpiHandle, REG_IRQ_FLAGS);
    if (flags & IRQ_RX_DONE_MASK)
    {
        return !(flags & IRQ_PAYLOAD_CRC_ERROR_MASK);
    }
    return false;
}

uint8_t SX1278_ReceivePacket(SPI_Handle_t *pSpiHandle, uint8_t *rxBuf, uint8_t maxLen, int16_t *pRSSI, int8_t *pSNR)
{
    uint8_t count = SX1278_ReadReg(pSpiHandle, REG_RX_NB_BYTES);
    if (count > maxLen) count = maxLen;

    uint8_t currentAddr = SX1278_ReadReg(pSpiHandle, REG_FIFO_RX_CURRENT_ADDR);
    SX1278_WriteReg(pSpiHandle, REG_FIFO_ADDR_PTR, currentAddr);

    for (uint8_t i = 0; i < count; i++)
    {
        rxBuf[i] = SX1278_ReadReg(pSpiHandle, REG_FIFO);
    }

    int8_t snr = (int8_t)SX1278_ReadReg(pSpiHandle, REG_PKT_SNR_VALUE);
    uint8_t rssi_raw = SX1278_ReadReg(pSpiHandle, REG_PKT_RSSI_VALUE);
    int16_t rssi = -164 + rssi_raw;

    if (pRSSI) *pRSSI = rssi;
    if (pSNR)  *pSNR  = snr / 4;

    SX1278_WriteReg(pSpiHandle, REG_IRQ_FLAGS, IRQ_RX_DONE_MASK);

    return count;
}

