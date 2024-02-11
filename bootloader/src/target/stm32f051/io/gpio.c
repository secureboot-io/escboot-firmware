#include "io/gpio.h"

#include "stm32f0xx_hal.h"

void pinSetInputPullDownMode()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void pinSetOutputMode()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    //GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void pinWrite(bool set)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, set ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool pinRead()
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
}

void pinInit()
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    pinSetInputPullDownMode();
}