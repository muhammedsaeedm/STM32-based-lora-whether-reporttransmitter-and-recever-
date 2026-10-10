
#ifndef BSP_INC_OLED_SH1106_H_
#define BSP_INC_OLED_SH1106_H_

#include <stdint.h>
#include <stdbool.h>
#include "BoardConfig.h"

#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_COLOR_BLACK    0
#define OLED_COLOR_WHITE    1
#define OLED_COLOR_INVERT   2

bool OLED_Init(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr, bool is_sh1106);

void OLED_SetColumnOffset(uint8_t offset);
uint8_t OLED_GetColumnOffset(void);

void OLED_Clear(void);

void OLED_Update(void);

void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color);
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void OLED_DrawHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
void OLED_DrawVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void OLED_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);

void OLED_DrawChar(int16_t x, int16_t y, char c, uint8_t color);
void OLED_DrawString(int16_t x, int16_t y, const char *str, uint8_t color);
void OLED_DrawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t percent);

#endif

