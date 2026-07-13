#include "dhemitus/logger.h"
#include "dhemitus/asserts.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 init_log(void){
    return true;
}

void close_log(void){
}

void log_output(log_type type, const char *message, ...){
    const char *type_string[6] = {"[FATAL]: ", "[ERROR]: ", "[WARNING]: ","[INFO]: ", "[DEBUG]: ", "[TRACE]: "};

//    b8 is_error = type < LOG_TYPE_WARN;

    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[32000];
    sprintf(out_message2, "%s%s\n", type_string[type], out_message);
    printf("%s", out_message2);
}

void report_assertion_failure(const char *expression, const char *message, const char *file, i32 line){
    log_output(LOG_TYPE_FATAL, "assertion failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}
