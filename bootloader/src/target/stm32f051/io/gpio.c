#include "io/gpio.h"

//#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_bus.h"

void pinSetInputPullDownMode()
{
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_1, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinSetOutputMode()
{
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinWrite(bool set)
{
    LL_GPIO_WriteOutputPort(GPIOC, set ? LL_GPIO_PIN_1 : 0);
}

bool pinRead()
{
    return LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_1);
}

void pinInit()
{
    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_1);

    //__HAL_RCC_GPIOC_CLK_ENABLE();
    pinSetInputPullDownMode();
}