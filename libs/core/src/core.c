#include <stdio.h>
#include "dhemitus/core.h"
#include "dhemitus/common.h"

/* Public: Explicitly exported */
int math_add(int a, int b) {
    int res = a + b;
    mylib_internal_helper(res);
    return res;
}

/* Public: Explicitly exported */
int math_sub(int a, int b) {
    int res = a - b;
    mylib_internal_helper(res);
    return res;
}
