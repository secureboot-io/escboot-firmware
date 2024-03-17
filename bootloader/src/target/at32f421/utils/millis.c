#include "utils/millis.h"
#include "utils/micros.h"

uint32_t millis() {
	return ticks;
}