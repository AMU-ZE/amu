#include "router_app.h"
#include <stdio.h>
#include "thirdparty/log/log.h"
#include "mqtt_app.h"

/*设备最大数量*/
#define DEVICE_COUNT 10

/*设备数组*/
static Device **devices;
static int device_count;

/*处理消息的回调函数 将消息写入对应设备的缓存*/
int router_app_mqttcallback(char *playload, int playload_len)
{
    Message message;
    /*将收到的json数据palyload转换 存到message*/
    message_app_initByJson(&message,playload,playload_len);

    Device *device = NULL;
    for (int i = 0; i < device_count; i++)
    {
        if (devices[i]->connetion_type == message.connetion_type )
        {
            device = devices[i];
            break;
        }
        
    }
    
    char bin[1024];

    /*把message 转换为 二进制数据*/
    message_app_toBinary(&message,bin,sizeof(bin));

    meesage_app_free(&message);

    return device_app_write(device,bin,sizeof(bin));

}


/*将数据发送到mqtt服务器*/
int router_app_devicecallback(Device *device,void *ptr,int len)
{   
    Message message;
    char json_buf[1024];
    /*将ptr的二进制数据转换为字符串  存到message*/    
    int res = message_app_initByBinary(&message,ptr,len);
    if (res < 0)
    {
        meesage_app_free(&message);
        log_warn("message_app_initByBinary failed");
        return -1;
    }
    
    /*将message 转换为json格式存在json buf*/
    res = message_app_toJson(&message,json_buf,sizeof(json_buf));
    if (res < 0)
    {
        log_warn("message_app_toJson failed");
        return -1;
    }
    meesage_app_free(&message);
    /*mqtt发送数据*/
    return mqtt_app_send(json_buf,sizeof(json_buf) + 1);
}

int router_app_init(void)
{
    devices = malloc(sizeof(Device*)*DEVICE_COUNT);
    if (devices == NULL)
    {
        log_warn("router_app_init malloc failed");
        return -1;
    }

    int res = mqtt_app_init();
    if (res < 0)
    {
        log_error("router app init mqtt init failed");
        return -1;
    }

    /*注册mqtt 回调函数 将数据写入到设备缓存*/
    mqtt_app_registerRecvcallback(router_app_mqttcallback);
    
}



/**
 * @brief 注册设备到路由器应用
 * 
 * 当设备数量达到上限时，不再注册新设备，并返回错误代码-1
 * 否则，将设备添加到设备数组中，启动设备应用，并注册接收回调函数
 * 
 * @param device 指向设备结构体的指针
 * @return int 设备注册结果，如果设备数量已满，返回-1，否则返回默认值
 */
int router_app_registerDevice(Device *device)
{
    // 检查设备数量是否达到上限
    if (device_count >= DEVICE_COUNT)
    {
        // 设备数量已满，不再注册新设备
        log_warn("router_app_registerDevice device count is full");
        return -1;
    }
    // 将设备添加到设备数组中，并增加设备计数
    devices[device_count++] = device;
    // 注册接收回调函数 此函数作用将数据发送到mqtt服务器
    device_app_regiesterRecvcallback(device,router_app_devicecallback);
    // 启动设备应用
    device_app_start(device);
}

void router_app_close(void)
{
    for (int i = 0; i < device_count; i++)
    {
        device_app_stop(devices[i]);
        device_app_close(devices[i]);
    }
    
    mqtt_app_close();
    free(devices);
}
