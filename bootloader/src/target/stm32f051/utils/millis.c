#include "utils/millis.h"

#include "stm32f0xx_hal.h"

uint32_t millis() {
	return HAL_GetTick();
}