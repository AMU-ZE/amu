#include "log/log.h"


int main(int argc, void *argv[])
{
    log_set_level(LOG_TRACE);
    log_trace("Test trace");/*追踪日志  内容详细*/
    log_debug("Test debug");/**/
    log_info("Test info");
    log_warn("Test warn");
    log_error("Test error");
    log_fatal("Test fatal");

}