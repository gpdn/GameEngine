#include "renderer.h"

int renderer_init(const platform_state_t* const platform_data) {
    return renderer_api_init(platform_data);
}

int renderer_draw() {
    return renderer_api_draw_frame();
}

void renderer_wait() {
    renderer_api_wait();
}

int renderer_destroy() {
    return renderer_api_destroy();
}