#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_main.h>

#include <dhemitus/input.h>
#include <dhemitus/engine.h>
#include <dhemitus/event.h>
#include <dhemitus/platform.h>
#include "game.h"
#include <preference.h>
#include <entry.h>

b8 engine_init(engine *engine){

    game_config config = {
        .title = "dhemitus engine",
        .width = 1280,
        .height = 720,
        .fps = 60
    };

    input_event input = {
        .window_width = config.width,
        .window_height = config.height
    };

    engine->on_event_callback = window_poll_events;
    engine->on_update_callback = on_update;
    engine->on_render_callback = on_render;
    engine->input_event = &input;
    engine->config = &config;
    engine->is_running = true;
    engine->is_visible = true;
    engine->on_input_callback = on_input;
    engine->on_window_callback = on_window;
    engine->game_state = allocate_memory(sizeof(game), false);
    game state = {
        .update_called = 0,
        .render_called = 0,
        .time_passed = 0
    };

    engine->game_state = &state;

    return true;
}

