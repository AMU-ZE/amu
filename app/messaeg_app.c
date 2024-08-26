#include "message_app.h"
#include <string.h>
#include "thirdparty/log/log.h"
#include "thirdparty/cJSON/cJSON.h"
#include <stdlib.h>
int message_app_StrToBinary(char *str,void **ptr,int *len)
{

    int str_len = strlen(str);
    if (str_len % 2 != 0)
    {
        log_warn("json string not valid");
        return -1;
    }

    *len = str_len / 2;
    *ptr = malloc(*len);
    if (!*ptr)
    {
        log_warn("malloc failed");
        return -1;
    }
    
    for (int i = 0; i < str_len; i += 2)
    {
        unsigned char temp = 0;
        if (str[i] >= '0' && str[i] <= '9')
        {
            temp += str[i] - '0';
        }
        else if (str[i] >= 'a' && str[i] <= 'f')
        {
            temp += str[i] - 'a' + 10;
        }
        else 
        {
            temp += str[i] - 'A' + 10;
        }

        temp <<= 4;

        if (str[i+1] >= '0' && str[i+1] <= '9')
        {
            temp += str[i+1] - '0';
        }
        else if (str[i+1] >= 'a' && str[i+1] <= 'f')
        {
            temp += str[i+1] - 'a' + 10;
        }
        else  
        {
            temp += str[i+1] - 'A' + 10;
        }
        
        memcpy(*ptr + i / 2, &temp, 1);
        
    }
     
    return 0;
}


char *message_app_BinaryToStr(void *ptr,int len)
{
    char table[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char *str = malloc(len * 2 + 1);

    unsigned char *temp = ptr;

    for (int i = 0; i < len; i++)
    {

        str[i * 2] = table[(temp[i] & 0xf0) >> 4];
        str[i * 2 + 1] = table[temp[i] & 0x0f];
    }

    str[len * 2] = '\0';

    return str;

}

/**
 * 使用JSON初始化消息对象
 * 
 * @param message 指向Message结构体的指针，用于存储解析后的消息
 * @param json 包含消息数据的JSON字符串
 * @param len JSON字符串的长度
 * @return 返回0表示成功，-1表示失败
 * 
 * 该函数通过解析传入的JSON字符串，初始化消息对象message
 * 主要步骤包括：解析JSON字符串，获取连接类型、ID和消息内容，
 * 并将这些数据存储在message指向的对象中
 */
int message_app_initByJson(Message *message, char*json,int len)
{
    // 初始化message对象，确保其初始状态为空
    memset(message,0,sizeof(Message));

    // 解析JSON字符串，获得cJSON对象
    cJSON *json_obj = cJSON_ParseWithLength(json,len);

    // 如果解析失败，记录警告并返回错误
    if (json_obj == NULL)
    {
        log_warn("json parse failed");
        return -1;
    }

    // 解析连接类型
    cJSON *json_connetion_type = cJSON_GetObjectItem(json_obj,"connection_type");

    // 如果解析失败，记录警告，释放资源并返回错误
    if (!json_connetion_type)
    {
        log_warn("connetion_type parse failed");
        cJSON_Delete(json_obj);
        return -1;
    }

    // 存储解析得到的连接类型到message对象
    message->connetion_type = json_connetion_type->valueint;


    // 解析ID
    cJSON *json_id = cJSON_GetObjectItem(json_obj,"id");
    if (!json_id)
    {
        log_warn("id parse failed");
        cJSON_Delete(json_obj);
        return -1;
    }

    // 解析消息内容
    cJSON *json_message = cJSON_GetObjectItem(json_obj,"message");
    if (!json_message)
    {
        log_warn("message parse failed");
        cJSON_Delete(json_obj);
        return -1;
    }

    // 将ID字符串转换为二进制形式
    void *id_ptr;
    if ((message_app_StrToBinary(json_id->valuestring,&id_ptr,&message->id_len)) < 0)
    {
        log_warn("id parse failed");
        cJSON_Delete(json_obj);
        return -1;
    }


    // 将消息内容字符串转换为二进制形式
    void *message_ptr;
    if ((message_app_StrToBinary(json_message->valuestring,&message_ptr,&message->message_len)) < 0)
    {
        log_warn("message parse failed");
        free(message_ptr);
        return -1;
    }

    // 分配内存以存储ID和消息内容
    message->mtr = malloc(message->id_len + message->message_len);
    
    // 复制ID到分配的内存
    memcpy(message->mtr,id_ptr,message->id_len);
    // 复制消息内容到分配的内存
    memcpy(message->mtr + message->id_len,message_ptr,message->message_len);


    // 释放转换后的ID和消息内容的临时内存
    free(id_ptr);
    free(message_ptr);

    // 释放解析后的JSON对象
    cJSON_Delete(json_obj);

    // 成功完成初始化，返回0
    return 0;
}



/**
 * 使用二进制数据初始化消息对象
 * 
 * @param message 指向消息对象的指针
 * @param ptr 二进制数据指针
 * @param len 二进制数据长度
 * @return 返回0表示成功，-1表示失败
 * 
 * 该函数通过指针和长度从二进制数据初始化消息对象
 * 它首先检查二进制数据的长度是否满足最小要求，
 * 然后将数据复制到消息对象的相应字段中，并动态分配内存用于存储消息的主体和ID
 */
int message_app_initByBinary(Message *message, void*ptr,int len)
{
    // 检查二进制数据长度是否小于最小长度，是则返回错误
    if (len < 3)
    {
        return -1;
    }
    
    // 清除消息对象，准备初始化
    memset(message,0,sizeof(Message));
    
    // 从二进制数据中复制消息的连接类型、ID长度和消息长度到消息对象中
    memcpy(&message->connetion_type,ptr,1);
    memcpy(&message->id_len,ptr+1,1);
    memcpy(&message->message_len,ptr+2,1);

    // 动态分配内存用于存储消息的ID和消息体
    message->mtr = malloc(message->id_len + message->message_len);

    // 如果内存分配失败，记录警告并返回错误
    if (message->mtr == NULL)
    {
        log_warn("message malloc failed");
        return -1;
    }

    // 从二进制数据中复制消息的ID和消息体到动态分配的内存中
    memcpy(message->mtr,ptr+3,message->id_len + message->message_len);

    // 记录消息初始化的跟踪日志
    log_trace("message init by binary");

    // 初始化成功，返回0
    return 0;
}



int message_app_toJson(Message *message,char *json,int len)
{
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_obj,"connection_type",message->connetion_type);
    cJSON_AddStringToObject(json_obj,"id",message_app_BinaryToStr(message->mtr,message->id_len));
    cJSON_AddStringToObject(json_obj,"message",message_app_BinaryToStr(message->mtr + message->id_len,message->message_len));

    char* json_str = cJSON_PrintUnformatted(json_obj);
    if ((int)strlen(json_str) +1 > len)
    {
        return -1;
    }
    strcpy(json,json_str);

    free(json_str);

    cJSON_Delete(json_obj);
    
    return 0;
}



int message_app_toBinary(Message *message,void *ptr,int len)
{
    if (message->id_len + message->message_len + 3 > len)
    {
        log_warn("message to binary failed");
        return -1;
    }
    memcpy(ptr,&message->connetion_type,1);

    memcpy(ptr+1,&message->id_len,1);

    memcpy(ptr+2,&message->message_len,1);
    memcpy(ptr+3,message->mtr,message->id_len + message->message_len);
    

    return message->id_len + message->message_len + 3;
}



void meesage_app_free(Message *message)
{

    free(message->mtr);
}