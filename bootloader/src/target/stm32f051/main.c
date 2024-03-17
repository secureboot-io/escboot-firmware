#include "bootloader.h"
#include "target_bootloader.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_cortex.h"
#include "cmox_crypto.h"
#include "stdio.h"
#include "debug/logging.h"

void SystemClock_Config(void);

void blTargetReboot()
{
    NVIC_SystemReset();
}

typedef __attribute__((noreturn)) void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress;

bool blTargetGotoApplication()
{
    JumpAddress = *(__IO uint32_t *)(0x8001000 + 4);
    uint8_t value = *(uint8_t *)(0x8007c00);

    if (value != 0x01)
    {
        LOG_TRACE("application not found, not jumping to application");
        return false;
    }

    JumpToApplication = (pFunction)JumpAddress;

    __disable_irq();
    __set_MSP(*(__IO uint32_t *)0x8001000);
    JumpToApplication();
}

int main(void)
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    SystemClock_Config();

    blMain();
}

void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
    {
    }
    LL_RCC_HSI_Enable();

    /* Wait till HSI is ready */
    while (LL_RCC_HSI_IsReady() != 1)
    {
    }
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1)
    {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    }
    LL_Init1msTick(48000000);
    LL_SetSystemCoreClock(48000000);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
    LL_SYSTICK_EnableIT();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
