#ifndef __SERIAL_APP_H__
#define __SERIAL_APP_H__


#define _DEFAULT_SOURCE

#include "device_app.h"
#include  <termio.h>
/*波特率*/
typedef enum {
    baudrate_9600 = B9600,
    baudrate_19200 = B19200,
    baudrate_38400 = B38400,
    baudrate_57600 = B57600,
    baudrate_115200 = B115200,
} BaudRate;

/*校验位*/
typedef enum {
    Parity_None = 0,                  /*无校验*/
    Parity_Odd = PARENB | PARODD,     /*奇校验*/
    Parity_Even = PARENB,             /*偶校验*/
} Parity;

/*数据位*/
typedef enum {
    DataBits_5 = CS5,
    DataBits_6 = CS6,
    DataBits_7 = CS7,
    DataBits_8 = CS8,
} DataBits;

/*停止位*/
typedef enum {
    StopBits_1 = 0,
    StopBits_2 = CSTOPB,    
} StopBits;


typedef struct {
    Device super;
    BaudRate baudrate;
    Parity parity;
    DataBits databits;
    StopBits stopbits;

} Serialdevice;

int serial_app_init(Serialdevice *serial_device,char *filename);

int serial_app_setBaudrate(Serialdevice *serial_device,BaudRate baudrate);

int serial_app_setParity(Serialdevice *serial_device,Parity parity);

int serial_app_setDataBits(Serialdevice *serial_device,DataBits databits);

int serial_app_setStopBits(Serialdevice *serial_device,StopBits stopbits);

int serial_app_setBlockMode(Serialdevice *serial_device,int enable);


int serial_app_flush(Serialdevice *serial_device);

#endif
