
#include <math.h>
#include "bmp280.h"

#define BMP280_REG_TEMP_XLSB        0xFC
#define BMP280_REG_TEMP_LSB         0xFB
#define BMP280_REG_TEMP_MSB         0xFA
#define BMP280_REG_PRESS_XLSB       0xF9
#define BMP280_REG_PRESS_LSB        0xF8
#define BMP280_REG_PRESS_MSB        0xF7
#define BMP280_REG_CONFIG           0xF5
#define BMP280_REG_CTRL_MEAS        0xF4
#define BMP280_REG_STATUS           0xF3
#define BMP280_REG_RESET            0xE0
#define BMP280_REG_CHIPID           0xD0
#define BMP280_REG_CALIB00          0x88

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_Calib_t;

static BMP280_Calib_t g_calib;
static int32_t g_t_fine = 0;
static bool g_bmp280_detected = false;

static bool BMP280_ReadRegisters(I2C_Handle_t *pI2cHandle, uint8_t devAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
    I2C_MasterSendData(pI2cHandle, &regAddr, 1, devAddr, I2C_ENABLE_SR);
    I2C_MasterReceiveData(pI2cHandle, pData, len, devAddr, I2C_DISABLE_SR);
    return true;
}

static bool BMP280_WriteRegister(I2C_Handle_t *pI2cHandle, uint8_t devAddr, uint8_t regAddr, uint8_t val)
{
    uint8_t buf[2] = { regAddr, val };
    I2C_MasterSendData(pI2cHandle, buf, 2, devAddr, I2C_DISABLE_SR);
    return true;
}

bool BMP280_Init(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr)
{
    uint8_t chipId = 0;

    BMP280_ReadRegisters(pI2cHandle, i2c_addr, BMP280_REG_CHIPID, &chipId, 1);

    if (chipId == 0x58 || chipId == 0x60)
    {
        g_bmp280_detected = true;

        uint8_t calibBuf[24] = {0};
        BMP280_ReadRegisters(pI2cHandle, i2c_addr, BMP280_REG_CALIB00, calibBuf, 24);

        g_calib.dig_T1 = (uint16_t)((calibBuf[1] << 8) | calibBuf[0]);
        g_calib.dig_T2 = (int16_t) ((calibBuf[3] << 8) | calibBuf[2]);
        g_calib.dig_T3 = (int16_t) ((calibBuf[5] << 8) | calibBuf[4]);

        g_calib.dig_P1 = (uint16_t)((calibBuf[7] << 8) | calibBuf[6]);
        g_calib.dig_P2 = (int16_t) ((calibBuf[9] << 8) | calibBuf[8]);
        g_calib.dig_P3 = (int16_t) ((calibBuf[11] << 8) | calibBuf[10]);
        g_calib.dig_P4 = (int16_t) ((calibBuf[13] << 8) | calibBuf[12]);
        g_calib.dig_P5 = (int16_t) ((calibBuf[15] << 8) | calibBuf[14]);
        g_calib.dig_P6 = (int16_t) ((calibBuf[17] << 8) | calibBuf[16]);
        g_calib.dig_P7 = (int16_t) ((calibBuf[19] << 8) | calibBuf[18]);
        g_calib.dig_P8 = (int16_t) ((calibBuf[21] << 8) | calibBuf[20]);
        g_calib.dig_P9 = (int16_t) ((calibBuf[23] << 8) | calibBuf[22]);

        BMP280_WriteRegister(pI2cHandle, i2c_addr, BMP280_REG_CTRL_MEAS, 0x57);

        BMP280_WriteRegister(pI2cHandle, i2c_addr, BMP280_REG_CONFIG, 0x10);

        return true;
    }

    g_bmp280_detected = false;
    return false;
}

static int32_t BMP280_CompensateTemperature(int32_t adc_T)
{
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)g_calib.dig_T1 << 1))) * ((int32_t)g_calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)g_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)g_calib.dig_T1))) >> 12) * ((int32_t)g_calib.dig_T3)) >> 14;
    g_t_fine = var1 + var2;
    return (g_t_fine * 5 + 128) >> 8;
}

static uint32_t BMP280_CompensatePressure(int32_t adc_P)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)g_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)g_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)g_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)g_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)g_calib.dig_P3) >> 8) + ((var1 * (int64_t)g_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)g_calib.dig_P1) >> 33;

    if (var1 == 0)
    {
        return 0;
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)g_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)g_calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)g_calib.dig_P7) << 4);

    return (uint32_t)p;
}

bool BMP280_ReadData(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr, BMP280_Data_t *pData)
{
    if (g_bmp280_detected)
    {
        uint8_t burst[6] = {0};
        BMP280_ReadRegisters(pI2cHandle, i2c_addr, BMP280_REG_PRESS_MSB, burst, 6);

        int32_t adc_P = (int32_t)(((uint32_t)burst[0] << 12) | ((uint32_t)burst[1] << 4) | ((uint32_t)burst[2] >> 4));
        int32_t adc_T = (int32_t)(((uint32_t)burst[3] << 12) | ((uint32_t)burst[4] << 4) | ((uint32_t)burst[5] >> 4));

        int32_t temp_c_int = BMP280_CompensateTemperature(adc_T);
        uint32_t press_pa_256 = BMP280_CompensatePressure(adc_P);

        pData->temperature_c = (float)temp_c_int / 100.0f;
        pData->pressure_hpa  = (float)press_pa_256 / 25600.0f;

        pData->altitude_m = 44330.0f * (1.0f - powf(pData->pressure_hpa / 1013.25f, 0.1902949f));
        pData->is_valid = true;
        return true;
    }

    static float simPress = 1013.4f;
    pData->temperature_c = 25.1f;
    pData->pressure_hpa  = simPress;
    pData->altitude_m    = 112.5f;
    pData->is_valid      = false;
    return false;
}

