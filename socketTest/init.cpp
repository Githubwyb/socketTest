#include "init.h"
#include "log.h"

int Init::appInit() {
    if(LOG_INIT() != 0) {
        return -1;
    }
    LOG_INFO("Log init success");

    return 0;
}
