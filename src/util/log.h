#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

#define LOG_FATAL -1
#define LOG_ERROR 0
#define LOG_WARNING 1
#define LOG_INFO 2

typedef int LogLevel;

void log(const char* message, LogLevel level);
void log_fatal(const char* error);
void log_error(const char* error);
void log_warning(const char* error);
void log_info(const char* error);

#endif