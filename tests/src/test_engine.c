#include <stdio.h>
#include <dhemitus/core.h>
#include <dhemitus/common.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    printf("🚀 Starting macOS Demo App...\n");
    int result = math_add(100, 200);
    printf("✅ Final add Result: %d\n", result);
    result = math_sub(200, 100);
    printf("✅ Final sub Result: %d\n", result);
//    mylib_internal_helper(5); // ❌ Linker Error: Symbol not found if uncommented
    return 0;
}
