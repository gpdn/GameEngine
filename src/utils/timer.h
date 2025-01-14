#ifndef H_TIMER
#define H_TIMER

#include "macro_controller.h"
#include <stdio.h>
#include <time.h>
#include "logger.h"

clock_t timer_start(const char* name);
clock_t timer_start_time(const char* name);
float timer_stop(clock_t timer_start);
float timer_stop_log(const char* name, clock_t timer_start);

#if TIMER_ENABLED == 1
    #define TIMER_START(TIMER_NAME, NAME) clock_t TIMER_NAME = timer_start(name)
    #define TIMER_END(NAME, TIMER) timer_stop_log(const char* name, clock_t TIMER)
#endif

#endif