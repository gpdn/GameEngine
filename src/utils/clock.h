#ifndef H_CLOCK
#define H_CLOCK

#include "macro_controller.h"
#include <stdio.h>
#include <time.h>

typedef struct application_clock_t {
    float start_time;
    float time_elapsed;
    float time_interval;
} application_clock_t;

void clock_init(application_clock_t* clock);
void clock_update(application_clock_t* clock);
void clock_destroy(application_clock_t* clock);

#endif