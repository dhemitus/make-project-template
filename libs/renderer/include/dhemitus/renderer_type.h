#pragma once

#include "preference.h"

typedef enum renderer_backend_type {
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_WEBGPU,
    RENDERER_BACKEND_TYPE_OPENGL
} renderer_backend_type;

typedef struct renderer_backend {
    u64 frame_number;
    struct window_context *platform_state;
    b8 (*backend_init)(struct renderer_backend *backend, const char *application_name, struct window_context *platform_state);
    void (*backend_destroy)(struct renderer_backend *backend);
    void (*renderer_resized)(struct renderer_backend *backend, u16 width, u16 height);
    b8 (*begin_frame)(struct renderer_backend *backend, u64 delta_time);
    b8 (*end_frame)(struct renderer_backend *backend, u64 delta_time);
} renderer_backend;

typedef struct render_packet {
    u64 delta_time; 
} render_packet;
