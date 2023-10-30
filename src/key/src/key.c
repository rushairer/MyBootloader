#include "key.h"
#include "delay.h"

void Key_Init(
    Key *this,
    uint32_t RCC_APB2Periph,
    GPIO_TypeDef *GPIOx,
    uint16_t *GPIO_Pins)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);

    this->RCC_APB2Periph = RCC_APB2Periph;
    this->GPIOx          = GPIOx;
    this->GPIO_Pins      = GPIO_Pins;

    uint16_t pins = 0;
    for (uint8_t i = 0; i < sizeof(GPIO_Pins); i++) {
        pins = pins | GPIO_Pins[i];
    }
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin   = pins;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

uint8_t Key_IsPressed(Key *this, uint16_t GPIO_Pin)
{
    uint8_t isPressed = 0;

    if (GPIO_ReadInputDataBit(this->GPIOx, GPIO_Pin) == 0) {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(this->GPIOx, GPIO_Pin) == 0)
            ;
        Delay_ms(20);
        isPressed = 1;
    }
    return isPressed;
}