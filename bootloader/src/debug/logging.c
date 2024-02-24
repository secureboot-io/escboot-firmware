#include "debug/logging.h"
#include "io/uart.h"
#include <stdarg.h>
#include <stdio.h>

void logInit()
{
    uartInit();
}

void logWrite(const char *fmt, ...)
{
    uint8_t buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf((char *)buf, sizeof(buf), fmt, args);
    va_end(args);
}
