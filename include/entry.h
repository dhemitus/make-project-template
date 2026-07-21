#include "dhemitus/logger.h"
#include "dhemitus/asserts.h"
#include "dhemitus/engine.h"
#include "preference.h"

extern b8 engine_init(engine *engine);

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    LOG_FATAL("std::vector<const char*> enabledInstanceExtensions; fatal situation: %f", 3.14f);
    LOG_ERROR(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_WARN(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_INFO(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_DEBUG(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    LOG_TRACE(" std::vector<const char*> enabledInstanceExtensions;error situation: %f", 3.14f);
    ASSERTION(1 == 1);

    engine engine = {0}; 
    if(!engine_init(&engine)){
        LOG_INFO("-----------------engine init failed");
        return -2;
    }

    if(!engine_create(&engine)){
        LOG_INFO("-----------------engine create failed");
        return -1;
    }

    engine_run(&engine);

    return 0;
}
