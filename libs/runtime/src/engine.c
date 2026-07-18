#include "dhemitus/engine.h"
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

b8 engine_create(engine *engine, const game_config *config){

    engine->window_context = &context;
    if(!window_create(engine->window_context, config->width, config->height, config->title)){
        LOG_WARN("window create failed");
        return false;
    }
    return true;
}

void engine_run(engine *engine, void *game_state, frame_data *frame_data){

    while (engine->is_running) {
        engine_next_loop(engine, game_state, frame_data);
        if(engine->is_visible){
            window_swap_buffers(engine->window_context);
        } else {
            SDL_Delay(16);
        }
    }

    window_destroy(engine->window_context);

}

b8 engine_next_loop(engine *engine, void *game_state, frame_data *frame_data){
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
            engine->on_update_callback(engine->window_context, game_state);
        }
        frame_data->accumulator -= frame_data->update_time;
    }

    // here we render
    if(engine->on_render_callback){
        engine->on_render_callback(engine->window_context, game_state, dt);
    }

    frame_data->accumulator += dt;

    return true;
}

