#ifndef H_RENDERER
#define H_RENDERER

#include "renderer_api.h"

int renderer_init(const platform_state_t* const platform_data);
int renderer_draw();
void renderer_wait();
int renderer_destroy();

#endif