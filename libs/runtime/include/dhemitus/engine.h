#pragma once

#include "dhemitus/window.h"
#include "preference.h"
#include "dhemitus/frame_data.h"
#include "dhemitus/input.h"

typedef struct engine engine;

typedef void (*input_callback_func)(engine *engine);
typedef void (*window_callback_func)(engine *engine);

typedef b8 (*window_event_callback)(engine*, void *event);
typedef b8 (*render_callback)(window_context*, void *game_state, u64 dt);
typedef b8 (*update_callback)(window_context*, void *game_state);

struct engine {
    void *game_state;
    window_context *window_context;

    b8 is_running;
    b8 is_visible;

    input_event *input_event;

    input_callback_func on_input_callback;
    window_callback_func on_window_callback;

    window_event_callback on_event_callback;
    render_callback on_render_callback;
    update_callback on_update_callback;
};

typedef struct game_config {
    const char *title;
    i16 width;
    i16 height;
    i16 fps;
} game_config;

DHEMITUS_API b8 engine_create(engine *engine, const game_config *config);
DHEMITUS_API void engine_run(engine *engine, void *game_state, frame_data *frame_data);

b8 engine_next_loop(engine *engine, void *game_state, frame_data *frame_data);

