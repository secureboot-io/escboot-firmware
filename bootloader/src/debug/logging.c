#include "debug/logging.h"
#include "io/uart.h"
#include <stdarg.h>
#include <stdio.h>

#define ANSI_RESET      "\x1b[0m"
#define ANSI_BLACK      "\x1b[30m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_YELLOW     "\x1b[33m"
#define ANSI_BLUE       "\x1b[34m"
#define ANSI_PURPLE     "\x1b[35m"
#define ANSI_CYAN       "\x1b[36m"
#define ANSI_WHITE      "\x1b[37m"
#define ANSI_TRACE      "\x1b[94m"
#define ANSI_FILE       "\x1b[90m"

const char *logLevelNames[] = {
    ANSI_TRACE  "TRACE" ANSI_RESET,
    ANSI_CYAN   "DEBUG" ANSI_RESET,
    ANSI_GREEN  "INFO " ANSI_RESET,
    ANSI_YELLOW "WARN " ANSI_RESET,
    ANSI_RED    "ERROR" ANSI_RESET,
    ANSI_PURPLE "FATAL" ANSI_RESET
};

void logInit()
{
    uartInit();
}

void logWrite(logLevel_t level, const char *file, int line, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    int len = snprintf(buf, sizeof(buf), "\033[31;1m%s\033[0m " ANSI_FILE "%s:%d:" ANSI_RESET " ", logLevelNames[level], file, line);
    len += vsnprintf(buf + len, sizeof(buf) - len, fmt, args);
    snprintf(buf + len, sizeof(buf) - len, "%s", "\r\n");
    va_end(args);
    const char *p = buf;
    while (*p)
    {
        uartTransmit(*p++);
    }
}
