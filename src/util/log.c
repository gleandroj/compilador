#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "memory.h"
#include "typings.h"
#include "log.h"

void _log(char *message, LogLevel level, va_list args)
{
    if(!can_allocate_memory(MAX_LOG_LENGHT * sizeof(char))){
        printf("[ERROR] - Cannot allocate memory.\n");
        exit(-1);
    }
    
    char *buffer = (char *)malloc(MAX_LOG_LENGHT * sizeof(char));
    Booleano _abort = FALSE;

    switch (level)
    {
        case LOG_FATAL:
            sprintf(buffer, "[%s] - %s", "FATAL", message);
            _abort = TRUE;
            break;
        case LOG_ERROR:
            sprintf(buffer, "[%s] - %s", "ERROR", message);
            _abort = TRUE;
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
    free(buffer);

    if (_abort)
        exit(-1);
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

void log_debug(char *message, ...)
{
    va_list args;
    va_start(args, message);
    _log(message, LOG_DEBUG, args);
    va_end(args);
}