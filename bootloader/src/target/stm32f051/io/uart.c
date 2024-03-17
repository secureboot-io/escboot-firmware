#include "io/uart.h"
#include "io/gpio.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_rcc.h"

volatile bool isTxComplete = true;
uint8_t txRingBuffer[1024];
volatile uint32_t txRingBufferHead = 0;
volatile uint32_t txRingBufferTail = 0;

bool txRingBufferPush(uint8_t data)
{
    if ((txRingBufferHead + 1) % sizeof(txRingBuffer) == txRingBufferTail)
    {
        return false;
    }
    txRingBuffer[txRingBufferHead] = data;
    txRingBufferHead = (txRingBufferHead + 1) % sizeof(txRingBuffer);
    return true;
}

bool txRingBufferPop(uint8_t *data)
{
    if (txRingBufferHead == txRingBufferTail)
    {
        return false;
    }
    *data = txRingBuffer[txRingBufferTail];
    txRingBufferTail = (txRingBufferTail + 1) % sizeof(txRingBuffer);
    return true;
}

void USART1_IRQHandler(void)
{
    uint8_t d;
    __disable_irq();
    if (LL_USART_IsActiveFlag_TXE(USART1))
    {
        if (!txRingBufferPop(&d))
        {
            //LL_USART_TransmitData8(USART1, 'c');
            LL_USART_DisableIT_TXE(USART1);
            isTxComplete = true;
        }
        else
        {
            LL_USART_TransmitData8(USART1, d);
        }
    }
    __enable_irq();
}

void uartTransmit(uint8_t data)
{
    __disable_irq();
    if (isTxComplete == false)
    {
        txRingBufferPush(data);
    }
    else
    {
        isTxComplete = false;
        LL_USART_TransmitData8(USART1, data);
        LL_USART_EnableIT_TXE(USART1);
    }
    __enable_irq();
}

void uartFlush()
{
    while (!isTxComplete);
}

void uartInit(void)
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);

    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_NO);

    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_1);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);

    LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
    LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
    LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);

    LL_USART_SetBaudRate(USART1, LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE), LL_USART_OVERSAMPLING_16, 115200);

    LL_USART_DisableIT_CTS(USART1);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
}