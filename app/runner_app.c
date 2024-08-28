#include "runner_app.h"
#include "serial_app.h"
#include "bluetooth_app.h"
#include "poll.h"
#include "router_app.h"
#include "thirdparty/log/log.h"
#include "stdlib.h"


static Serialdevice serial_device;


void runner_app_run(void)
{
    if(serial_app_init(&serial_device,"/dev/ttyS1") < 0)
    {
        log_fatal("serial init failed");
        exit(EXIT_FAILURE);
    }

}