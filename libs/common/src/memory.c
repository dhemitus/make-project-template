#include "dhemitus/memory.h"
#include "dhemitus/platform.h"
#include "dhemitus/logger.h"
#include "dhemitus/string.h"
#include "preference.h"
#include <stdio.h>

struct memory_status {
    u64 total_memory_allocated;
    u64 labeled_allocations[MEMORY_LABEL_MAX_LABELS];
};

static struct memory_status status;

static const char *memory_label_string[MEMORY_LABEL_MAX_LABELS] = {
    "UNKNOWN          ",
    "ARRAY            ",
    "DYNAMIC_ARRAY    ",
    "DICTIONARY       ",
    "RING_QUEUE       ",
    "BST              ",
    "STRING           ",
    "APPLICATION      ",
    "JOB              ",
    "TEXTURE          ",
    "MATERIAL_INSTANCE",
    "RENDERER         ",
    "GAME             ",
    "TRANSFORM        ",
    "ENTITY           ",
    "ENTITY_NODE      ",
    "SCENE            "
};

void memory_init(void){
    platform_zero_memory(&status, sizeof(status));
}
void memory_destroy(void){

}
void *allocate_memory(u64 size, memory_label label){
    if(label == MEMORY_LABEL_UNKNOWN) {
        LOG_WARN("unrecommended memory label");
    }

    status.total_memory_allocated += size;
    status.labeled_allocations[label] += size;

    void *block = platform_allocate_memory(size, false);
    platform_zero_memory(block, size);
    return block;

}
void free_memory(void *block, u64 size, memory_label label){
    if(label == MEMORY_LABEL_UNKNOWN) {
        LOG_WARN("unrecommended memory label");
    }

    status.total_memory_allocated -= size;
    status.labeled_allocations[label] -= size;

    platform_free_memory(block, false);
}

void *zero_memory(void *block, u64 size){
    return platform_zero_memory(block, size);
}

void *copy_memory(void *dest, const void *source, u64 size){
    return platform_copy_memory(dest, source, size);
}

void *set_memory(void *dest, i32 value, u64 size){
    return platform_set_memory(dest, value, size);
}

char *get_memory_to_string(void){
    char buff[8000] = "system memory use (labeled): \n";
    u64 offset = string_length(buff);
    for(u32 i = 0; i < MEMORY_LABEL_MAX_LABELS; ++i){
        float amount = 1.0f;
        char unit[4] = "oiB";
        if(status.labeled_allocations[i] >= GIBBYTES(amount)){
            unit[0] = 'G';
            amount = (float)status.labeled_allocations[i] / GIBBYTES(1);
        } else if(status.labeled_allocations[i] >= MEBBYTES(amount)){
            unit[0] = 'M';
            amount = (float)status.labeled_allocations[i]  / MEBBYTES(1);
        } else if(status.labeled_allocations[i] >= KIBBYTES(amount)){
            unit[0] = 'K';
            amount = (float)status.labeled_allocations[i] / MEBBYTES(amount);
        } else {
            unit[0] = 'B';
            amount = (float)status.labeled_allocations[i];
        }

        i32 len =  snprintf(buff + offset, 8000, "%s: %.2f%s\n", memory_label_string[i], amount, unit);
        offset += len;
    }

    char *out_string = string_duplicate(buff);
    return out_string;
}
