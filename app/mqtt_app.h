#ifndef __MQTT_APP_H__
#define __MQTT_APP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int mqtt_app_init(void);

int mqtt_app_send(char* topic, char* payload);






#endif
