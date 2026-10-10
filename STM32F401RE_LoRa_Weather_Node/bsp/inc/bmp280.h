
#ifndef BSP_INC_BMP280_H_
#define BSP_INC_BMP280_H_

#include <stdint.h>
#include <stdbool.h>
#include "BoardConfig.h"

typedef struct {
    float temperature_c;
    float pressure_hpa;
    float altitude_m;
    bool  is_valid;
} BMP280_Data_t;

bool BMP280_Init(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr);

bool BMP280_ReadData(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr, BMP280_Data_t *pData);

#endif

