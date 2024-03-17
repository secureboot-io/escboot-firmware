#include "io/gpio.h"
#include "utils/micros.h"
#include "at32f421.h"
// #include "stm32f0xx_ll_bus.h"

void pinSetInputPullUp()
{
//     gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
//   gpio_init_struct.gpio_pins = GPIO_PINS_0;
//   gpio_init_struct.gpio_pull = GPIO_PULL_UP;
//   gpio_init(GPIOC, &gpio_init_struct);
    GPIOA->cfgr_bit.iomc2 = GPIO_MODE_INPUT;
    GPIOA->pull_bit.pull2 = GPIO_PULL_UP;

    // LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);
    // LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);
    // LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinSetInputPullDown()
{
    GPIOA->cfgr_bit.iomc2 = GPIO_MODE_INPUT;
    GPIOA->pull_bit.pull2 = GPIO_PULL_DOWN;
    // LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);
    // LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_DOWN);
    // LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinSetInputPullNone()
{
    GPIOA->cfgr_bit.iomc2 = GPIO_MODE_INPUT;
    GPIOA->pull_bit.pull2 = GPIO_PULL_NONE;
    // LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);
    // LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);
    // LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinSetOutput()
{
    GPIOA->cfgr_bit.iomc2 = GPIO_MODE_OUTPUT;
    GPIOA->omode_bit.om2 = GPIO_OUTPUT_PUSH_PULL;
    // LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
    // LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
    // LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
}

void pinWrite(bool set)
{
    GPIOA->odt_bit.odt2 = set;
    // LL_GPIO_WriteOutputPort(GPIOA, set ? LL_GPIO_PIN_2 : 0);
}

bool pinRead()
{
    return GPIOA->idt_bit.idt2;
    // return LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2);
}

bool pinReadSample()
{
    int i = 0, p = 0;
    do {
        p += GPIOA->idt_bit.idt2 ? 1 : -1;
        i++;
    } while(i < 4);
    return p >= 0;
}

bool pinHasSignal()
{
    //check with pin pull down
    pinSetInputPullDown();
    //wait 500 initially then sample at 10us intervals for 500 samples
    uint32_t startTime = micros() + 500;
    uint32_t lowCount =  0;
    for(int i = 0; i < 500; i++) {
        //wait for start
        while(micros() < startTime);
        //read pin
        lowCount += pinRead() ? 0 : 1;
        startTime += 10;
    }
    //if none of the samplke was low signal was present
    if(lowCount == 0) {
        return true;
    }

    //check with pin floating
    pinSetInputPullNone();
    //wait 500 initially then sample at 10us intervals for 500 samples
    startTime = micros() + 500;
    lowCount =  0;
    for(int i = 0; i < 500; i++) {
        //wait for start
        while(micros() < startTime);
        //read pin
        lowCount += pinRead() ? 0 : 1;
        startTime += 10;
    }
    //if none of the samplke was low signal was present
    if(lowCount == 0) {
        return true;
    }

    //signal not found
    return false;
}

void pinInit()
{
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    /* GPIO Ports Clock Enable */
    // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
}