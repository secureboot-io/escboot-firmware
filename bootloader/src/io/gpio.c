#include "io/gpio.h"

#include "stm32f4xx_hal.h"

void pinSetInputPullDown()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void pinSetOutput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    //GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void pinInit()
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    pinSetInputPullDown();
}