#include "utils/millis.h"

#include "stm32f4xx_hal.h"

uint32_t millis() {
	return HAL_GetTick();
}