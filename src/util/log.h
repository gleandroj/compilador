#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdarg.h>

#ifndef MAX_LOG_LENGHT
#define MAX_LOG_LENGHT 100
#endif

typedef enum {
    LOG_FATAL = -1,
    LOG_ERROR = 0,
    LOG_WARNING = 1,
    LOG_INFO = 2,
    LOG_DEBUG = 3,
} LogLevel;

void _log(char *message, LogLevel level, va_list args);
void log_fatal(char *message, ...);
void log_error(char *message, ...);
void log_warning(char *message, ...);
void log_info(char *message, ...);
void log_debug(char *message, ...);

#endif