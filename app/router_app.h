#ifndef  __ROUTE_APP_H__
#define  __ROUTE_APP_H__

#include "device_app.h"

int router_app_init(void);

int router_app_registerDevice(Device *device);


void router_app_close(void);




#endif