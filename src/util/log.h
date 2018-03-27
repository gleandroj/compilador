#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdarg.h>

#ifndef MAX_LOG_LENGHT
#define MAX_LOG_LENGHT 100
#endif

#define LOG_FATAL -1
#define LOG_ERROR 0
#define LOG_WARNING 1
#define LOG_INFO 2
#define LOG_DEBUG 3

typedef int LogLevel;

void _log(char *message, LogLevel level, va_list args);
void log_fatal(char *message, ...);
void log_error(char *message, ...);
void log_warning(char *message, ...);
void log_info(char *message, ...);
void log_debug(char *message, ...);

#endif