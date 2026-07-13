#pragma once

#include "window.h"
#include "preference.h"

typedef b8 (*window_event_callback)(window_context*, void *event);
typedef b8 (*render_callback)(window_context*, void *game_state, u64 dt);
typedef b8 (*update_callback)(window_context*, void *game_state);

typedef struct engine_loop {
    u64 last_time;
    u64 current_time;
    u64 accumulator;
    u64 update_time;

    void *game_state;
    window_context *window_context;

    window_event_callback on_event_callback;
    render_callback on_render_callback;
    update_callback on_update_callback;
} engine_loop;

DHEMITUS_API b8 engine_next_loop(engine_loop *engine, void *game_state, void *event);
