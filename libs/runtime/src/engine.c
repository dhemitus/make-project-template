#include "dhemitus/engine.h"
#include "SDL3/SDL_timer.h"
#include "dhemitus/logger.h"
#include "preference.h"
#include "dhemitus/frame_data.h"
#include <SDL3/SDL.h>

window_context context = {
    .window = NULL,
    .renderer = NULL,
    .has_mouse_focus = false,
    .has_input_focus = false       
};

frame_data frame = {
    .accumulator = 0,
};

b8 engine_create(engine *engine){

    engine->window_context = &context;
    if(!window_create(engine->window_context, engine->config->width, engine->config->height, engine->config->title)){
        LOG_WARN("window create failed");
        return false;
    }
    return true;
}

void engine_run(engine *engine){
    frame.current_time = SDL_GetTicksNS();
    frame.update_time = 1000000000 / engine->config->fps; 

    while (engine->is_running) {

        if(!engine_next_loop(engine, &frame)){
            engine->is_running = false;
        }
        if(engine->is_visible){
            window_swap_buffers(engine->window_context);
        } else {
            SDL_Delay(16);
        }
    }

    //free_memory(engine->game_state, false);
    window_destroy(engine->window_context);

}

b8 engine_next_loop(engine *engine, frame_data *frame_data){
    if(!engine) return false;
    
    SDL_Event event;

    frame_data->last_time = frame_data->current_time;
    frame_data->current_time = SDL_GetTicksNS();

    u64 dt = frame_data->current_time - frame_data->last_time;

    const u64 MAX_DT_NS = 100000000;
    if(dt > MAX_DT_NS){
        dt = MAX_DT_NS;
    }

    while (frame_data->accumulator > frame_data->update_time) {
        if(engine->on_event_callback){
            engine->on_event_callback(engine, &event);
        }

        // here we update
        if(engine->on_update_callback){
            engine->on_update_callback(engine->game_state);
        }
        frame_data->accumulator -= frame_data->update_time;
    }

    // here we render
    if(engine->on_render_callback){
        engine->on_render_callback(engine->game_state, dt);
    }

    frame_data->accumulator += dt;

    return true;
}

