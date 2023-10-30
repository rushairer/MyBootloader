#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"

typedef enum {
    OLED_COLOR_NORMAL   = 0,
    OlED_COLOR_REVERSED = 1,
} Oled_Color;

typedef struct {
    uint32_t RCC_APB2Periph;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Scl_Pin;
    uint16_t GPIO_Sda_Pin;
    uint8_t Buffer[8][128];
} Oled;

void Oled_Init(
    Oled *this,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Scl_Pin,
    uint16_t GPIO_Sda_Pin);

void Oled_Display_On(Oled *this);
void Oled_Display_Off(Oled *this);
void Oled_Clear(Oled *this);
void Oled_RefreshScreen(Oled *this);
void Oled_CleanBuffer(Oled *this);

void Oled_SetPixel(Oled *this, uint8_t x, uint8_t y, uint8_t color);
void Oled_SetByte(Oled *this, uint8_t Line, uint8_t Column, uint8_t data, uint8_t color);
void Oled_DrawLine(Oled *this, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);

// TODO: 实现Buffer缓冲区的方式，全屏刷新
void Oled_ShowChar(Oled *this, uint8_t Line, uint8_t Column, char Char);
void Oled_ShowString(Oled *this, uint8_t Line, uint8_t Column, char *String);
void Oled_ShowNum(Oled *this, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void Oled_ShowSignedNum(Oled *this, uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void Oled_ShowHexNum(Oled *this, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void Oled_ShowBinNum(Oled *this, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void Oled_DrawBMP(Oled *this, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);

void Oled_WriteCommand(Oled *this, uint8_t Command);
void Oled_I2c_SendByte(Oled *this, uint8_t Byte);
void Oled_I2c_Start(Oled *this);
void Oled_I2c_Stop(Oled *this);
void Oled_I2c_Wait(Oled *this);

#endif // !__OLED_H