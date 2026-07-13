#pragma once

#include "dhemitus/input.h"
#include "preference.h"

typedef struct window_context window_context;

typedef void (*key_callback_func)(window_context *context);
typedef void (*mouse_button_callback_func)(window_context *context);
typedef void (*mouse_position_callback_func)(window_context *context);
typedef void (*mouse_scroll_callback_func)(window_context *context);

typedef void (*resize_callback_func)(window_context *context);
typedef void (*minimize_callback_func)(window_context *context);

typedef void (*gamepad_callback_func)(window_context *context, int jid, b8 connected);
typedef void (*gamepad_button_callback_func)(window_context *context, int jid, int button, int action);
typedef void (*gamepad_axis_callback_func)(window_context *context, int jid, int axis, float value);

typedef void (*gesture_pinch_callback_func)(window_context *context, float scale_factor);
typedef void (*gesture_pan_callback_func)(window_context *context, float dx, float dy);

struct window_context {
    void *window;
    void *renderer;
    input_event input_event;
    b8 is_running;
    b8 is_visible;
    b8 has_mouse_focus;
    b8 has_input_focus;
    key_callback_func on_key_callback;
    mouse_button_callback_func on_mouse_click_callback;
    mouse_position_callback_func on_mouse_move_callback;
    mouse_scroll_callback_func on_mouse_scroll_callback;

    resize_callback_func on_resize_callback;
    minimize_callback_func on_minimize_callback;
    gamepad_callback_func on_gamepad_callback;
    gamepad_button_callback_func on_gamepad_button_callback;
    gamepad_axis_callback_func on_gamepad_axis_callback;
    gesture_pinch_callback_func on_gesture_pinch_callback;
    gesture_pan_callback_func on_gesture_pan_callback;
};

typedef struct window_config {
    const char *title;
    i32 width;
    i32 height;
    i32 fps;
} window_config;

DHEMITUS_API b8 window_create(window_context *context, const window_config *config);
DHEMITUS_API void window_destroy(window_context *context);
DHEMITUS_API b8 window_should_run(window_context *context);
DHEMITUS_API b8 window_poll_events(window_context *context, void *event);
DHEMITUS_API void window_swap_buffers(window_context *context);

DHEMITUS_API void window_set_key_callback(window_context *context, key_callback_func callback);
DHEMITUS_API void window_set_mouse_button_callback(window_context *context, mouse_button_callback_func callback);
DHEMITUS_API void window_set_mouse_position_callback(window_context *context, mouse_position_callback_func callback);
DHEMITUS_API void window_set_mouse_scroll_callback(window_context *context, mouse_scroll_callback_func callback);

DHEMITUS_API void window_set_resize_callback(window_context *context, resize_callback_func callback);
DHEMITUS_API void window_set_minimize_callback(window_context *context, minimize_callback_func callback);

DHEMITUS_API void window_set_gamepad_callback(window_context *context, gamepad_callback_func callback);
DHEMITUS_API void window_set_gamepad_button_callback(window_context *context, gamepad_button_callback_func callback);
DHEMITUS_API void window_set_gamepad_axis_callback(window_context *context, gamepad_axis_callback_func callback);
DHEMITUS_API void window_set_gesture_pinch_callback(window_context *context, gesture_pinch_callback_func callback);
DHEMITUS_API void window_set_gesture_pan_callback(window_context *context, gesture_pan_callback_func callback);
