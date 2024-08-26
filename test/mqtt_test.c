#include "app/mqtt_app.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

static char buf[1024];
static int message_arrived = 0;

int recv_callback(char *message, int len)
{
    strncpy(buf, message, len);
    message_arrived = 1;
    return 0;
}

int main(void)
{
    memset(buf, 0, sizeof(buf));
    int result = mqtt_app_init();
    assert(result == 0);
    mqtt_app_registerRecvcallback(recv_callback);

    result = mqtt_app_send("abcdef", 7);
    assert(result == 0);

    while (!message_arrived) {
        sleep(1);
    }

    assert(strncmp(buf, "abcdef",6) == 0);

    mqtt_app_close();

    return 0;
}