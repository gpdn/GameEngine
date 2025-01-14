#include "application.h"
#include "core/event.h"
#include "core/input.h"
#include "renderer/renderer.h"
#include "utils/utils.h"

typedef struct application_data_t {
    platform_state_t* platform;
    int running;
    int paused;
    int active;
    application_clock_t clock;
    float frame_rate_limit;
} application_data_t;

typedef struct application_config_t {
    int initial_width;
    int initial_height;
    int x;
    int y;
    const char* name;
} application_config_t;

static application_data_t app_data;
static int initialised = 0;

int application_on_window_close(event_type_t type, void *sender, void *listener, event_data_t data);
int application_on_window_active(event_type_t type, void *sender, void *listener, event_data_t data);
int application_on_key_release(event_type_t type, void *sender, void *listener, event_data_t data);

int application_init() {

    if(initialised) {
        LOG_ERROR("Application already initialised.\n");
        return 0;
    }
    
    event_system_init();
    input_init();

    platform_state_t* platform = platform_init("Example", 100, 100, 1000, 1000);

    if(platform == NULL) return 0;

    event_register(NULL, E_APPLICATION_EXIT, application_on_window_close);
    event_register(NULL, E_ACTIVE, application_on_window_active);
    event_register(NULL, E_KEY_RELEASE, application_on_key_release);

    renderer_init((const platform_state_t* const)platform);

    app_data.platform = platform;

    initialised = 1;

    app_data.running = 1;
    app_data.paused = 0;
    app_data.active = 1;
    app_data.frame_rate_limit = 1.0f / 60;

    clock_init(&app_data.clock);

    return 1;
}

int application_run() {

    app_data.paused = 1; 

    while(app_data.running) {

        clock_update(&app_data.clock);

        //LOG_DEBUG("%f\n", app_data.clock.time_interval);

        platform_get_events(app_data.platform);

        if(app_data.paused || app_data.active == 0) continue;

        renderer_draw();

        input_update();
    }
    
    //platform_wait(5000);

    LOG_DEBUG("Closing Application\n");

    return 1;
}

int application_on_window_close(event_type_t type, void *sender, void *listener, event_data_t data) {
    app_data.running = 0;
    return 1;
}

int application_on_window_active(event_type_t type, void *sender, void *listener, event_data_t data) {
    app_data.active = data.i1;
    if(data.i1 == 0 && app_data.running == 1) renderer_wait();
    return 1;
}

int application_on_key_release(event_type_t type, void *sender, void *listener, event_data_t data) {
    switch(data.i1) {
        case KEY_ESCAPE:
            app_data.running = 0;
            break;
        case KEY_P:
            app_data.paused = !app_data.paused;
            break;
        default: 
            break;
    }
    return 1;
}

int application_destroy() {
    renderer_destroy();
    event_system_destroy();
    input_destroy();
    if(app_data.platform) return platform_destroy(app_data.platform);

    return 1; 
}