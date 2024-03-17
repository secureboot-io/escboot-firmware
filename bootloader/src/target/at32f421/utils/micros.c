#include "utils/micros.h"

#include "at32f421.h"
// #include "stm32f0xx_ll_cortex.h"

__STATIC_INLINE uint32_t LL_SYSTICK_IsActiveCounterFlag(void)
{
  return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}

volatile uint32_t ticks;

uint32_t micros(void)
{
    /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
    LL_SYSTICK_IsActiveCounterFlag();
    uint32_t m = ticks;
    const uint32_t tms = SysTick->LOAD + 1;
    __IO uint32_t u = tms - SysTick->VAL;
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
        m = ticks;
        u = tms - SysTick->VAL;
    }
    return (m * 1000 + (u * 1000) / tms);
}