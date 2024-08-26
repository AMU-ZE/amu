#ifndef __DEVICE_APP_H__
#define __DEVICE_APP_H__


#include "buffer_app.h"
#include "message_app.h"

typedef struct vTable;

//定义设备结构体类型，用于表示和操作设备的相关信息
typedef struct {
      
    struct vTable *vptr;/*设备操作表的指针*/
    char *filename;     /*文件名，用于标识设备的文件路径*/
    int fd;             /*设备文件描述符*/ 
    ConnetionType connetion_type;   /*设备连接类型*/
    Buf recv_buf;       /*接收缓冲区 用于存储接收到的数据*/  
    Buf send_buf;       /*发送缓冲区 用于存储待发送的数据*/ 
    int is_running;     /*运行状态标志 用于指示设备是否正在运行*/
    pthread_t background_thread;    /*后台线程ID*/
} Device;


// 定义一个结构体vTable，用于存储设备的各种回调函数指针
struct vTable 
{
    // 设备接收回调函数指针
    // 该函数在设备接收到数据时调用，目的是处理接收到的数据
    // 参数1: Device *device - 表示设备对象指针
    // 参数2: void *ptr - 指向接收的数据缓冲区
    // 参数3: int len - 接收的数据长度
    // 返回值: int - 表示处理结果
    int (*recv_callback)(Device *,void *, int );
    
    // 设备后台处理函数指针
    // 该函数作为设备的后台处理任务，以非阻塞方式执行
    // 参数: void *arg - 后台任务的参数
    // 返回值: void * - 后台任务的处理结果
    void *(*background_func)(void *);
    
    // 设备读取后处理函数指针
    // 该函数在设备读取数据后调用，用于执行一些后续处理操作
    // 参数1: Device *device - 表示设备对象指针
    // 参数2: void *ptr - 指向读取的数据缓冲区
    // 参数3: int len - 读取到的数据长度
    // 返回值: int - 表示处理结果
    int (*post_read)(Device *,void *, int );

    int (*pre_write)(Device *,void *, int );

    void (*recv_task)(void *);
    void (*send_task)(void *);

};


int device_app_init(Device *device,char *filename);

void device_app_close(Device *device);


int device_app_start(Device *device);

int device_app_stop(Device *device);


int device_app_write(Device *device,void *ptr,int len);

void device_app_regiesterRecvcallback(Device *device,int (*callback)(Device *,void *, int ));

#endif

