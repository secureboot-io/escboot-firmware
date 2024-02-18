#include "io/gpio.h"

#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_bus.h"

void pinSetInputPullDownMode()
{
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinSetOutputMode()
{
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinWrite(bool set)
{
    LL_GPIO_WriteOutputPort(GPIOA, set ? LL_GPIO_PIN_2 : 0);
}

bool pinRead()
{
    return LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2);
}

bool pinReadSample()
{
    int i = 0, p = 0;
    do {
        p += LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2) ? 1 : -1;
        i++;
    } while(i < 4);
    return p >= 0;
}

void pinInit()
{
    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    pinSetInputPullDownMode();
}