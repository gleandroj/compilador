#include <stdio.h>
#include "typings.h"
#include "log.h"

void log(const char *message, LogLevel level)
{
    char levelStr[8];
    Booleano _abort = FALSE;

    switch (level)
    {
        case LOG_FATAL:
            levelStr = "FATAL";
            _abort = TRUE;
            break;
        case LOG_ERROR:
            levelStr = "ERROR";
            _abort = TRUE;
            break;
        case LOG_WARNING:
            levelStr = "WARNING";
            break;
        case LOG_INFO:
            levelStr = "INFO";
            break;
    }

    printf("[%s] - %s", message, levelStr);
    if(_abort){

    }
}

void log_fatal(const char *error)
{
    log(error, LOG_FATAL);
}

void log_error(const char *error)
{
    log(error, LOG_ERROR);
}

void log_warning(const char *error)
{
    log(error, LOG_WARNING);
}

void log_info(const char *error)
{
    log(error, LOG_INFO);
}