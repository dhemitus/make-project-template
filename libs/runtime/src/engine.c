#include "dhemitus/engine.h"
#include "SDL3/SDL_timer.h"
#include "dhemitus/logger.h"
#include "preference.h"
#include "dhemitus/frame_data.h"
#include <SDL3/SDL.h>
#include "dhemitus/memory.h"
#include "dhemitus/event.h"
#include "dhemitus/input.h"

window_context context = {
    .window = NULL,
    .has_mouse_focus = false,
    .has_input_focus = false       
};

frame_data frame = {
    .accumulator = 0,
};

typedef struct engine_state {
    b8 is_running;
    b8 is_visible;
} engine_state;

static b8 initialized = false;

static engine_state state;

b8 engine_on_event(u16 code, void *sender, void *listener, event_context context);
b8 engine_on_key(u16 code, void *sender, void *listener, event_context context);

b8 engine_create(engine *engine){
    if(initialized){
        LOG_ERROR("create game failed");
        return false;
    }

    init_log();
    input_init();

    if(!event_init()){
        LOG_ERROR("initialize event failed");
        return false;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, 0, engine_on_event);
    event_register(EVENT_CODE_WINDOW_MINIMIZED, 0, engine_on_event);
    event_register(EVENT_CODE_WINDOW_MAXIMIZED, 0, engine_on_event);
    event_register(EVENT_CODE_WINDOW_RESIZED, 0, engine_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, engine_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, engine_on_key);

    engine->window_context = &context;

    if(!window_create(engine->window_context, engine->config->width, engine->config->height, engine->config->title)){
        LOG_WARN("window create failed");
        return false;
    }
    state.is_running = true;
    state.is_visible = true;

#ifdef DEBUG
    LOG_DEBUG("%s %i %i %s %i\n", "all mode ------------------------", engine->config->width, engine->config->height, engine->config->title, engine->config->fps);
#endif

    return true;
}

void engine_run(engine *engine){
    frame.current_time = SDL_GetTicksNS();
    frame.update_time = 1000000000 / engine->config->fps; 
    LOG_INFO(get_memory_to_string());

    while (state.is_running) {
        if(!engine_next_loop(engine, &frame)){
            state.is_running = false;
        }

    }

    state.is_running = false;
    state.is_visible = false;

    input_destroy();
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, engine_on_event);
    event_unregister(EVENT_CODE_WINDOW_MINIMIZED, 0, engine_on_event);
    event_unregister(EVENT_CODE_WINDOW_MAXIMIZED, 0, engine_on_event);
    event_unregister(EVENT_CODE_WINDOW_RESIZED, 0, engine_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, engine_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, engine_on_key);


    event_destroy();
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
        window_poll_events(engine, &event);

//        if(state.is_visible){
        // here we update
            if(engine->on_update_callback){
                engine->on_update_callback(engine->game_state);
            }

            input_update(dt);
//        }
        frame_data->accumulator -= frame_data->update_time;
    }

    // here we render
    if(engine->on_render_callback){
        engine->on_render_callback(engine->game_state, dt);
    }

    frame_data->accumulator += dt;

    return true;
}

b8 engine_on_event(u16 code, void *sender, void *listener, event_context context){
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT:
            LOG_INFO("can close application from here");
            state.is_running = false;
            state.is_visible = false;
            return true;
        case EVENT_CODE_WINDOW_MINIMIZED:
            LOG_INFO("-------------- window minimized");
            state.is_visible = false;
            return true;
        case EVENT_CODE_WINDOW_MAXIMIZED:
            LOG_INFO("-------------- window maximized");
            state.is_visible = true;
            break;
        case EVENT_CODE_WINDOW_RESIZED:
            LOG_INFO("-------------- window resized: %d x %d", context.data.u16[0], context.data.u16[1]);
    }

    return false;
}

b8 engine_on_key(u16 code, void *sender, void *listener, event_context context){
    if(code == EVENT_CODE_KEY_PRESSED){
        u16 key = context.data.u16[0];
        if(key == KEY_ESCAPE){
            event_context context = {0};
            event_send(EVENT_CODE_APPLICATION_QUIT, 0, context);
            return true;
        } else if(key == KEY_W){
            LOG_DEBUG("w key pressed");
        }
    } else if(code == EVENT_CODE_KEY_RELEASED){
        u16 key = context.data.u16[0];
        if(key == KEY_A){
            LOG_DEBUG("a key released");
        }
    }

    return false;
}
