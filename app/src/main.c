#include "SDL3/SDL_events.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h> // Required for chdir()

#include <dhemitus/input.h>
#include <dhemitus/loop.h>
#include <dhemitus/core.h>
#include <preference.h>
#include <dhemitus/logger.h>
#include <dhemitus/asserts.h>
#include <dhemitus/window.h>
#include "SDL3/SDL_timer.h"
#include "game.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    printf("[Engine] Initializing sub-modules...\n");
    int result = math_add(5, 10);
    LOG_INFO("[Engine] Test function returned value: %d", result);
    LOG_FATAL("std::vector<const char*> enabledInstanceExtensions; fatal situation: %f", 3.14f);
    LOG_ERROR(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_WARN(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_INFO(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_DEBUG(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_TRACE(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    ASSERTION(1 == 1);

    // Spawn window surface framework 
    window_config config = {
        .title = "dhemitus engine",
        .width = 1024,
        .height = 720,
        .fps = 10
    };

    input_event input = {
        .window_width = config.width,
        .window_height = config.height
    };

    game state = {
        .update_called = 0,
        .render_called = 0,
        .time_passed = 0
    };

    engine_loop engine = {
        .current_time = SDL_GetTicksNS(),
        .accumulator = 0,
        .update_time = (1000 * 1000 * 1000) / config.fps,
        .on_event_callback = window_poll_events,
        .on_update_callback = on_update,
        .on_render_callback = on_render
    };

    window_context context = {
        .window = NULL,
        .renderer = NULL,
        .input_event = input,
        .is_running = true,
        .is_visible = true,
        .has_mouse_focus = false,
        .has_input_focus = false       
    };

    engine.window_context = &context;


    if(!window_create(engine.window_context, &config)){
        LOG_INFO("-----------------maka jadi");
        return -1;
    }

    window_set_input_callback(engine.window_context, on_input);
    window_set_window_callback(engine.window_context, on_window);
    /*window_set_key_callback(engine.window_context, on_key);
    window_set_mouse_button_callback(engine.window_context, on_click);
    window_set_mouse_position_callback(engine.window_context, on_move);
    window_set_mouse_scroll_callback(engine.window_context, on_scroll);

    window_set_gamepad_callback(engine.window_context, on_pad);*/

    //window_set_resize_callback(engine.window_context, on_resize);

    //window_set_minimize_callback(engine.window_context, on_minimize);

    SDL_Event event;

    while (window_should_run(engine.window_context)) {
        //window_poll_events(&context, &event);
        engine_next_loop(&engine, &state, &event);
        if(engine.window_context->is_visible){
            window_swap_buffers(engine.window_context);
        } else {
            SDL_Delay(16);
        }
    }

    window_destroy(engine.window_context);
    return 0;
}
