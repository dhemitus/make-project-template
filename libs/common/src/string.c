#include "dhemitus/string.h"
#include "preference.h"
#include "dhemitus/memory.h"
#include "dhemitus/logger.h"
#include <string.h>

u64 string_length(const char *str){
    if(!str){
        return 0;
    }

    return strlen(str);
}

char *string_duplicate(const char *str){
    if(!str){
        LOG_WARN("there is no string");
        return 0;
    }

    u64 length = string_length(str);
    char *copy = allocate_memory(length + 1, MEMORY_LABEL_STRING);
    copy_memory(copy, str, length);
    copy[length] = 0;
    return copy;
}
