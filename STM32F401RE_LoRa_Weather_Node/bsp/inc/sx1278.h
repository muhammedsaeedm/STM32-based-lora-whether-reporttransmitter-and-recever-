
#ifndef BSP_INC_SX1278_H_
#define BSP_INC_SX1278_H_

#include <stdint.h>
#include <stdbool.h>
#include "BoardConfig.h"

typedef struct {
    uint32_t frequency_hz;
    uint8_t  power_dbm;
    uint8_t  spreading_factor;
    uint32_t bandwidth_hz;
    uint8_t  coding_rate;
} SX1278_Config_t;

bool SX1278_Init(SPI_Handle_t *pSpiHandle, const SX1278_Config_t *pConfig);

void SX1278_SetNssPin(GPIO_RegDef_t *pPort, uint8_t pin);

bool SX1278_SendPacket(SPI_Handle_t *pSpiHandle, const uint8_t *payload, uint8_t len);

bool SX1278_IsPacketAvailable(SPI_Handle_t *pSpiHandle);

uint8_t SX1278_ReceivePacket(SPI_Handle_t *pSpiHandle, uint8_t *rxBuf, uint8_t maxLen, int16_t *pRSSI, int8_t *pSNR);

void SX1278_SetStandby(SPI_Handle_t *pSpiHandle);
void SX1278_SetSleep(SPI_Handle_t *pSpiHandle);

uint8_t SX1278_ReadVersion(SPI_Handle_t *pSpiHandle);

#endif

