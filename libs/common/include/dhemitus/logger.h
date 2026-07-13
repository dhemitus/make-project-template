#pragma once

#include "preference.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1

#if RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#else
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1
#endif

typedef enum log_type {
    LOG_TYPE_FATAL = 0,
    LOG_TYPE_ERROR = 1,
    LOG_TYPE_WARN = 2,
    LOG_TYPE_INFO = 3,
    LOG_TYPE_DEBUG = 4,
    LOG_TYPE_TRACE = 5
} log_type;

b8 init_log(void);
void close_log(void);

DHEMITUS_API void log_output(log_type type, const char *message, ...);

#define LOG_FATAL(message, ...) log_output(LOG_TYPE_FATAL, message, ##__VA_ARGS__);

#ifndef LOG_ERROR 
#define LOG_ERROR(message, ...) log_output(LOG_TYPE_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define LOG_WARN(message, ...) log_output(LOG_TYPE_WARN, message, ##__VA_ARGS__);
#else
#define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define LOG_INFO(message, ...) log_output(LOG_TYPE_INFO, message, ##__VA_ARGS__);
#else
#define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define LOG_DEBUG(message, ...) log_output(LOG_TYPE_DEBUG, message, ##__VA_ARGS__);
#else
#define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define LOG_TRACE(message, ...) log_output(LOG_TYPE_TRACE, message, ##__VA_ARGS__);
#else
#define LOG_TRACE(message, ...)
#endif
