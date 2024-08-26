#include "app/message_app.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>


int main(void)
{
    // 定义一个JSON格式的字符串，描述了消息的连接类型、ID和内容
    char str[] = "{\"connection_type\":1,\"id\":\"0A0B\",\"message\":\"AACCBBDD\"}";
    // 定义一个unsigned char数组，存储与消息相关的一系列字节数据
    unsigned char bin[] = {0x01,0x02,0x04,0x0A,0x0B,0xAA,0xCC,0xBB,0xDD};
    // 定义一个char数组作为缓冲区，用于存储消息的二进制或JSON表示形式
    char buf[128] = {0};
    // 定义Message结构体实例，用于处理消息
    Message message;

    // 解析json字符串，并将解析结果存储在message对象中
    message_app_initByJson(&message,str,strlen(str));
    // 将message消息对象转换为二进制形式，并存储到缓冲区中
    int len = message_app_toBinary(&message,buf,128);
    //断言转换结果与预定义的字节数据相同
    assert(len == 9);
    assert(memcmp(bin,buf,9) == 0);

    // 释放消息对象的资源
    meesage_app_free(&message);
    
    // 解析二进制bin字符串，并将解析结果存储在message对象中
    message_app_initByBinary(&message,bin,9);
    // 将消息对象转换回JSON形式，并存储到缓冲区中
    message_app_toJson(&message,buf,sizeof(buf));

    // 断言转换回的JSON形式与原始JSON字符串相同
    assert(strcmp(str,buf) == 0);

    // 再次释放消息对象的资源
    meesage_app_free(&message);
    
    // 主函数执行完毕，返回0
    return 0;
}