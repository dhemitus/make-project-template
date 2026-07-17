#pragma once

#include "preference.h"

void *allocate_memory(u64 size, b8 aligned);
void free_memory(void *block, b8 aligned);
void *zero_memory(void *block, u64 size);
void *copy_memory(void *dest, const void *source, u64 size);
void *set_memory(void *dest, i32 value, u64 size);

void console_write(const char *message, u8 color);
void console_write_error(const char *message, u8 color);
