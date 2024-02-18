/**
 ******************************************************************************
 * @file    entropy.c
 * @author  MCD Application Team
 * @brief   This file contains functions for Entropy gathering utility
 * @note    This solution should be used only if the target device doesn’t provide
 *          a TRNG peripheral, and its usage is discouraged for critical security
 *          or cryptographic purposes.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// #include "main.h"
#include "crypto/entropy.h"
#include "stm32f0xx.h"
#include "utils/millis.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_rcc.h"
#include <stdio.h>
/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// TIM_HandleTypeDef htimx;
static volatile uint32_t EntropyIndex;
static volatile uint32_t EntropyMax;
static volatile uint8_t *pEntropyBuffer;
/* Private function prototypes -----------------------------------------------*/
static void RestartLSI(void);
static void ConfigureMeasureTimer(void);
static void StartMeasureTimer(void);
// static void Error_Handler(void);
void TIM_IRQ_HANDLER(void);
/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Entropy gathering utility
 * @param  nbBits    Number of entropy bits to gather
 * @param  pEntropy  Buffer to receive entropy data (must be enough large to contain nbBits bits)
 * @retval None
 */
void Entropy_Gather(uint32_t nbBits, volatile uint8_t *pEntropy)
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

#define RCC_CSR_LSIRDY_BitNumber 1
#define CSR_REG_INDEX ((uint8_t)4U)
#define RCC_FLAG_LSIRDY ((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_LSIRDY_BitNumber))
#define RCC_FLAG_MASK ((uint8_t)0x1FU)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U) ? RCC->CR : ((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR : ((((__FLAG__) >> 5U) == 3U) ? RCC->CSR : RCC->CIR))) & (1U << ((__FLAG__) & RCC_FLAG_MASK))) != 0U) ? 1U : 0U)

/** @brief Macro to enable the Internal Low Speed oscillator (LSI).
 * @note   After enabling the LSI, the application software should wait on
 *         LSIRDY flag to be set indicating that LSI clock is stable and can
 *         be used to clock the IWDG and/or the RTC.
 */
#define __HAL_RCC_LSI_ENABLE() SET_BIT(RCC->CSR, RCC_CSR_LSION)

/** @brief Macro to disable the Internal Low Speed oscillator (LSI).
 * @note   LSI can not be disabled if the IWDG is running.
 * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
 *         clock cycles.
 */
#define __HAL_RCC_LSI_DISABLE() CLEAR_BIT(RCC->CSR, RCC_CSR_LSION)
/**
 * @brief  LSI restart utility
 * @param  None
 * @retval None
 */
static void RestartLSI(void)
{
    /* Disable the Internal Low Speed oscillator (LSI). */
    LL_RCC_LSI_Disable();

    uint32_t tickstart = millis();
    /* Wait till LSI is disabled */
    while(LL_RCC_LSI_IsReady() == 1)
    {
        if ((millis() - tickstart) > 2)
        {
            return;
        }
    }

    LL_RCC_LSI_Enable();
    /* Get Start Tick */
    tickstart = millis();
    /* Wait till LSI is ready */
    while(LL_RCC_LSI_IsReady() == 0)
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
    LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_LSI, LL_RCC_MCO1_DIV_1);
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);

    uint32_t tmpcr1 = LL_TIM_ReadReg(TIM14, CR1);
    MODIFY_REG(tmpcr1, TIM_CR1_CKD, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_WriteReg(TIM14, CR1, tmpcr1);

    /* Set the Autoreload value */
    LL_TIM_SetAutoReload(TIM14, 65535);

    /* Set the Prescaler value */
    LL_TIM_SetPrescaler(TIM14, 0);

    LL_TIM_GenerateEvent_UPDATE(TIM14);

    LL_TIM_DisableARRPreload(TIM14);
    LL_TIM_IC_SetActiveInput(TIM14, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM14, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM14, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM14, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_SetRemap(TIM14, LL_TIM_TIM14_TI1_RMP_MCO);

    NVIC_SetPriority(TIM14_IRQn, 0);
    NVIC_EnableIRQ(TIM14_IRQn);
}

static void StartMeasureTimer(void)
{
    LL_TIM_EnableIT_CC1(TIM14);
    LL_TIM_CC_EnableChannel(TIM14, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM14);
}

/**
 * @brief  Input Capture callback in non-blocking mode
 * @param  htim TIM IC handle
 * @retval None
 */
void TIM14_IRQHandler(void)
{
    uint32_t meas = LL_TIM_IC_GetCaptureCH1(TIM14);

    LL_TIM_ClearFlag_CC1(TIM14);
    LL_TIM_CC_DisableChannel(TIM14, LL_TIM_CHANNEL_CH1);
    LL_TIM_DisableCounter(TIM14);

    pEntropyBuffer[EntropyIndex / 8] |= (uint8_t)((meas & 0x01UL) << (7 - (EntropyIndex & 0x07UL)));
    EntropyIndex++;

    if (EntropyIndex < EntropyMax)
    {
        RestartLSI();
        StartMeasureTimer();
    }
}
