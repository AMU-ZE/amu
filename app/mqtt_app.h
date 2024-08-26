#ifndef __MQTT_APP_H__
#define __MQTT_APP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



// 定义MQTT服务器地址，使用TCP协议连接
#define ADDRESS     "tcp://192.168.10.1:1883"
// 定义客户端ID，用于标识本客户端在MQTT服务器上
#define CLIENTID    "d695ed42-b13c-4084-b159-7ea394ec7518"
// 定义主题，用于发布和订阅消息的 topic  
//#define PULL_TOPIC       "51Car_remote"
#define PULL_TOPIC       "51Car_status"
#define PUSH_TOPIC       "51Car_status"

// 定义消息发布的服务质量等级，QOS为1表示至少一次传输
#define QOS         0
// 定义操作超时时间，单位为毫秒
#define TIMEOUT     10000L
 


int mqtt_app_init(void);

void mqtt_app_close(void);

/*发送的数据*/
int mqtt_app_send(char* payload, int payload_len);


/*回调函数 用于收数据  参数为 函数
*当受到数据调用回调函数来接收
*/
void mqtt_app_registerRecvcallback(int (*callback)(char*payload, int payload_len));





#endif
