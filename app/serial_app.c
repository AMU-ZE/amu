#include "serial_app.h"
#include <unistd.h>
#include "thirdparty/log/log.h"

/*设置串口设备为原始模式*/
static serial_app_setRawMode(Serialdevice *serial_device)
{
    struct termios attr;
    /*获取串口属性 并放在attr中*/
    if (tcgetattr(serial_device->super.fd, &attr) < 0)
    {
        log_warn("tcgetattr error");
        return -1;
    }

    /*更改属性*/
    cfmakeraw(&attr);

    /*将属性写回串口*/
    /*参数1表示设备文件描述符，参数2使用标志表示何时生效，此处使用TCSAFLUSH刷新时生效，&attr新终端属性。*/
    return tcsetattr(serial_device->super.fd, TCSAFLUSH, &attr);
}
/*设置阻塞模式 1为阻塞 0为非阻塞*/
int serial_app_setBlockMode(Serialdevice *serial_device, int enable)
{
    struct termios attr;
    /*获取串口属性 并放在attr中*/
    if (tcgetattr(serial_device->super.fd, &attr) < 0)
    {
        log_warn("tcgetattr error");
        return -1;
    }

    /*设置串口阻塞模式*/
    if (enable) 
    {
        attr.c_cc[VMIN] = 1;/*读取至少一个字节返回 */
        attr.c_cc[VTIME] = 0;/*超时时间为0*/        
    }
    else
    {
        /*读取200ms后会返回*/
        attr.c_cc[VMIN] = 0;
        attr.c_cc[VTIME] = 2;
    }
    
    

    /*将属性写回串口*/
    /*参数1表示设备文件描述符，参数2使用标志表示何时生效，此处使用TCSAFLUSH刷新时生效，&attr新终端属性。*/
    return tcsetattr(serial_device->super.fd, TCSAFLUSH, &attr);
}

int serial_app_flush(Serialdevice *serial_device)
{
    /*执行刷新操作 立即刷新*/
    return tcflush(serial_device->super.fd, TCIOFLUSH);
}

int serial_app_setBaudrate(Serialdevice *serial_device, BaudRate baudrate)
{
    struct termios attr;
    /*获取串口属性 并放在attr中*/
    if (tcgetattr(serial_device->super.fd, &attr) < 0)
    {
        log_warn("tcgetattr error");
        return -1;
    }

    /*更改属性*/
    /*更改波特率 输入波特率和输出波特率*/
    cfsetispeed(&attr, baudrate);
    cfsetospeed(&attr, baudrate);

    /*将属性写回串口*/
    /*参数1表示设备文件描述符，参数2使用标志表示何时生效，此处使用TCSAFLUSH刷新时生效，&attr新终端属性。*/
    return tcsetattr(serial_device->super.fd, TCSAFLUSH, &attr);
}

int serial_app_setParity(Serialdevice *serial_device, Parity parity)
{
    struct termios attr;
    /*获取串口属性 并放在attr中*/
    if (tcgetattr(serial_device->super.fd, &attr) < 0)
    {
        log_warn("tcgetattr error");
        return -1;
    }

    /*更改属性*/
    /*更改校验位 先关闭校验位 并将默认校验设置为偶校验 */
    attr.c_cflag &= ~PARENB;
    attr.c_cflag &= ~PARODD;

    attr.c_iflag &= parity;

    /*将属性写回串口*/
    /*参数1表示设备文件描述符，参数2使用标志表示何时生效，此处使用TCSAFLUSH刷新时生效，&attr新终端属性。*/
    return tcsetattr(serial_device->super.fd, TCSAFLUSH, &attr);
}

int serial_app_setDataBits(Serialdevice *serial_device, DataBits databits)
{
    struct termios attr;
    /*获取串口属性 并放在attr中*/
    if (tcgetattr(serial_device->super.fd, &attr) < 0)
    {
        log_warn("tcgetattr error");
        return -1;
    }

    /*更改属性*/
    /*先志位0 再修改*/
    attr.c_cflag &= ~CSIZE;
    /*修改数据位*/
    attr.c_cflag |= databits;

    /*将属性写回串口*/
    /*参数1表示设备文件描述符，参数2使用标志表示何时生效，此处使用TCSAFLUSH刷新时生效，&attr新终端属性。*/
    return tcsetattr(serial_device->super.fd, TCSAFLUSH, &attr);
}
int serial_app_setStopBits(Serialdevice *serial_device, StopBits stopbits)
{
    struct termios attr;
    /*获取串口属性 并放在attr中*/
    if (tcgetattr(serial_device->super.fd, &attr) < 0)
    {
        log_warn("tcgetattr error");
        return -1;
    }

    /*更改属性*/
    /*先志位0 再修改*/
    attr.c_cflag &= ~CSTOP;
    attr.c_cflag |= stopbits;

    /*将属性写回串口*/
    /*参数1表示设备文件描述符，参数2使用标志表示何时生效，此处使用TCSAFLUSH刷新时生效，&attr新终端属性。*/
    return tcsetattr(serial_device->super.fd, TCSAFLUSH, &attr);
}
int serial_app_init(Serialdevice *serial_device, char *filename)
{
    if (device_app_init(&serial_device->super, filename) < 0)
    {
        log_warn("serial device init failed");
        return -1;
    }

    if (serial_app_setBaudrate(serial_device, baudrate_9600) < 0)
    {
        log_warn("set baudrate failed");
        return -1;
    }

    if (serial_app_setParity(serial_device, Parity_None) < 0)
    {
        log_warn("set parity failed");
        return -1;
    }

    if (serial_app_setDataBits(serial_device, DataBits_8) < 0)
    {
        log_warn("set databits failed");
        return -1;
    }

    if (serial_app_setStopBits(serial_device, StopBits_1) < 0)
    {
        log_warn("set stopbits failed");
        return -1;
    }

    if (serial_app_setRawMode(serial_device) < 0)
    {
        log_warn("set rawmode failed");
        return -1;
    }

    if (serial_app_setBlockMode(serial_device,1) < 0)/*阻塞模式*/
    {
        log_warn("set blockmode failed");
        return -1;
    }

    return serial_app_flush(serial_device);
}