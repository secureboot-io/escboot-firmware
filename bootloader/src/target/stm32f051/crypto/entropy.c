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
static void Error_Handler(void);
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
    __HAL_RCC_LSI_DISABLE();

    /* Get Start Tick */
    uint32_t tickstart = millis();

    /* Wait till LSI is disabled */
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) != RESET)
    {
        if ((millis() - tickstart) > 2)
        {
            return;
        }
    }

    __HAL_RCC_LSI_ENABLE();

    /* Get Start Tick */
    tickstart = millis();

    /* Wait till LSI is ready */
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
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
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    /**TIM16 GPIO Configuration
    PA6   ------> TIM16_CH1
    */
    //   GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    //   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    //   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    //   GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    //   LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_5);

    /* USER CODE BEGIN TIM16_Init 1 */

    /* USER CODE END TIM16_Init 1 */
    // TIM_InitStruct.Prescaler = 0;
    // TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    // TIM_InitStruct.Autoreload = 65535;
    // TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    // TIM_InitStruct.RepetitionCounter = 0;
    // LL_TIM_Init(TIM16, &TIM_InitStruct);

    uint32_t tmpcr1 = LL_TIM_ReadReg(TIM16, CR1);

    // if (IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIM16))
    // {
    //     /* Select the Counter Mode */
    //     MODIFY_REG(tmpcr1, (TIM_CR1_DIR | TIM_CR1_CMS), LL_TIM_COUNTERMODE_UP);
    // }

    // if (IS_TIM_CLOCK_DIVISION_INSTANCE(TIM16))
    //{
    /* Set the clock division */
    MODIFY_REG(tmpcr1, TIM_CR1_CKD, LL_TIM_CLOCKDIVISION_DIV1);
    //}

    /* Write to TIMx CR1 */
    LL_TIM_WriteReg(TIM16, CR1, tmpcr1);

    /* Set the Autoreload value */
    LL_TIM_SetAutoReload(TIM16, 65535);

    /* Set the Prescaler value */
    LL_TIM_SetPrescaler(TIM16, 0);

    // if (IS_TIM_REPETITION_COUNTER_INSTANCE(TIM16))
    //{
    /* Set the Repetition Counter value */
    LL_TIM_SetRepetitionCounter(TIM16, 0);
    //}

    /* Generate an update event to reload the Prescaler
       and the repetition counter value (if applicable) immediately */
    LL_TIM_GenerateEvent_UPDATE(TIM16);

    LL_TIM_DisableARRPreload(TIM16);
    LL_TIM_IC_SetActiveInput(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);

    // TIM_IC_InitTypeDef sConfigIC = {0};

    // htimx.Instance = TIM16;
    // htimx.Init.Prescaler = 0;
    // htimx.Init.CounterMode = TIM_COUNTERMODE_UP;
    // htimx.Init.Period = 0xFFFFU; /* Set Max Period */
    // htimx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // htimx.Init.RepetitionCounter = 0;
    // htimx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    // if (HAL_TIM_Base_Init(&htimx) != HAL_OK)
    // {
    //     Error_Handler();
    // }

    // if (HAL_TIM_IC_Init(&htimx) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    // sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    // sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    // sConfigIC.ICFilter = 0;
    // if (HAL_TIM_IC_ConfigChannel(&htimx, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // if (HAL_TIMEx_TISelection(&htimx, TIM_TIM16_TI1_LSI, TIM_CHANNEL_1) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    /* TIM interrupt Init */
    NVIC_SetPriority(TIM16_IRQn, 0);
    NVIC_EnableIRQ(TIM16_IRQn);
}

/**
 * @brief  Start timer to measure LSI cycle length
 * @param  None
 * @retval None
 */
static void StartMeasureTimer(void)
{
    // if (HAL_TIM_IC_Start_IT(&htimx, TIM_CHANNEL_1) != HAL_OK)
    // {
    //     Error_Handler();
    // }
}

/**
 * @brief  Input Capture callback in non-blocking mode
 * @param  htim TIM IC handle
 * @retval None
 */
void HAL_TIM_IC_CaptureCallback()
{
    uint32_t meas = LL_TIM_IC_GetCaptureCH1(TIM16);
    //HAL_TIM_IC_Stop_IT(&htimx, TIM_CHANNEL_1);

    pEntropyBuffer[EntropyIndex / 8] |= (uint8_t)((meas & 0x01UL) << (7 - (EntropyIndex & 0x07UL)));
    EntropyIndex++;

    if (EntropyIndex < EntropyMax)
    {
        RestartLSI();
        StartMeasureTimer();
    }
}

// /**
//  * @brief  This function is executed in case of error occurrence.
//  * @param  None
//  * @retval None
//  */
// static void Error_Handler(void)
// {
//     /* User may add here some code to deal with this error */
//     /* Toggle LED4 @2Hz to notify error condition */
//     while (1)
//     {
//         BSP_LED_Toggle(LED4);
//         HAL_Delay(250);
//     }
// }

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
