#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_main.h>

#include <dhemitus/input.h>
#include <dhemitus/engine.h>
#include <dhemitus/event.h>
#include <dhemitus/memory.h>
#include "game.h"
#include <preference.h>
#include <entry.h>

b8 engine_init(engine *engine){

   engine->config = allocate_memory(sizeof(game_config), MEMORY_LABEL_APPLICATION);
    if(!engine->config){
        return false;
    }

    game *state = allocate_memory(sizeof(game), MEMORY_LABEL_GAME);
    if(!state){
        return false;
    }

    zero_memory(state, sizeof(game));


    state->render_called = 0;
    state->update_called = 0;
    state->time_passed = 0;

    engine->game_state = state;

    engine->config->title = "dhemitus engine";
    engine->config->width = 1280;
    engine->config->height = 720;
    engine->config->fps = 120;
    engine->on_update_callback = on_update;
    engine->on_render_callback = on_render;

 
//    engine->is_running = true; // gonna move inside
//    engine->is_visible = true; // gonna move inside
//    engine->on_window_callback = on_window;
    return true;
}

b8 engine_clear(engine *engine){
    free_memory(engine->game_state, sizeof(game), MEMORY_LABEL_GAME);
    free_memory(engine->config, sizeof(game_config), MEMORY_LABEL_APPLICATION);
    engine->game_state = 0;
    engine->config = 0;

    return true;
}
