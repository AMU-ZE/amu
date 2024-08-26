#include "mqtt_app.h"
#include <MQTTClient.h>
#include <stdlib.h>
#include "thirdparty/log/log.h"
#include <assert.h>
// 定义MQTT客户端实例
static MQTTClient client;
// 初始化连接选项，设置默认连接参数
static MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
// 初始化待发布的消息
static MQTTClient_message pubmsg = MQTTClient_message_initializer;
// 定义消息传递的令牌
static MQTTClient_deliveryToken token;

static int (*recv_callback)(char*,int);
 
/**
 * 回调函数：确认消息已传递
 * 
 * 此函数用作MQTT客户端的回调函数，当消息成功传递时被调用它打印出已确认传递的消息的令牌值，并将其存储在全局变量deliveredtoken中
 * 
 * @param context 上下文对象，由MQTT客户端库传递，本例中未使用
 * @param dt 消息传递确认的令牌，用于跟踪和确认特定消息的传递
 */
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    assert(context == NULL);
    // 打印出确认已传递的消息的令牌值
    log_trace("Message with token value %d delivery confirmed\n", dt);
   
}

/**
 * 处理MQTT消息到达的回调函数
 * 
 * @param context 回调函数上下文，通常用于访问用户数据，本函数中未使用
 * @param topicName 主题名称，消息发布的主题
 * @param topicLen 主题名称的长度，用于确定主题名称的大小
 * @param message 指向MQTTClient_message结构体的指针，包含接收到的消息信息
 * 
 * @return 返回1表示成功处理了消息
 * 
 * 该函数在接收到MQTT消息时被调用它打印出到达的消息所属的主题和消息内容然后释放消息和主题的相关资源
 */

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    assert(context == NULL);
    log_trace("Message arrived: topic %s message:%s\n",topicName,message->payload);
    int res = recv_callback(message->payload,message->payloadlen);
    return res == 0 ? 1 : 0;
}
 

/**
 * 处理连接丢失的函数
 * 当与服务器的连接意外丢失时调用此函数
 * 
 * @param context 上下文信息，通常用于存储连接的状态信息
 * @param cause 连接丢失的原因，以字符串形式提供
 */
void connlost(void *context, char *cause)
{
    assert(context == NULL);
    // 记录连接丢失的日志，包含原因
    log_warn("Connection lost%s", cause);

}




int mqtt_app_init(void)
{
    // 创建MQTT客户端
    if ((MQTTClient_create(&client, ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to create client, return code\n");
         goto MQTT_CREATE_FAIL;
    }

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    
    // 设置连接选项
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // 连接至MQTT代理
    if ((MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        log_error("Failed to connect, return code\n");
        goto MQTT_CONNECT_FAIL;
    }

    /*订阅*/
    if(MQTTClient_subscribe(client, PULL_TOPIC, QOS) != MQTTCLIENT_SUCCESS)
    {
        log_error("Failed to subscribe, return code\n");
        goto MQTT_SUBSCRIBE_FAIL;
    }

    return 0;


MQTT_SUBSCRIBE_FAIL:
    MQTTClient_disconnect(client, TIMEOUT);

MQTT_CONNECT_FAIL:
    MQTTClient_destroy(&client);

MQTT_CREATE_FAIL:
    return -1;
}



int mqtt_app_send(char* payload, int payload_len)
{
    pubmsg.payload = payload;
    pubmsg.payloadlen = payload_len;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    int res = MQTTClient_publishMessage(client, PUSH_TOPIC, &pubmsg, &token);
    if (res == MQTTCLIENT_SUCCESS)
    {
        log_trace("Message with delivery token %d delivered\n", token);
        return 0;
    }
    else
    {
        log_warn("Failed to deliver message token %d failed\n",token);
        return -1;
    }
    
    return 0;
}


void mqtt_app_close(void)
{
    MQTTClient_disconnect(client, 10000);

    MQTTClient_destroy(&client);
}

void mqtt_app_registerRecvcallback(int (*callback)(char*payload, int payload_len))
{
    recv_callback = callback;
}

