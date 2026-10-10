
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

bool SX1278_Init(const SX1278_Config_t *pConfig);

void SX1278_StartReceive(void);

bool SX1278_IsPacketAvailable(void);

uint8_t SX1278_ReceivePacket(uint8_t *rxBuf, uint8_t maxLen, int16_t *pRSSI, int8_t *pSNR);

void SX1278_SetStandby(void);
void SX1278_SetSleep(void);

uint8_t SX1278_ReadVersion(void);

#endif

