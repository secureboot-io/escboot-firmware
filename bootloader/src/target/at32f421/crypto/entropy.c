// /**
//  ******************************************************************************
//  * @file    entropy.c
//  * @author  MCD Application Team
//  * @brief   This file contains functions for Entropy gathering utility
//  * @note    This solution should be used only if the target device doesn’t provide
//  *          a TRNG peripheral, and its usage is discouraged for critical security
//  *          or cryptographic purposes.
//  ******************************************************************************
//  * @attention
//  *
//  * Copyright (c) 2018 STMicroelectronics.
//  * All rights reserved.
//  *
//  * This software is licensed under terms that can be found in the LICENSE file in
//  * the root directory of this software component.
//  * If no LICENSE file comes with this software, it is provided AS-IS.
//  *
//  ******************************************************************************
//  */

// /* Includes ------------------------------------------------------------------*/
// // #include "main.h"
// #include "crypto/entropy.h"
// #include "stm32f0xx.h"
// #include "utils/millis.h"
// #include "stm32f0xx_ll_tim.h"
// #include "stm32f0xx_ll_gpio.h"
// #include "stm32f0xx_ll_bus.h"
// #include "stm32f0xx_ll_rcc.h"
// #include <stdio.h>
// /* Global variables ----------------------------------------------------------*/
// /* Private typedef -----------------------------------------------------------*/
// /* Private defines -----------------------------------------------------------*/
// /* Private macros ------------------------------------------------------------*/
// /* Private variables ---------------------------------------------------------*/
// // TIM_HandleTypeDef htimx;
// static volatile uint32_t EntropyIndex;
// static volatile uint32_t EntropyMax;
// static volatile uint8_t *pEntropyBuffer;
// /* Private function prototypes -----------------------------------------------*/
// static void RestartLSI(void);
// static void ConfigureMeasureTimer(void);
// static void StartMeasureTimer(void);
// // static void Error_Handler(void);
// void TIM_IRQ_HANDLER(void);
// /* Functions Definition ------------------------------------------------------*/
#include <stdint.h>
#include "at32f421_crm.h"

static volatile uint32_t EntropyIndex;
static volatile uint32_t EntropyMax;
static volatile uint8_t *pEntropyBuffer;


static void RestartLSI(void);
static void ConfigureMeasureTimer(void);
static void StartMeasureTimer(void);

/**
 * @brief  Entropy gathering utility
 * @param  nbBits    Number of entropy bits to gather
 * @param  pEntropy  Buffer to receive entropy data (must be enough large to contain nbBits bits)
 * @retval None
 */
void entropyGather(uint32_t nbBits, volatile uint8_t *pEntropy)
{
    EntropyIndex = 0;
    EntropyMax = nbBits;
    pEntropyBuffer = pEntropy;

    /* Reset entropy buffer contents */
    for (uint32_t i = 0U; i < (((nbBits & 0x7UL) == 0UL) ? (nbBits / 8) : (nbBits / 8 + 1)); i++)
    {
        pEntropy[i] = 0U;
    }

    /* Configure timer to measure LSI cycle length */
    ConfigureMeasureTimer();

    /* Restart LSI oscillator */
    RestartLSI();

    /* Launch measurement timer */
    StartMeasureTimer();

    /* Wait for all entropy bit to be gathered */
    while (EntropyIndex < EntropyMax)
    {
    }
}

// #define RCC_CSR_LSIRDY_BitNumber 1
// #define CSR_REG_INDEX ((uint8_t)4U)
// #define RCC_FLAG_LSIRDY ((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_LSIRDY_BitNumber))
// #define RCC_FLAG_MASK ((uint8_t)0x1FU)
// #define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U) ? RCC->CR : ((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : ((((__FLAG__) >> 5U) == 3U) ? RCC->CSR : RCC->CIR))) & (1U << ((__FLAG__) & RCC_FLAG_MASK))) != 0U) ? 1U : 0U)

// /** @brief Macro to enable the Internal Low Speed oscillator (LSI).
//  * @note   After enabling the LSI, the application software should wait on
//  *         LSIRDY flag to be set indicating that LSI clock is stable and can
//  *         be used to clock the IWDG and/or the RTC.
//  */
// #define __HAL_RCC_LSI_ENABLE() SET_BIT(RCC->CSR, RCC_CSR_LSION)

// /** @brief Macro to disable the Internal Low Speed oscillator (LSI).
//  * @note   LSI can not be disabled if the IWDG is running.
//  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
//  *         clock cycles.
//  */
// #define __HAL_RCC_LSI_DISABLE() CLEAR_BIT(RCC->CSR, RCC_CSR_LSION)
/**
 * @brief  LSI restart utility
 * @param  None
 * @retval None
 */
static void RestartLSI(void)
{
    /* Disable the Internal Low Speed oscillator (LSI). */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, FALSE);

    uint32_t tickstart = millis();
    /* Wait till LSI is disabled */
    while(crm_flag_get(CRM_LICK_STABLE_FLAG) == SET)
    {
        if ((millis() - tickstart) > 2)
        {
            return;
        }
    }

    crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);
    /* Get Start Tick */
    tickstart = millis();
    /* Wait till LSI is ready */
    while(crm_flag_get(CRM_LICK_STABLE_FLAG) == RESET)
    {
        if ((millis() - tickstart) > 2)
        {
            return;
        }
    }
}

/**
 * @brief  Configure timer to measure LSI cycle length
 * @param  None
 * @retval None
 */
static void ConfigureMeasureTimer(void)
{
    crm_periph_clock_enable(CRM_TMR14_PERIPH_CLOCK, TRUE);
  /* add user code begin tmr14_init 0 */

  /* add user code end tmr14_init 0 */

  tmr_input_config_type  tmr_input_struct;

  /* add user code begin tmr14_init 1 */

  /* add user code end tmr14_init 1 */

  /* configure counter settings */
  tmr_base_init(TMR14, 65535, 0);
  tmr_cnt_dir_set(TMR14, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR14, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR14, FALSE);

  /* configure overflow event */
//   tmr_overflow_event_disable(TMR14, TRUE);

  /* configure channel 1 input settings */
  tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_1;
  tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_struct.input_polarity_select = TMR_INPUT_FALLING_EDGE;
  tmr_input_struct.input_filter_value = 0;

  tmr_input_channel_init(TMR14, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);
  tmr_iremap_config(TMR14, TMR14_ERTCCLK);

//   tmr_counter_enable(TMR14, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR14_GLOBAL_IRQn, 0, 0);

//     NVIC_SetPriority(TIM14_IRQn, 0);
//     NVIC_EnableIRQ(TIM14_IRQn);
}

static void StartMeasureTimer(void)
{
    // LL_TIM_EnableIT_CC1(TIM14);
    // LL_TIM_CC_EnableChannel(TIM14, LL_TIM_CHANNEL_CH1);
    // LL_TIM_EnableCounter(TIM14);
    tmr_interrupt_enable(TMR14, TMR_C1_INT, TRUE);
    tmr_channel_enable(TMR14, TMR_SELECT_CHANNEL_1, TRUE);
    tmr_counter_enable(TMR14, TRUE);
}

/**
 * @brief  Input Capture callback in non-blocking mode
 * @param  htim TIM IC handle
 * @retval None
 */
void TMR14_GLOBAL_IRQHandler(void)
{
    uint32_t meas = tmr_channel_value_get(TMR14, TMR_SELECT_CHANNEL_1);

    // LL_TIM_ClearFlag_CC1(TIM14);
    // LL_TIM_CC_DisableChannel(TIM14, LL_TIM_CHANNEL_CH1);
    // LL_TIM_DisableCounter(TIM14);
    tmr_interrupt_enable(TMR14, TMR_C1_INT, FALSE);
    tmr_channel_enable(TMR14, TMR_SELECT_CHANNEL_1, FALSE);
    tmr_counter_enable(TMR14, FALSE);

    pEntropyBuffer[EntropyIndex / 8] |= (uint8_t)((meas & 0x01UL) << (7 - (EntropyIndex & 0x07UL)));
    EntropyIndex++;

    if (EntropyIndex < EntropyMax)
    {
        RestartLSI();
        StartMeasureTimer();
    }
}
