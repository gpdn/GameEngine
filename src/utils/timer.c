#include "timer.h"

clock_t timer_start(const char* name) {
    LOG_COLOR(COLOR_GREEN, "%s started\n", name);
    return clock();
}

clock_t timer_start_time(const char* name) {
    time_t current_time;
    struct tm* time_info; 

    char time_string[50];

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(time_string, 50, "%a %d %B %Y %H:%M:%S", time_info);

    LOG_COLOR(COLOR_GREEN, "%s started at %s\n", name, time_string);

    return clock();
}

float timer_stop(clock_t timer_start) {
    return (clock() - timer_start)/((float)CLOCKS_PER_SEC);
}

float timer_stop_log(const char* name, clock_t timer_start) {
    float timer_end = timer_stop(timer_start);
    LOG_COLOR(COLOR_GREEN, "%s finished in %1.4fs\n", name, timer_end);
    return timer_end;
}