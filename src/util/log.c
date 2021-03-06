#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "typings.h"
#include "memory.h"
#include "log.h"

void _log(char *message, LogLevel level, va_list args)
{
    char *buffer = (char *)malloc(MAX_LOG_LENGHT * sizeof(char));

    switch (level)
    {
        case LOG_FATAL:
            sprintf(buffer, "[%s] - %s", "FATAL", message);
            break;
        case LOG_ERROR:
            sprintf(buffer, "[%s] - %s", "ERROR", message);
            break;
        case LOG_WARNING:
            sprintf(buffer, "[%s] - %s", "WARNING", message);
            break;
        case LOG_INFO:
            sprintf(buffer, "[%s] - %s", "INFO", message);
            break;
        case LOG_DEBUG:
            sprintf(buffer, "[%s] - %s", "DEBUG", message);
            break;
    }

    vprintf(buffer, args);
}

void log_fatal(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_FATAL, args);
    va_end(args);
}

void log_error(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_ERROR, args);
    va_end(args);
    exit(0);
}

void log_warning(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_WARNING, args);
    va_end(args);
}

void log_info(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_INFO, args);
    va_end(args);
}

#ifdef DEBUG_MODE
void log_debug(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_DEBUG, args);
    va_end(args);
}
#else
void log_debug(char *message, ...){}
#endif