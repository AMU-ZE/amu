#include "device_app.h"
#include <string.h>
#include "thirdparty/log/log.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "pool_app.h"

/**
 * device_app_defaultSendTask函数用于处理设备数据的默认发送任务。
 * 它从设备的发送缓冲区中读取数据，并将这些数据写入到设备的文件描述符中。
 * 此函数展示了如何从设备的发送缓冲区中读取数据并将其发送出去的完整过程。
 * 
 * @param ptr 设备结构体的指针，此处应指向一个Device类型的实例。
 */
void device_app_defaultSendTask(void *ptr)
{
    // 将void指针转换为Device指针，以便操作Device结构体中的成员
    Device *device = ptr;
    
    // 定义一个unsigned char类型的缓冲区，用于临时存储从设备发送缓冲区中读取的数据
    unsigned char buf[1024];
    
    // 从设备的发送缓冲区中读取前3个字节的数据到本地缓冲区buf中
    buffer_app_read(&device->send_buf, buf, 3);
    
    // 从设备的发送缓冲区中读取剩余的数据到本地缓冲区buf中，
    // 其中数据长度由缓冲区中的第二个和第三个字节内容决定
    buffer_app_read(&device->send_buf, buf + 3, buf[1] + buf[2]);

    // 检查设备是否有一个预写入处理函数，处理发送前的数据变为设备想要的数据
    if (device->vptr->pre_write)
    {
        // 调用设备的预写入处理函数，对即将发送的数据进行处理
        device->vptr->pre_write(device, buf, 3 + buf[1] + buf[2]);
    } 
    
    // 通过设备的文件描述符，将缓冲区中的数据写入设备中
    write(device->fd, buf, 3 + buf[1] + buf[2]);
}


void device_app_defaultRecvTask(void *ptr)
{
    unsigned char buf[128];
    Device *device = ptr;
    buffer_app_read(&device->recv_buf, buf, 3);
    buffer_app_read(&device->recv_buf, buf + 3, buf[1] + buf[2]);

    // 调用回调函数，将读取到的数据buf 发送到mqtt服务器
    device->vptr->recv_callback(device,buf,3 + buf[1] + buf[2]);
}

// 设备应用后台读取线程函数
// 该线程持续从设备的文件描述符中读取数据
// 参数 arg 是一个指向 Device 结构的指针，表示与当前线程关联的设备
void *device_app_readBackgroundThread(void *arg)
{
    // 将 void 指针转换为 Device 结构的指针
    Device *device = arg;
    
    // 缓冲区，用于存储从设备读取的数据
    char buf[1024];
    Task task = 
    {
        .argv = device,
        .task_func = device->vptr->recv_task
    };
    
    // 无限循环，持续读取设备数据
    while (1)
    {
        // 从设备的文件描述符中读取数据
        int ret = read(device->fd, buf, sizeof(buf));
        
        // 如果读取操作失败，跳过当前循环继续尝试
        if (ret < 0)
        {
            continue;
        }

        // 如果设备具有在读取数据后的处理函数，则写入 数据
        
        if (device->vptr->post_read)
        {
            int ret = device->vptr->post_read(device, buf, ret);
        }

        // 如果读取的数据量大于 0，则进行后续处理
        if (ret > 0)
        {
            buffer_app_write(&device->recv_buf, buf, ret);
        }

        //读取完数据后在线程池注册一个发送任务 任务名device_app_defaultRecvTask 让线程池处理后续任务 
        pool_registerTask(&task);

    }
    
    // 线程正常执行到这里是无法结束的，因此以下的代码实际上不会被执行
    // 这里的返回语句仅为了语法完整性，实际不会被调用
    return NULL;
}


int device_app_init(Device *device,char *filename)
{
    if (device->fd > 0)
    {
        log_warn("device_app_init device is already open");
        return -1;
    }
    
    device->filename = malloc(strlen(filename) + 1);

    if (!device->filename)
    {
        log_error("malloc device->filename failed");
        free(device->filename);
        return -1;
    }

    if (buffer_app_init(&device->recv_buf,16*1024) < 0)
    {
        free(device->filename);    
    }
    
    if (buffer_app_init(&device->send_buf,16*1024) < 0)
    {
        buffer_app_free(&device->recv_buf);
    }
    /*O_RDWR表示以读写方式打开，O_NOCTTY表示不将此设备作为控制终端*/
    device->fd = open(filename,O_RDWR | O_NOCTTY);
    if (device->fd < 0)
    {
        buffer_app_free(&device->send_buf);
    }
    
    device->vptr->background_func = device_app_readBackgroundThread;
    /*默认情况下，设备不预写入和后读取  需要时自己添加即可*/
    device->vptr->post_read = NULL;
    device->vptr->pre_write = NULL;
    device->vptr->recv_task = device_app_defaultRecvTask;
    device->vptr->send_task = device_app_defaultSendTask;

    /*默认没有设备连接*/
    device->connetion_type = CONNETION_NONE;

    strcpy(device->filename,filename);
    device->is_running = 0;
    log_info("device_app_init device->fd = %d",device->fd);

    return 0;

}


void device_app_close(Device *device)
{
    if (device->is_running)
    {
        log_warn("device is running");
        return -1;
    }
    
    if (device->fd > 0)
    {
        close(device->fd);
        device->fd = -1;

        buffer_app_free(&device->recv_buf);

        buffer_app_free(&device->send_buf);

        free(device->filename);
        device->filename = NULL;
    }
}


//启动后台线程
int device_app_start(Device *device)
{
    if (device->is_running)
    {
        log_warn("device is running");
        return -1;
    }
    if(pthread_create(&device->background_thread,NULL,device->vptr->background_func,(void*)device) < 0)
    {
        log_error("pthread_create failed");
        return -1;
    }
    device->is_running = 1;

}

int device_app_stop(Device *device)
{
    if (!device->is_running)
    {
        log_warn("device is not running");
        return -1;
    }
    
    pthread_cancel(device->background_thread);
    pthread_join(device->background_thread,NULL);

    device->is_running = 0;
    log_info("device_app_stop device->fd = %d",device->fd);

    return 0;
}


int device_app_write(Device *device,void *ptr,int len)
{
    Task task = 
    {
        .argv = device,
        .task_func = device->vptr->send_task
    };

    /* 写入数据到发送缓冲区 */
    int res = buffer_app_write(&device->send_buf,ptr,len);
    if (res < 0)
    {
        log_error("device send buffer is full");
        return -1;
    }
    
    return pool_registerTask(&task);

}


void device_app_regiesterRecvcallback(Device *device,int (*callback)(Device *,void *, int ))
{
    device->vptr->recv_callback = callback;
}
