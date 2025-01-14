#include "utils/clock.h"
#include "platform/platform.h"
#include <memory.h>

void clock_init(application_clock_t* clock) {
    clock->start_time = platform_get_time();
    clock->time_elapsed = 0;
}

void clock_update(application_clock_t* clock) {
    float current_time = platform_get_time() - clock->start_time;
    clock->time_interval = current_time - clock->time_elapsed;
    clock->time_elapsed = current_time; 
}

void clock_destroy(application_clock_t* clock) {
    memset(clock, 0, sizeof(application_clock_t));
}