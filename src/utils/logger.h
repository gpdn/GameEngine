#ifndef H_LOGGER
#define H_LOGGER

#include <stdio.h>
#include "macro_controller.h"

#define COLOR_WHITE "\e[0;37m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GRAY "\x1b[30m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define LOG_DEBUG_PREFIX "[Debug]"
#define LOG_INFO_PREFIX "[Info]"
#define LOG_WARNING_PREFIX "[Warning]"
#define LOG_ERROR_PREFIX "[Error]"
#define LOG_TODO_PREFIX "[Todo]"

#define LOG(...) printf(__VA_ARGS__)
#define SET_COLOR(COLOR) printf("%s", COLOR)
#define RESET_COLOR() printf("%s", COLOR_RESET)
#define LOG_COLOR(COLOR, TAG, ...) SET_COLOR(COLOR); LOG(__VA_ARGS__); RESET_COLOR()
#define LOG_COLOR_TAG(COLOR, TAG, ...) SET_COLOR(COLOR); LOG("%s", TAG); LOG(__VA_ARGS__); RESET_COLOR()

typedef enum log_level {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} log_level;

#if LOG_TODO_ENABLED
    #define LOG_TODO(NAME) LOG_COLOR_TAG(COLOR_CYAN, LOG_DEBUG_PREFIX, "Implement %s in file: %s, line: %d", #NAME, (__FILE__), __LINE__)
#else
    #define LOG_TODO(...)
#endif

#if LOG_DEBUG_ENABLED == 1
    #define LOG_DEBUG(...) LOG_COLOR_TAG(COLOR_CYAN, LOG_DEBUG_PREFIX, __VA_ARGS__)
#else
    #define LOG_DEBUG(...)
#endif

#if LOG_INFO_ENABLED == 1
    #define LOG_INFO(...) LOG(LOG_INFO_PREFIX, __VA_ARGS__)
#else
    #define LOG_INFO(...)
#endif

#if LOG_WARNING_ENABLED == 1
    #define LOG_WARNING(...) LOG_COLOR_TAG(COLOR_YELLOW, LOG_WARNING_PREFIX, __VA_ARGS__)
#else
    #define LOG_WARNING(...)
#endif

#if LOG_ERROR_ENABLED == 1
    #define LOG_ERROR(...) LOG_COLOR_TAG(COLOR_RED, LOG_ERROR_PREFIX, __VA_ARGS__)
#else
    #define LOG_ERROR(...)
#endif


#endif