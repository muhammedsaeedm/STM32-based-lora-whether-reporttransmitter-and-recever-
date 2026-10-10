
#include <string.h>
#include <stdlib.h>
#include "oled_sh1106.h"
#include "font6x8.h"

static I2C_Handle_t *g_pI2c = NULL;
static uint8_t g_oled_addr = OLED_I2C_ADDR_A;
static bool g_is_sh1106 = true;

static uint8_t g_framebuffer[8][OLED_WIDTH];
static uint8_t g_col_offset = 2;

void OLED_SetColumnOffset(uint8_t offset)
{
    g_col_offset = offset;
}

uint8_t OLED_GetColumnOffset(void)
{
    return g_col_offset;
}

static void OLED_WriteCommand(uint8_t cmd)
{
    uint8_t buf[2] = { 0x00, cmd };
    I2C_MasterSendData(g_pI2c, buf, 2, g_oled_addr, I2C_DISABLE_SR);
}

static void OLED_WriteData(uint8_t *data, uint16_t len)
{

    uint8_t packet[17];
    packet[0] = 0x40;

    uint16_t sent = 0;
    while (sent < len)
    {
        uint8_t chunk = (len - sent > 16) ? 16 : (len - sent);
        memcpy(&packet[1], &data[sent], chunk);
        I2C_MasterSendData(g_pI2c, packet, chunk + 1, g_oled_addr, I2C_DISABLE_SR);
        sent += chunk;
    }
}

bool OLED_Init(I2C_Handle_t *pI2cHandle, uint8_t i2c_addr, bool is_sh1106)
{
    g_pI2c = pI2cHandle;
    g_is_sh1106 = is_sh1106;

    Board_DelayMs(50);

    if (I2C_DeviceReady(pI2cHandle, i2c_addr))
    {
        g_oled_addr = i2c_addr;
    }
    else
    {
        uint8_t alt_addr = (i2c_addr == 0x3C) ? 0x3D : 0x3C;
        if (I2C_DeviceReady(pI2cHandle, alt_addr))
        {
            g_oled_addr = alt_addr;
        }
        else
        {
            g_oled_addr = i2c_addr;
        }
    }

    OLED_WriteCommand(0xAE);

    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);

    OLED_WriteCommand(0x8D);
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAD);
    OLED_WriteCommand(0x8B);

    OLED_WriteCommand(0x20);
    OLED_WriteCommand(0x02);

    OLED_WriteCommand(0xA1);
    OLED_WriteCommand(0xC8);

    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);
    OLED_WriteCommand(0x7F);

    OLED_WriteCommand(0xD9);
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);
    OLED_WriteCommand(0x40);

    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xAF);

    Board_DelayMs(20);

    OLED_Clear();
    OLED_FillRect(0, 0, 128, 64, OLED_COLOR_BLACK);
    OLED_DrawRect(0, 0, 128, 64, OLED_COLOR_WHITE);
    OLED_DrawString(16, 20, "STM32F401 OLED", OLED_COLOR_WHITE);
    OLED_DrawString(24, 34, "ONLINE - READY", OLED_COLOR_WHITE);
    OLED_Update();

    return true;
}

void OLED_Clear(void)
{
    memset(g_framebuffer, 0, sizeof(g_framebuffer));
}

void OLED_Update(void)
{
    uint8_t colOffset = g_is_sh1106 ? g_col_offset : 0;

    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_WriteCommand(0xB0 + page);
        OLED_WriteCommand(0x00 | (colOffset & 0x0F));
        OLED_WriteCommand(0x10 | ((colOffset >> 4) & 0x0F));

        OLED_WriteData(g_framebuffer[page], OLED_WIDTH);
    }
}

void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color)
{
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT)
    {
        return;
    }

    uint8_t page = y / 8;
    uint8_t bit  = y % 8;

    if (color == OLED_COLOR_WHITE)
    {
        g_framebuffer[page][x] |= (1 << bit);
    }
    else if (color == OLED_COLOR_BLACK)
    {
        g_framebuffer[page][x] &= ~(1 << bit);
    }
    else if (color == OLED_COLOR_INVERT)
    {
        g_framebuffer[page][x] ^= (1 << bit);
    }
}

void OLED_DrawHLine(int16_t x, int16_t y, int16_t w, uint8_t color)
{
    for (int16_t i = 0; i < w; i++)
    {
        OLED_DrawPixel(x + i, y, color);
    }
}

void OLED_DrawVLine(int16_t x, int16_t y, int16_t h, uint8_t color)
{
    for (int16_t i = 0; i < h; i++)
    {
        OLED_DrawPixel(x, y + i, color);
    }
}

void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1)
    {
        OLED_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color)
{
    OLED_DrawHLine(x, y, w, color);
    OLED_DrawHLine(x, y + h - 1, w, color);
    OLED_DrawVLine(x, y, h, color);
    OLED_DrawVLine(x + w - 1, y, h, color);
}

void OLED_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color)
{
    for (int16_t i = 0; i < h; i++)
    {
        OLED_DrawHLine(x, y + i, w, color);
    }
}

void OLED_DrawChar(int16_t x, int16_t y, char c, uint8_t color)
{
    if (c < 32 || c > 126)
    {
        c = ' ';
    }

    uint8_t index = c - 32;

    for (uint8_t col = 0; col < 6; col++)
    {
        uint8_t line = Font6x8[index][col];
        for (uint8_t row = 0; row < 8; row++)
        {
            if (line & (1 << row))
            {
                OLED_DrawPixel(x + col, y + row, color);
            }
            else
            {
                OLED_DrawPixel(x + col, y + row, (color == OLED_COLOR_WHITE) ? OLED_COLOR_BLACK : OLED_COLOR_WHITE);
            }
        }
    }
}

void OLED_DrawString(int16_t x, int16_t y, const char *str, uint8_t color)
{
    int16_t curX = x;
    while (*str)
    {
        OLED_DrawChar(curX, y, *str, color);
        curX += 6;
        if (curX + 6 > OLED_WIDTH)
        {
            break;
        }
        str++;
    }
}

void OLED_DrawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t percent)
{
    if (percent > 100) percent = 100;
    OLED_DrawRect(x, y, w, h, OLED_COLOR_WHITE);
    int16_t fillW = (w - 4) * percent / 100;
    if (fillW > 0)
    {
        OLED_FillRect(x + 2, y + 2, fillW, h - 4, OLED_COLOR_WHITE);
    }
}

