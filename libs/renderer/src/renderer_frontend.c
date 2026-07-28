#include "dhemitus/renderer_backend.h"
#include "dhemitus/renderer_frontend.h"
#include "dhemitus/logger.h"
#include "dhemitus/memory.h"
#include "dhemitus/renderer_type.h"

static renderer_backend *backend = 0;

b8 renderer_init(const char *application_name, struct window_context *platform_state){
    backend = allocate_memory(sizeof(renderer_backend), MEMORY_LABEL_RENDERER);

    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, platform_state, backend);
    backend->frame_number = 0;

    if(!backend->backend_init(backend, application_name, platform_state)){
        LOG_FATAL("initialize render backend failed!");
        return false;
    }

    return true;
}
void renderer_destroy(void){
    backend->backend_destroy(backend);
    free_memory(backend, sizeof(renderer_backend), MEMORY_LABEL_RENDERER);
}

void renderer_on_resized(u16 width, u16 height);

b8 renderer_begin_frame(u64 delta_time){
    return backend->begin_frame(backend, delta_time);
}

b8 renderer_end_frame(u64 delta_time){
    b8 result = backend->end_frame(backend, delta_time);
    backend->frame_number++;
    return result;
}

b8 renderer_draw_frame(render_packet *packet){
    if(renderer_begin_frame(packet->delta_time)){
        b8 result = renderer_end_frame(packet->delta_time);

        if(!result){
            LOG_ERROR("rendering end frame failed");
            return false;
        }
    }
    return true;
}
