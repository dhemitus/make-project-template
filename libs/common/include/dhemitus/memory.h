#pragma once

#include "preference.h"

typedef enum memory_label {
    MEMORY_LABEL_UNKNOWN,
    MEMORY_LABEL_ARRAY,
    MEMORY_LABEL_DYNAMIC_ARRAY,
    MEMORY_LABEL_DICTIONARY,
    MEMORY_LABEL_RING_QUEUE,
    MEMORY_LABEL_BST,
    MEMORY_LABEL_STRING,
    MEMORY_LABEL_APPLICATION,
    MEMORY_LABEL_JOB,
    MEMORY_LABEL_TEXTURE,
    MEMORY_LABEL_MATERIAL_INSTANCE,
    MEMORY_LABEL_RENDERER,
    MEMORY_LABEL_GAME,
    MEMORY_LABEL_TRANSFORM,
    MEMORY_LABEL_ENTITY,
    MEMORY_LABEL_ENTITY_NODE,
    MEMORY_LABEL_SCENE,
    MEMORY_LABEL_MAX_LABELS,
} memory_label;

DHEMITUS_API void memory_init(void);
DHEMITUS_API void memory_destroy(void);
DHEMITUS_API void *allocate_memory(u64 size, memory_label label);
DHEMITUS_API void free_memory(void *block, u64 size, memory_label label);
DHEMITUS_API void *zero_memory(void *block, u64 size);
DHEMITUS_API void *copy_memory(void *dest, const void *source, u64 size);
DHEMITUS_API void *set_memory(void *dest, i32 value, u64 size);

DHEMITUS_API char *get_memory_to_string(void);
