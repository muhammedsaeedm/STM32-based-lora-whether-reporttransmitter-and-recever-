
#include "sht3x.h"

#define SHT3X_CMD_MEAS_HIGHREP      0x2400
#define SHT3X_CMD_SOFT_RESET        0x30A2
#define SHT3X_CMD_HEATER_ENABLE     0x306D
#define SHT3X_CMD_HEATER_DISABLE    0x3066
#define SHT3X_CRC_POLYNOMIAL        0x31

static uint8_t SHT3x_CalculateCRC(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ SHT3X_CRC_POLYNOMIAL;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool SHT3x_Init(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr)
{
    if (!I2C_DeviceReady(pI2cHandle, i2c_addr))
    {
        return false;
    }

    return SHT3x_SoftReset(pI2cHandle, i2c_addr);
}

bool SHT3x_SoftReset(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr)
{
    uint8_t cmd[2] = { (uint8_t)(SHT3X_CMD_SOFT_RESET >> 8), (uint8_t)(SHT3X_CMD_SOFT_RESET & 0xFF) };
    I2C_MasterSendData(pI2cHandle, cmd, 2, i2c_addr, I2C_DISABLE_SR);
    Board_DelayMs(2);
    return true;
}

bool SHT3x_ReadMeasurement(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr, SHT3x_Data_t *pData)
{
    uint8_t cmd[2] = { (uint8_t)(SHT3X_CMD_MEAS_HIGHREP >> 8), (uint8_t)(SHT3X_CMD_MEAS_HIGHREP & 0xFF) };
    uint8_t rxBuf[6] = {0};

    I2C_MasterSendData(pI2cHandle, cmd, 2, i2c_addr, I2C_DISABLE_SR);

    Board_DelayMs(15);

    I2C_MasterReceiveData(pI2cHandle, rxBuf, 6, i2c_addr, I2C_DISABLE_SR);

    uint8_t temp_crc = SHT3x_CalculateCRC(&rxBuf[0], 2);
    uint8_t hum_crc  = SHT3x_CalculateCRC(&rxBuf[3], 2);

    if (temp_crc == rxBuf[2] && hum_crc == rxBuf[5] && (rxBuf[0] != 0 || rxBuf[1] != 0))
    {
        uint16_t rawTemp = (uint16_t)((rxBuf[0] << 8) | rxBuf[1]);
        uint16_t rawHum  = (uint16_t)((rxBuf[3] << 8) | rxBuf[4]);

        pData->temperature_c = -45.0f + (175.0f * (float)rawTemp / 65535.0f);

        pData->humidity_rh = 100.0f * (float)rawHum / 65535.0f;
        pData->is_valid = true;
        return true;
    }

    static float simTemp = 24.8f;
    static float simHum  = 54.2f;
    simTemp += 0.05f;
    if (simTemp > 28.0f) simTemp = 23.5f;
    pData->temperature_c = simTemp;
    pData->humidity_rh = simHum;
    pData->is_valid = false;
    return false;
}

