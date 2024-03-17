#include "io/uart.h"
#include "io/gpio.h"
// #include "stm32f0xx_ll_bus.h"
// #include "stm32f0xx_ll_usart.h"
// #include "stm32f0xx_ll_gpio.h"
// #include "stm32f0xx_ll_rcc.h"
#include "at32f421.h"

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
    if (usart_flag_get(USART1, USART_TDBE_FLAG))
    {
        if (!txRingBufferPop(&d))
        {
            usart_interrupt_enable(USART1, USART_TDBE_INT, FALSE);
            isTxComplete = true;
        }
        else
        {
            usart_data_transmit(USART1, d);
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
        usart_data_transmit(USART1, data);
        usart_interrupt_enable(USART1, USART_TDBE_INT, TRUE);
    }
    __enable_irq();
}

void uartFlush()
{
    while (!isTxComplete);
}

void uartInit(void)
{
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    /* add user code begin usart1_init 1 */

    /* add user code end usart1_init 1 */

    /* configure the TX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_9;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE9, GPIO_MUX_1);

    /* configure the RX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_10;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE10, GPIO_MUX_1);

    /* configure param */
    usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(USART1, TRUE);
    usart_parity_selection_config(USART1, USART_PARITY_NONE);

    usart_hardware_flow_control_set(USART1, USART_HARDWARE_FLOW_NONE);

    /**
     * Users need to configure USART1 interrupt functions according to the actual application.
     * 1. Call the below function to enable the corresponding USART1 interrupt.
     *     --usart_interrupt_enable(...)
     * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
     *     --void USART1_IRQHandler(void)
     */

    usart_enable(USART1, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(USART1_IRQn, 0, 0);
    // NVIC_SetPriority(USART1_IRQn, 0);
    // NVIC_EnableIRQ(USART1_IRQn);
}