#include "dhemitus/renderer_backend.h"
#include "dhemitus/renderer_type.h"
#include "dhemitus/vulkan_backend.h"
#include "preference.h"

b8 renderer_backend_create(renderer_backend_type type, struct window_context *platform_state, renderer_backend *backend){
    backend->platform_state = platform_state;

    if(type == RENDERER_BACKEND_TYPE_VULKAN){
        // hmm
        backend->backend_init = vulkan_backend_init;
        backend->backend_destroy = vulkan_backend_destroy;
        backend->begin_frame = vulkan_begin_frame;
        backend->end_frame = vulkan_end_frame;
        backend->renderer_resized = vulkan_renderer_resized;
        return true;
    }

    return false;
}

void renderer_backend_destroy(renderer_backend *backend){
    backend->backend_init = 0;
    backend->backend_destroy = 0;
    backend->begin_frame = 0;
    backend->end_frame = 0;
    backend->renderer_resized = 0;
}
