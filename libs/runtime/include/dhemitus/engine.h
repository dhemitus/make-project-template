#pragma once

#include "dhemitus/window.h"
#include "preference.h"
#include "dhemitus/frame_data.h"
#include "dhemitus/input.h"

typedef struct engine engine;

typedef void (*input_callback_func)(engine *engine);
typedef void (*window_callback_func)(engine *engine);

typedef b8 (*window_event_callback)(engine*, void *event);
typedef b8 (*render_callback)(void *game_state, u64 dt);
typedef b8 (*update_callback)(void *game_state);

typedef struct game_config game_config;

struct engine {
    void *game_state;
    window_context *window_context;

    b8 is_running;
    b8 is_visible;

    input_event *input_event;
    game_config *config;

    input_callback_func on_input_callback;
    window_callback_func on_window_callback;

    window_event_callback on_event_callback;
    render_callback on_render_callback;
    update_callback on_update_callback;
};

struct game_config {
    const char *title;
    i16 width;
    i16 height;
    i16 fps;
};

DHEMITUS_API b8 engine_create(engine *engine);
DHEMITUS_API void engine_run(engine *engine);

b8 engine_next_loop(engine *engine, frame_data *frame_data);

