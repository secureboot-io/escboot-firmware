#include "utils/millis.h"

uint32_t millis() {
	return HAL_GetTick();
}