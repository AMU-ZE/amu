#ifndef __BUFFER_APP
#define __BUFFER_APP

#include <pthread.h>


typedef struct 
{
    unsigned char *ptr;    /*缓存区指针*/
    int len;               /*缓存区已存储数据大小*/
    int size;                /*缓存区数据总长度*/
    int start;              /*数据起始*/
    pthread_mutex_t lock;    /*缓存锁*/ 
}Buf;

/*初始化缓存结构体*/
int buffer_app_init(Buf * buffer,int n);

/*释放缓存*/
void buffer_app_free(Buf * buffer);

/*从缓存读取数据*/
int buffer_app_read(Buf * buffer, void*buf, int len);


/*将数据写入缓存*/
int buffer_app_write(Buf* buffer, void*buf,int len);


#endif



