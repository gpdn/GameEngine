#include <memory.h>
#include "input.h"
#include "event.h"

#if LOG_DEBUG_INPUT_ENABLED == 1
    #define LOG_DEBUG_INPUT(...) LOG_DEBUG(__VA_ARGS__) 
#else
    #define LOG_DEBUG_INPUT(...) 
#endif

typedef struct input_state_t {
    int keys_state[KEY_MAX_VALUE];
    int mouse_state[MOUSE_BUTTON_MAX];
    int mouse_x;
    int mouse_y;
} input_state_t;

static input_state_t input_state;
static int initialised = 0;

void input_init() {
    if(initialised == 1) return;
    
    memset(&input_state, 0, sizeof(input_state_t));
    initialised = 1;
}

void input_update() {
    
}

void input_update_key(keycodes_t key, int pressed) {
    if(input_state.keys_state[key] == pressed) return;

    LOG_DEBUG_INPUT("Key %s: %d, %d\n", pressed ? "Pressed" : "Released", key, pressed);

    input_state.keys_state[key] = pressed;
    event_data_t data = {.i1 = key};
    event_fire(NULL, pressed ? E_KEY_PRESS : E_KEY_RELEASE, data);
}

void input_update_button(mouse_buttons_t button, int pressed) {
    if(input_state.keys_state[button] == pressed) return;

    input_state.mouse_state[button] = pressed;
    event_data_t data = {.i1 = button};
    event_fire(NULL, pressed ? E_MOUSE_PRESS : E_MOUSE_RELEASE, data);
}

void input_update_mouse_position(int x, int y) {
    if(input_state.mouse_x == x && input_state.mouse_y == y) return;

    input_state.mouse_x = x;
    input_state.mouse_y = y;

    LOG_DEBUG_INPUT("Mouse Position: %d, %d\n", x, y);

    event_data_t data = {.i2 = {x, y}};
    event_fire(NULL, E_MOUSE_MOVE, data);
}

void input_update_mouse_wheel(int z) {
    event_fire(NULL, E_MOUSE_WHEEL_MOVE, (event_data_t){.i1 = z});
}

inline int input_is_key_up(keycodes_t key) {
    return input_state.keys_state[key] == 0;
}

inline int input_is_key_down(keycodes_t key) {
    return input_state.keys_state[key] == 1;
}

inline int input_is_button_up(mouse_buttons_t button) {
    return input_state.mouse_state[button] == 0;
}

inline int input_is_button_down(mouse_buttons_t button) {
    return input_state.mouse_state[button] == 1;
}

inline void input_get_mouse_position(int* x, int* y) {
    *x = input_state.mouse_x;
    *y = input_state.mouse_y;
}

void input_destroy() {
    initialised = 0;
}