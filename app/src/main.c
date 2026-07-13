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

void on_key(window_context *context) {
    LOG_INFO("appppp-------------- %d : %d", context->input_event.key_code, context->input_event.key_action);
}

void on_resize(window_context *context){
    LOG_INFO("[System] Window resized to: %dx%d\n", context->input_event.window_width, context->input_event.window_height);
}

void on_click(window_context *context){
    (void)context;
    //LOG_INFO("[Mouse] Click: Button type: %d action: %d  at X:%.1f, Y:%.1f (Count: %d)", context->input_event.type, context->input_event.mouse_action, context->input_event.mouse_x, context->input_event.mouse_y, context->input_event.mouse_clicks);

}

void on_move(window_context *context){
    LOG_INFO("[Mouse] move callback: Button:  at X:%.1f, Y:%.1f ", context->input_event.mouse_x, context->input_event.mouse_y);
}

void on_scroll(window_context *context){
    LOG_INFO("[Mouse] scroll: Button:  at X:%.1f, Y:%.1f ", context->input_event.scroll_x, context->input_event.scroll_y);
}

void on_minimize(window_context *context){

    if(context->is_visible){
        LOG_INFO("window maximize-------------------------------");
    } else {
        LOG_INFO("window minimize-------------------------------");
    }
}

void on_pad(window_context *context, int jid, b8 connected){
    (void)context;
    LOG_INFO("[Gamepad] id %d connect: %d", jid, connected);
}

typedef struct game {
    int update_called;
    int render_called;
    u64 time_passed;
} game;

b8 on_update(window_context *context, void *game_state){
    game *state = (game *)game_state;
    (void)context;
    /*if(context->input_event.mouse_action == BUTTON_ACTION_PRESS){
        LOG_INFO("[Mouse] Click: Button action: %d  at X:%.1f, Y:%.1f (Count: %d)", context->input_event.mouse_action, context->input_event.mouse_x, context->input_event.mouse_y, context->input_event.mouse_clicks);
        LOG_INFO("[Mouse] move callback: Button:  at X:%.1f, Y:%.1f ", context->input_event.mouse_x, context->input_event.mouse_y);
    }*/
    if(context->input_event.type == INPUT_EVENT_TYPE_MOUSE_DOWN){
        LOG_INFO("[Mouse] Click: Button type: %d action: %d  at X:%.1f, Y:%.1f (Count: %d)", context->input_event.type, context->input_event.mouse_action, context->input_event.mouse_x, context->input_event.mouse_y, context->input_event.mouse_clicks);
    }

    state->update_called += 1;
    return true;
}

b8 on_render(window_context *context, void *game_state, u64 dt){
    game *state = (game *)game_state;
    state->render_called += 1;
    state->time_passed += dt;
    (void)context;
    if(state->time_passed > 1000 * 1000 * 1000){
//        LOG_INFO("update new fps: %.2f", (double)state->update_called);
//        LOG_INFO("render new fps: %.2f", (double)state->render_called);

        state->update_called = 0;
        state->render_called = 0;
        state->time_passed = 0;
    }

    return true;

}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    printf("[Engine] Initializing sub-modules...\n");
    int result = math_add(5, 10);
    LOG_INFO("[Engine] Test function returned value: %d\n", result);
    /*LOG_FATAL("std::vector<const char*> enabledInstanceExtensions; fatal situation: %f", 3.14f);
    LOG_ERROR(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_WARN(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_INFO(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    ASSERTION(1 == 1);*/

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

    window_context context = {
        .window = NULL,
        .renderer = NULL,
        .input_event = input,
        .is_running = true,
        .is_visible = true,
        .has_mouse_focus = false,
        .has_input_focus = false       
    };

    game state = {
        .update_called = 0,
        .render_called = 0,
        .time_passed = 0
    };

    b8 cek = window_create(&context, &config);

    window_set_key_callback(&context, on_key);
    window_set_mouse_button_callback(&context, on_click);
    window_set_mouse_position_callback(&context, on_move);
    window_set_mouse_scroll_callback(&context, on_scroll);

    window_set_gamepad_callback(&context, on_pad);

    window_set_resize_callback(&context, on_resize);
    window_set_minimize_callback(&context, on_minimize);

    if(cek){
        LOG_INFO("-----------------maka jadi");
    }

    engine_loop engine = {
        .window_context = &context,
        .current_time = SDL_GetTicksNS(),
        .accumulator = 0,
        .update_time = (1000 * 1000 * 1000) / config.fps,
        .on_event_callback = window_poll_events,
        .on_update_callback = on_update,
        .on_render_callback = on_render
    };

    SDL_Event event;

    while (window_should_run(&context)) {
        //window_poll_events(&context, &event);
        engine_next_loop(&engine, &state, &event);
        window_swap_buffers(&context);
    }

    window_destroy(&context);
    return 0;
}
