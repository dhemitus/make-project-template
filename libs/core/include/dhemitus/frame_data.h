#pragma once

#include "preference.h"

typedef struct frame_data {
    u64 last_time;
    u64 current_time;
    u64 accumulator;
    u64 update_time;
} frame_data;

