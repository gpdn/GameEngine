#ifndef H_PLATFORM
#define H_PLATFORM

#if _WIN32 | _WIN64
    #define PLATFORM_WINDOWS 1
    #define PLATFORM_IDENTIFIED 1

    #include <windows.h>
    #include <windowsx.h>

    typedef struct platform_window_state_t {
        HINSTANCE instance;
        HWND window;
    } platform_window_state_t;

    typedef platform_window_state_t platform_state_t;

#else
    #define PLATFORM_WINDOWS 0
#endif

#if PLATFORM_IDENTIFIED == 0
    typedef void platform_state_t;
#endif

platform_state_t* platform_init(const char* name, int x, int y, int width, int height);
int platform_destroy(platform_state_t* platform_state);
void platform_get_events(platform_state_t* platform_state);
double platform_get_time();
void platform_wait(int milliseconds);

#endif