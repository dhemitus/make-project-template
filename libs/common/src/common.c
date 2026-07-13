
#include <stdio.h>
#include "dhemitus/common.h"

/* Internal: Hidden via -fvisibility=hidden (default in CMake config below) */
void mylib_internal_helper(int value) {
    printf("🔒 [Internal] Processing value: %d\n", value);
}

