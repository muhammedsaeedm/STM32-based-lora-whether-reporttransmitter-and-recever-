
#ifndef BSP_INC_SHT3X_H_
#define BSP_INC_SHT3X_H_

#include <stdint.h>
#include <stdbool.h>
#include "BoardConfig.h"

typedef struct {
    float temperature_c;
    float humidity_rh;
    bool is_valid;
} SHT3x_Data_t;

bool SHT3x_Init(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr);

bool SHT3x_ReadMeasurement(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr, SHT3x_Data_t *pData);

bool SHT3x_SoftReset(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr);

#endif

