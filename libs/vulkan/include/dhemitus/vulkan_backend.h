#pragma once
#include "dhemitus/renderer_backend.h"

b8 vulkan_backend_init(struct renderer_backend *backend, const char *application_name, struct window_context *platform_state);
void vulkan_backend_destroy(struct renderer_backend *backend);
void vulkan_renderer_resized(struct renderer_backend *backend, u16 width, u16 height);
b8 vulkan_begin_frame(struct renderer_backend *backend, u64 delta_time);
b8 vulkan_end_frame(struct renderer_backend *backend, u64 delta_time);

