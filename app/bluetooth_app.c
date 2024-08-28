#include "bluetooth_app.h"
#include <unistd.h>
#include <string.h>

static char read_buf[128];
static int buf_len;
static char fixed_header[] = {0xF1,0xDD};
static bluetooth_app_waitACK(Serialdevice *serial_device)
{
    char ack[4];
    usleep(50);
    int res = read(serial_device->super.fd,ack,4);
    if (res != 4)
    {
        return -1;
    }
    if (ack[0] == 'O' && ack[1] == 'K')
    {
        return 0;
    }
    return -1;
}

/*将收到的数据从read_buf中忽略掉前n个字节*/
static bluetooth_app_ignore(int n)
{
    buf_len -= n;
    memmove(read_buf,read_buf + n,buf_len);
}


/*检查蓝牙芯片状态*/
int bluetooth_app_status(Serialdevice *serial_device)
{
    int res = write(serial_device->super.fd,"AT\r\n",4);
    if (res != 4)
    {
        return -1;
    }
    return bluetooth_app_waitACK(serial_device);
}

int bluetooth_app_setBaudrate(Serialdevice *serial_device,BT_BaudRate baudrate)
{
    char buf[] = "AT+BAUD1\r\n";
    buf[7] = baudrate;
    int res = write(serial_device->super.fd,buf,10);
    if (res != 10)
    {
        return -1;
    }
    
    return bluetooth_app_waitACK(serial_device);
}

int bluetooth_app_reset(Serialdevice *serial_device)
{
    int res = write(serial_device->super.fd,"AT+RESET\r\n",10);
    if (res != 10)
    {
        return -1;
    }
    
    return bluetooth_app_waitACK(serial_device);
}

int bluetooth_app_setNetID(Serialdevice *serial_device,char *net_id)
{
    if (strlen((net_id) != 4))
    {
        return -1;
    }
    
    char buf[] = "AT+NETID0000\r\n";
    memcpy(buf + 8,net_id,4);

    int res = write(serial_device->super.fd,buf,14);
    if (res != 14)
    {
        return -1;
    }
    
    return bluetooth_app_waitACK(serial_device);
    
}


int bluetooth_app_setMaddr(Serialdevice *serial_device,char *m_addr)
{
    if (strlen((m_addr) != 4))
    {
        return -1;
    }
    
    char buf[] = "AT+MADDR0000\r\n";
    memcpy(buf + 8,m_addr,4);

    int res = write(serial_device->super.fd,buf,14);
    if (res != 14)
    {
        return -1;
    }
    
    return bluetooth_app_waitACK(serial_device);

}

/*设置蓝牙连接类型*/
/**
 * 设置蓝牙应用的连接类型
 * 首先设置连接类型，
 * 然后配置串口的阻塞模式和波特率，最后对蓝牙设备进行初始化和状态检查此函数是蓝牙设备集成的核心部分，
 * 
 * @param serial_device 串口设备结构体指针，包含蓝牙设备的必要信息
 * @param connection_type 蓝牙连接类型，决定数据传输的模式
 * @return 返回蓝牙设备的状态：0表示正常，非0表示存在错误
 */
int bluetooth_app_setConnectionType(Serialdevice *serial_device, ConnetionType connection_type)
{
    // 设置蓝牙连接类型
    serial_device->super.connetion_type = connection_type;
    
    // 设置串口为非阻塞模式，提高响应速度 为下面测试蓝牙能否使用
    serial_app_setBlockMode(serial_device, 0);

    // 设置串口波特率为9600，初期通信使用
    serial_app_setBaudrate(serial_device, BT_BaudRate_9600);
   
    // 清空串口缓冲区，确保通信干净无残留数据
    serial_app_flush(serial_device);
   
    // 检查蓝牙芯片是否通讯 如果回复ok 配置蓝牙
    if (bluetooth_app_status(serial_device) == 0)
    {
        // 设置蓝牙波特率为115200，
        bluetooth_app_setBaudrate(serial_device, BT_BaudRate_115200);
        // 设置蓝牙网络ID，用于网络内设备识别
        bluetooth_app_setNetID(serial_device, "0004");
        // 设置蓝牙设备内存地址，用于数据传输配置
        bluetooth_app_setMaddr(serial_device, "0008");
        // 重置蓝牙设备，应用上述配置
        bluetooth_app_reset(serial_device);
    }

    // 再次设置串口波特率为115200，确保后续通信
    serial_app_setBaudrate(serial_device, BT_BaudRate_115200);
    // 再次清空串口缓冲区，准备新的通信
    serial_app_flush(serial_device);

    // 检查并获取蓝牙设备状态
    int res = bluetooth_app_status(serial_device);

    // 设置串口为阻塞模式，确保稳定的数据传输
    serial_app_setBlockMode(serial_device, 1);

    // 返回蓝牙设备状态：0表示正常，非0表示存在错误
    return res;
}

/*将消息data转为蓝牙字符串格式再放入data  */
int bluetooth_app_msgTosend(Device *device,void *data,int len)
{
    char buf[128];
    int id_len = 0;
    int msg_len = 0;
    memcpy(&id_len,data + 1,1);
    memcpy(&msg_len,data + 1,1);

    memcpy(buf, "At+MESH", 8);/*把\0也拷贝过去*/
    memcpy(buf + 8, data + 3, 2);
    memcpy(buf + 10, data + 5, msg_len);
    memcpy(buf + 10 + msg_len, "\r\n", 2);
    
    len = 10 + msg_len + 2;

    memcpy(data, buf,len);

    return 0;


}


// 处理蓝牙接收的数据，区分不同的数据帧并进行相应处理
int bluetooth_app_recvTomsg(Device *device,void *data,int len)
{
    // 将接收到的数据追加到读缓冲区中
    memcpy(read_buf + buf_len,data,len);
    buf_len += len;
    // 如果缓冲区数据长度不足4个字节，返回
    if (buf_len < 4)
    {
        return 0;
    }
    int data_type = 0; // 用于标记数据帧类型
    // 分辨数据帧
    // 只需检查到倒数第四个字节，因为剩下的即使符合也不够四个字节
    for (int  i = 0; i < buf_len -3; i++)
    {
        // 如果找到"OK\r\n"，处理收到的ack
        if(memcmp(read_buf + i, "OK\r\n", 4))
        {
            bluetooth_app_ignore(i + 4);
        }
        // 如果找到帧头
        else if (memcmp(read_buf + i, fixed_header, 2))
        {
            // 清理掉帧头之前的数据
            bluetooth_app_ignore(i);
            data_type = 1; // 标记找到有效数据类型
            break;
        }    
    }
    // 如果未识别到有效数据类型，返回
    if (data_type == 0)
    {
        return 0;
    }
    int temp_len = 0; // 用于存储数据长度
    // 读取剩余数据的长度，在第三位
    memcpy(&temp_len, read_buf + 2, 1);
    // 判断实际接收的数据长度buf_len是否大于原本要发来的数据长度
    if (buf_len < temp_len + 3)
    {
       return 0;
    }
    // 拼接数据
    char buf[128];
    memcpy(buf,&device->connetion_type,1);
    buf[1] = 2;
    buf[2] = temp_len -4;
    memcpy(buf+3,read_buf+3,2);
    memcpy(buf+5,read_buf+7,buf[2]);
    len = buf[2] + 5;
    // 将拼接好的数据复制到输入参数data指向的内存空间
    memcpy(data,buf,len);
    // 清理已处理的数据
    bluetooth_app_ignore(temp_len + 3);

    return len;
}