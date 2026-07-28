#pragma once

#include "dhemitus/renderer_type.h"

struct platform_state;
struct window_context;

b8 renderer_backend_create(renderer_backend_type type, struct window_context *platform_state, renderer_backend *backend);
void renderer_backend_destroy(renderer_backend *backend);
