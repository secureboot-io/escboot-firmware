#include "bootloader.h"
// #include "target_bootloader.h"
// #include "stm32f0xx_ll_usart.h"
// #include "stm32f0xx_ll_gpio.h"
// #include "stm32f0xx_ll_bus.h"
// #include "stm32f0xx_ll_rcc.h"
// #include "stm32f0xx_ll_system.h"
// #include "stm32f0xx_ll_utils.h"
// #include "stm32f0xx_ll_cortex.h"
// #include "cmox_crypto.h"
#include "stdio.h"
#include "debug/logging.h"
#include "at32f421.h"

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

uint32_t systick_interrupt_config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
  SysTick->VAL   = 0UL;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk |
                   SysTick_CTRL_ENABLE_Msk;
  return (0UL);
}

void wk_ertc_init(void)
{
  /* add user code begin ertc_init 0 */

  /* add user code end ertc_init 0 */

  /* add user code begin ertc_init 1 */

  /* add user code end ertc_init 1 */

  pwc_battery_powered_domain_access(TRUE);

  crm_ertc_clock_select(CRM_ERTC_CLOCK_LICK);
  crm_ertc_clock_enable(TRUE);
  ertc_reset();
  ertc_wait_update();
  ertc_divider_set(127, 255);
  ertc_hour_mode_set(ERTC_HOUR_MODE_24);

  /* add user code begin ertc_init 2 */

  /* add user code end ertc_init 2 */
}

void wk_system_clock_config(void);

int main(void)
{
    wk_system_clock_config();
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    wk_ertc_init();

    // wk_periph_clock_config();

    systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_DIV8);
    systick_interrupt_config(system_core_clock / 8 / 1000);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(SysTick_IRQn, 0, 0);

    __enable_irq();

    blMain();
}

void wk_system_clock_config(void)
{
    /* reset crm */
    crm_reset();

    /* config flash psr register */
    flash_psr_set(FLASH_WAIT_CYCLE_3);

    /* enable lick */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

    /* wait till lick is ready */
    while (crm_flag_get(CRM_LICK_STABLE_FLAG) != SET)
    {
    }

    /* enable hick */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

    /* wait till hick is ready */
    while (crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
    {
    }

    /* config pll clock resource */
    crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);

    /* enable pll */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

    /* wait till pll is ready */
    while (crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
    {
    }

    /* config ahbclk */
    crm_ahb_div_set(CRM_AHB_DIV_1);

    /* config apb2clk */
    crm_apb2_div_set(CRM_APB2_DIV_1);

    /* config apb1clk */
    crm_apb1_div_set(CRM_APB1_DIV_1);

    /* enable auto step mode */
    crm_auto_step_mode_enable(TRUE);

    /* select pll as system clock source */
    crm_sysclk_switch(CRM_SCLK_PLL);

    /* wait till pll is used as system clock source */
    while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
    {
    }

    /* disable auto step mode */
    crm_auto_step_mode_enable(FALSE);

    /* update system_core_clock global variable */
    system_core_clock_update();
}

void wk_periph_clock_config(void)
{
    /* enable gpioa periph clock */
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    /* enable usart1 periph clock */
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
