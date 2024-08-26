#ifndef __MESSAGE_APP_H__
#define __MESSAGE_APP_H__


/*枚举类型，用于表示不同的连接方式*/
typedef enum
{
    CONNETION_NONE,
    CONNETION_BT,
    CONNETION_CAN,
    CONNETION_LORA,
}ConnetionType;


typedef struct {
    ConnetionType connetion_type;
    int id_len;
    int message_len;
    unsigned char *mtr;
} Message;

int message_app_initByJson(Message *message, char*json,int len);

int message_app_initByBinary(Message *message, void*ptr,int len);



int message_app_toJson(Message *message,char *json,int len);
int message_app_toBinary(Message *message,void *ptr,int len);


void meesage_app_free(Message *message);

#endif

