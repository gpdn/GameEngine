#ifndef H_EVENT
#define H_EVENT

#include <stdlib.h>
#include "utils/logger.h"

typedef enum {
    E_APPLICATION_EXIT,
    E_KEY_PRESS,
    E_KEY_RELEASE,
    E_MOUSE_PRESS,
    E_MOUSE_RELEASE,
    E_MOUSE_MOVE,
    E_MOUSE_WHEEL_MOVE,
    E_RESISE,
    E_ACTIVE,

    E_MAX_VALUE
} event_type_t;

typedef union {
    int i1;
    float f1;

    int i2[2];
    float f2[2];
    
    int i4[4];
    float f4[4];

    char c[16];
} event_data_t;

typedef int (*event_fn)(event_type_t type, void* sender, void* listener, event_data_t data);

int event_system_init();
int event_register(void* listener, event_type_t type, event_fn callback);
int event_fire(void* sender, event_type_t type, event_data_t data);
int event_unregister(void* listener, event_type_t type, event_fn callback);
int event_system_destroy();

#endif