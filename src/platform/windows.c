#include "platform.h"
#include "utils/logger.h"
#include "core/event.h"
#include "core/input.h"

#if PLATFORM_WINDOWS

    static LRESULT CALLBACK handle_window_event(HWND hwnd, unsigned int msg, WPARAM w_param, LPARAM l_param);
    static inline void handle_resize();
    static inline void handle_key_press(WPARAM wparam);
    static inline void handle_key_release(WPARAM wparam);
    static inline void handle_mouse_move(LPARAM l_param);
    static inline void handle_mouse_wheel(WPARAM w_param);
    static inline void handle_mouse_button_press(mouse_buttons_t button);
    static inline void handle_mouse_button_release(mouse_buttons_t button);

    
    static inline void handle_resize(WPARAM w_param) {
        if(w_param == SIZE_MINIMIZED) event_fire(NULL, E_ACTIVE, (event_data_t){.i1 = 0});
    }
    
    static inline void handle_activate(WPARAM w_param) {
        switch(w_param) {
            case WA_ACTIVE:
            case WA_CLICKACTIVE:
                event_fire(NULL, E_ACTIVE, (event_data_t){.i1 = 1});
                break;
            case WA_INACTIVE:
                event_fire(NULL, E_ACTIVE, (event_data_t){.i1 = 0});
                break;
            default:
                break;
        }
    }

    static inline void handle_key_press(WPARAM wparam) {
        input_update_key((int)wparam, 1);
    }
    
    static inline void handle_key_release(WPARAM w_param) {
        input_update_key((int)w_param, 0);
    }

    static inline void handle_mouse_move(LPARAM l_param) {
        input_update_mouse_position(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
    }

    static inline void handle_mouse_wheel(WPARAM w_param) {
        input_update_mouse_wheel(GET_WHEEL_DELTA_WPARAM(w_param) > 0 ? 1 : -1);
    }

    static inline void handle_mouse_button_press(mouse_buttons_t button) {
        input_update_button(button, 1);
    }

    static inline void handle_mouse_button_release(mouse_buttons_t button) {
        input_update_button(button, 0);
    }
    

    static double clock_frequency; 
    static LARGE_INTEGER start_time;

    platform_state_t* platform_init(const char* name, int x, int y, int width, int height) {
        platform_window_state_t* win_state = (platform_window_state_t*)malloc(sizeof(platform_window_state_t));

        win_state->instance = GetModuleHandleA(0);

        HICON icon = LoadIcon(win_state->instance, IDI_APPLICATION);
        WNDCLASSA window;
        memset(&window, 0, sizeof(window));
        window.style = CS_DBLCLKS;
        window.lpfnWndProc = handle_window_event;
        window.cbClsExtra = 0;
        window.cbWndExtra = 0;
        window.hInstance = win_state->instance;
        window.hIcon = icon;
        window.hCursor = LoadCursor(NULL, IDC_ARROW);
        window.hbrBackground = NULL;
        window.lpszClassName = "EngineWindow";

        if(!RegisterClassA(&window)) {
            MessageBoxA(0, "Window Registration Failed", "Error", MB_ICONEXCLAMATION | MB_OK);
            return NULL;
        }

        unsigned int window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
        unsigned int window_ext_style = WS_EX_APPWINDOW;

        RECT window_rect = {0, 0, 0, 0};
        AdjustWindowRectEx(&window_rect, window_style, 0, window_ext_style);

        HWND window_handle = CreateWindowExA(window_ext_style, "EngineWindow", name, window_style, x, y, width, height, 0, 0, win_state->instance, 0);

        if(!window_handle) {
            MessageBoxA(0, "Failed to create window", "Error", MB_ICONEXCLAMATION | MB_OK);
        
            LOG_ERROR("Failed to create window.\n");
            return NULL;
        }

        win_state->window = window_handle;

        ShowWindow(win_state->window, SW_SHOW);

        if(!win_state->window) {
            MessageBoxA(0, "Failed to open window", "Error", MB_ICONEXCLAMATION | MB_OK);
        
            LOG_ERROR("Failed to open window.\n");
            return NULL;
        }

        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        clock_frequency = 1.0f / (double)frequency.QuadPart;
        QueryPerformanceCounter(&start_time);

        return win_state;

    }

    int platform_destroy(platform_state_t* platform_state) {
        platform_window_state_t* win_state = (platform_window_state_t*)platform_state;
        if(win_state->window) {
            DestroyWindow(win_state->window);
            win_state->window = NULL;

            return 1;
        }

        LOG_ERROR("Failed to destroy window.\n");

        return 0;
    }

    void platform_get_events(platform_state_t* platform_state) {
        MSG msg;
        while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    double platform_get_time() {
        LARGE_INTEGER current;
        QueryPerformanceCounter(&current);
        return (double)current.QuadPart * clock_frequency;
    }

    void platform_wait(int milliseconds) {
        Sleep(milliseconds);
    }

    LRESULT CALLBACK handle_window_event(HWND window, unsigned int msg, WPARAM w_param, LPARAM l_param) {
        switch(msg) {
            case WM_ERASEBKGND:
                return 1;
            case WM_CLOSE:
                event_fire(NULL, E_APPLICATION_EXIT, (event_data_t){.i1 = 0});
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_SIZE:
                handle_resize(w_param);
                return 0;
            case WM_ACTIVATE:
                handle_activate(w_param);
                return 0;
            case WM_KEYUP:
            case WM_SYSKEYUP:
                handle_key_release(w_param);
                break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                handle_key_press(w_param);
                break;
            case WM_MOUSEMOVE:
                handle_mouse_move(l_param);
                break;
            case WM_MOUSEWHEEL:
                handle_mouse_wheel(w_param);
                break;
            case WM_LBUTTONUP:
                handle_mouse_button_release(MOUSE_BUTTON_LEFT);
                break;
            case WM_MBUTTONUP:
                handle_mouse_button_release(MOUSE_BUTTON_MIDDLE);
                break;
            case WM_RBUTTONUP:
                handle_mouse_button_release(MOUSE_BUTTON_RIGHT);
                break;
            case WM_LBUTTONDOWN:
                handle_mouse_button_press(MOUSE_BUTTON_LEFT);
                break;
            case WM_MBUTTONDOWN:
                handle_mouse_button_press(MOUSE_BUTTON_MIDDLE);
                break;
            case WM_RBUTTONDOWN:
                handle_mouse_button_press(MOUSE_BUTTON_RIGHT);
                break;
            default:
                break;
        }

        return DefWindowProcA(window, msg, w_param, l_param);

    }

#endif