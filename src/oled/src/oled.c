#include "oled.h"
#include "delay.h"
#include "oled_font.h"
#include <string.h>

void Oled_CallScl(Oled *this, BitAction BitVal)
{
    GPIO_WriteBit(this->GPIOx, this->GPIO_Scl_Pin, BitVal);
}

void Oled_CallSda(Oled *this, BitAction BitVal)
{
    GPIO_WriteBit(this->GPIOx, this->GPIO_Sda_Pin, BitVal);
}

void Oled_I2c_Init(
    Oled *this,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Scl_Pin,
    uint16_t GPIO_Sda_Pin)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    this->RCC_APB2Periph = RCC_APB2Periph;
    this->GPIOx          = GPIOx;
    this->GPIO_Scl_Pin   = GPIO_Scl_Pin;
    this->GPIO_Sda_Pin   = GPIO_Sda_Pin;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Scl_Pin | GPIO_Sda_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_SetBits(GPIOx, GPIO_Scl_Pin | GPIO_Sda_Pin);
}

void Oled_I2c_Start(Oled *this)
{
    Oled_CallScl(this, Bit_SET);
    Oled_CallSda(this, Bit_SET);

    Oled_CallSda(this, Bit_RESET);
    Oled_CallScl(this, Bit_RESET);
}

void Oled_I2c_Stop(Oled *this)
{
    Oled_CallScl(this, Bit_SET);

    Oled_CallSda(this, Bit_RESET);
    Oled_CallSda(this, Bit_SET);
}

void Oled_I2c_Wait(Oled *this)
{
    Oled_CallScl(this, Bit_SET);
    Oled_CallScl(this, Bit_RESET);
}

void Oled_I2c_SendByte(Oled *this, uint8_t Byte)
{

    uint8_t i;
    Oled_CallScl(this, Bit_RESET);
    for (i = 0; i < 8; i++) {
        Oled_CallSda(this, Byte & (0x80 >> i));
        Oled_CallScl(this, Bit_SET);
        Oled_CallScl(this, Bit_RESET);
    }
    // Oled_CallScl(this, Bit_SET);
    // Oled_CallScl(this, Bit_RESET);
}

void Oled_WriteCommand(Oled *this, uint8_t Command)
{
    Oled_I2c_Start(this);
    Oled_I2c_SendByte(this, 0x78);
    Oled_I2c_Wait(this);
    Oled_I2c_SendByte(this, 0x00);
    Oled_I2c_Wait(this);
    Oled_I2c_SendByte(this, Command);
    Oled_I2c_Wait(this);
    Oled_I2c_Stop(this);
}

void Oled_WriteData(Oled *this, uint8_t Data)
{
    Oled_I2c_Start(this);
    Oled_I2c_SendByte(this, 0x78);
    Oled_I2c_Wait(this);
    Oled_I2c_SendByte(this, 0x40);
    Oled_I2c_Wait(this);
    Oled_I2c_SendByte(this, Data);
    Oled_I2c_Wait(this);
    Oled_I2c_Stop(this);
}

/**
 * @brief  OLED设置光标位置
 * @param this Oled实例
 * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
 * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
 * @retval 无
 */
void Oled_SetCursor(Oled *this, uint8_t X, uint8_t Y)
{
    Oled_WriteCommand(this, 0xB0 + Y); // 设置Y位置

    Oled_WriteCommand(this, 0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
    Oled_WriteCommand(this, 0x00 | (X & 0x0F));        // 设置X位置低4位
}

void Oled_CleanBuffer(Oled *this)
{
    memset(this->Buffer, 0, sizeof(this->Buffer));
}

void Oled_RefreshScreen(Oled *this)
{
    uint8_t i, j;
    for (i = 0; i < 8; i++) {
        Oled_SetCursor(this, 0, i);
        for (j = 0; j < 128; j++) {
            Oled_WriteData(this, this->Buffer[i][j]);
        }
    }
}

void Oled_Init(
    Oled *this,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t GPIO_Scl_Pin,
    uint16_t GPIO_Sda_Pin)
{
    Oled_I2c_Init(this, RCC_APB2Periph, GPIOx, GPIO_Scl_Pin, GPIO_Sda_Pin);

    Delay_ms(800);

    Oled_WriteCommand(this, 0xAE); // 关闭显示
    Oled_WriteCommand(this, 0x00); // set low column address
    Oled_WriteCommand(this, 0x10); // set high column address

    Oled_WriteCommand(this, 0x40); // 设置显示开始行
    Oled_WriteCommand(this, 0xB0); // set page address

    Oled_WriteCommand(this, 0x81); // 设置对比度控制
    Oled_WriteCommand(this, 0x60);

    Oled_WriteCommand(this, 0xA1); // 设置左右方向，0xA1正常 0xA0左右反置
    Oled_WriteCommand(this, 0xA6); // 设置正常/倒转显示
    Oled_WriteCommand(this, 0xA8); // 设置多路复用率
    Oled_WriteCommand(this, 0x3F); // 1/32 duty
    Oled_WriteCommand(this, 0xC8); // 设置上下方向，0xC8正常 0xC0上下反置

    Oled_WriteCommand(this, 0xD3); // 设置显示偏移
    Oled_WriteCommand(this, 0x00);

    Oled_WriteCommand(this, 0xD5); // 设置显示时钟分频比/振荡器频率
    Oled_WriteCommand(this, 0x80);

    Oled_WriteCommand(this, 0xD8); // set area color mode off
    Oled_WriteCommand(this, 0x05);

    Oled_WriteCommand(this, 0xD9); // 设置预充电周期
    Oled_WriteCommand(this, 0xF1);

    Oled_WriteCommand(this, 0xDA); // 设置COM引脚硬件配置
    Oled_WriteCommand(this, 0x12);

    Oled_WriteCommand(this, 0xDB); // 设置VCOMH取消选择级别
    Oled_WriteCommand(this, 0x30);

    Oled_WriteCommand(this, 0x8D); // 设置充电泵
    Oled_WriteCommand(this, 0x14);

    // Oled_WriteCommand(this, 0xA4); // 设置整个显示打开/关闭

    Oled_WriteCommand(this, 0xAF); // 开启显示

    // Oled_Clear(this);
    Oled_CleanBuffer(this);
    Oled_RefreshScreen(this);
}

void Oled_Clear(Oled *this)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++) {
        Oled_SetCursor(this, 0, j);
        for (i = 0; i < 128; i++) {
            Oled_WriteData(this, 0x00);
        }
    }
}

void Oled_SetPixel(Oled *this, uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= 128 || y >= 64) {
        return;
    }

    if (color) {
        this->Buffer[y / 8][x] &= ~(1 << (y % 8));
    } else {
        this->Buffer[y / 8][x] |= 1 << (y % 8);
    }
}

void Oled_SetByte(Oled *this, uint8_t Line, uint8_t Column, uint8_t data, uint8_t color)
{
    if (Line >= 8 || Column >= 128) {
        return;
    }

    if (color) {
        data = ~data;
    }

    this->Buffer[Line][Column] = data;
}

void Oled_DrawLine(Oled *this, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    static uint8_t temp = 0;
    if (x1 == x2) {
        if (y1 > y2) {
            temp = y1;
            y1   = y2;
            y2   = temp;
        }
        for (uint8_t y = y1; y <= y2; y++) {
            Oled_SetPixel(this, x1, y, color);
        }
    } else if (y1 == y2) {
        if (x1 > x2) {
            temp = x1;
            x1   = x2;
            x2   = temp;
        }
        for (uint8_t x = x1; x <= x2; x++) {
            Oled_SetPixel(this, x, y1, color);
        }
    } else {
        // Bresenham直线算法
        int16_t dx = x2 - x1;
        int16_t dy = y2 - y1;
        int16_t ux = ((dx > 0) << 1) - 1;
        int16_t uy = ((dy > 0) << 1) - 1;
        int16_t x = x1, y = y1, eps = 0;
        dx = abs(dx);
        dy = abs(dy);
        if (dx > dy) {
            for (x = x1; x != x2; x += ux) {
                Oled_SetPixel(this, x, y, color);
                eps += dy;
                if ((eps << 1) >= dx) {
                    y += uy;
                    eps -= dx;
                }
            }
        } else {
            for (y = y1; y != y2; y += uy) {
                Oled_SetPixel(this, x, y, color);
                eps += dx;
                if ((eps << 1) >= dy) {
                    x += ux;
                    eps -= dy;
                }
            }
        }
    }
}

/**
 * @brief  OLED显示一个字符
 * @param this Oled实例
 * @param  Line 行位置，范围：1~4
 * @param  Column 列位置，范围：1~16
 * @param  Char 要显示的一个字符，范围：ASCII可见字符
 * @retval 无
 */
void Oled_ShowChar(Oled *this, uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    Oled_SetCursor(this, (Column - 1) * 8, (Line - 1) * 2); // 设置光标位置在上半部分
    for (i = 0; i < 8; i++) {
        Oled_WriteData(this, OLED_F8x16[Char - ' '][i]); // 显示上半部分内容
    }
    Oled_SetCursor(this, (Column - 1) * 8, (Line - 1) * 2 + 1); // 设置光标位置在下半部分
    for (i = 0; i < 8; i++) {
        Oled_WriteData(this, OLED_F8x16[Char - ' '][i + 8]); // 显示下半部分内容
    }
}

/**
 * @brief  OLED显示字符串
 * @param  this Oled实例
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  String 要显示的字符串，范围：ASCII可见字符
 * @retval 无
 */
void Oled_ShowString(Oled *this, uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) {
        Oled_ShowChar(this, Line, Column + i, String[i]);
    }
}

uint32_t Oled_Pow(Oled *this, uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--) {
        Result *= X;
    }
    return Result;
}

/**
 * @brief  OLED显示数字（十进制，正数）
 * @param  this Oled实例
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~4294967295
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
void Oled_ShowNum(Oled *this, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) {
        Oled_ShowChar(this, Line, Column + i, Number / Oled_Pow(this, 10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED显示数字（十进制，带符号数）
 * @param  this Oled实例
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：-2147483648~2147483647
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
void Oled_ShowSignedNum(Oled *this, uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0) {
        Oled_ShowChar(this, Line, Column, '+');
        Number1 = Number;
    } else {
        Oled_ShowChar(this, Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++) {
        Oled_ShowChar(this, Line, Column + i + 1, Number1 / Oled_Pow(this, 10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED显示数字（十六进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
 * @param  Length 要显示数字的长度，范围：1~8
 * @retval 无
 */
void Oled_ShowHexNum(Oled *this, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++) {
        SingleNumber = Number / Oled_Pow(this, 16, Length - i - 1) % 16;
        if (SingleNumber < 10) {
            Oled_ShowChar(this, Line, Column + i, SingleNumber + '0');
        } else {
            Oled_ShowChar(this, Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
 * @brief  OLED显示数字（二进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
 * @param  Length 要显示数字的长度，范围：1~16
 * @retval 无
 */
void Oled_ShowBinNum(Oled *this, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) {
        Oled_ShowChar(this, Line, Column + i, Number / Oled_Pow(this, 2, Length - i - 1) % 2 + '0');
    }
}

void Oled_DrawBMP(Oled *this, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
{
    uint16_t i = 0, x, y;
    for (y = y0; y < y1; y++) {
        Oled_SetCursor(this, x0, y);
        for (x = x0; x < x1; x++) {
            Oled_WriteData(this, BMP[i++]);
        }
    }
}

void Oled_Display_On(Oled *this)
{
    Oled_WriteCommand(this, 0x8D);
    Oled_WriteCommand(this, 0x14);
    Oled_WriteCommand(this, 0xAF);
}

void Oled_Display_Off(Oled *this)
{
    Oled_WriteCommand(this, 0x8D);
    Oled_WriteCommand(this, 0x10);
    Oled_WriteCommand(this, 0xAE);
}