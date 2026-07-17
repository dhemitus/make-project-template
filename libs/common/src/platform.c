#include "dhemitus/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *allocate_memory(u64 size, b8 aligned){
    (void)aligned;
    return malloc(size);
}

void free_memory(void *block, b8 aligned){
    (void)aligned;
    free(block);
}

void *zero_memory(void *block, u64 size){
    return memset(block, 0, size);
}

void *copy_memory(void *dest, const void *source, u64 size){
    return memcpy(dest, source, size);
}

void *set_memory(void *dest, i32 value, u64 size){
    return memset(dest, value, size);
}

void console_write(const char *message, u8 color){
    const char *color_strings[] = {"0;41", "0;31", "0;33", "0;32", "0;34", "0;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], message);
}

void console_write_error(const char *message, u8 color){
    const char *color_strings[] = {"0;41", "0;31", "0;33", "0;32", "0;34", "0;30"};
    fprintf(stderr, "\033[%sm%s\033[0m", color_strings[color], message);
}
