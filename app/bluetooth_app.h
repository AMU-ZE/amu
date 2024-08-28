#ifndef __BLUETOOTH_APP_H__
#define __BLUETOOTH_APP_H__ 


#include "serial_app.h"
#include "message_app.h"
#include "device_app.h"


typedef enum{
    BT_BaudRate_9600 = '4',
    BT_BaudRate_19200 = '5',
    BT_BaudRate_38400 = '6',
    BT_BaudRate_57600 = '7',
    BT_BaudRate_115200 = '8'
}BT_BaudRate;


int bluetooth_app_status(Serialdevice *serial_device);

int bluetooth_app_setBaudrate(Serialdevice *serial_device,BT_BaudRate baudrate);

int bluetooth_app_reset(Serialdevice *serial_device);

int bluetooth_app_setNetID(Serialdevice *serial_device,char *net_id);

int bluetooth_app_setMaddr(Serialdevice *serial_device,char *m_addr);

int bluetooth_app_setConnectionType(Serialdevice *serial_device,ConnetionType connection_type);

int bluetooth_app_msgTosend(Device *device,void *data,int len);

int bluetooth_app_recvTomsg(Device *device,void *data,int len);

#endif
