#include "buffer_app.h"
#include <stdlib.h>
#include "thirdparty/log/log.h"

#include <string.h>
static pthread_mutex_t lock_in = PTHREAD_MUTEX_INITIALIZER;
/*初始化缓存结构体*/
int buffer_app_init(Buf *buffer, int n)
{
    if (buffer->ptr)
    {
        log_warn("buffer already initia");
        return -1;
    }

    buffer->ptr = malloc(n);
    if (!buffer->ptr)
    {
        log_warn("not enough");
        return -1;
    }

    memcpy(&buffer->lock, &lock_in, sizeof(pthread_mutex_t));
    buffer->size = n;
    buffer->start = 0;
    buffer->len = 0;

    return 0;
}

/*释放缓存*/
void buffer_app_free(Buf *buffer)
{
    free(buffer->ptr);
}

/*从缓存读取数据*/
/*len 为实际读取长度 最后返回实际读取长度*/
int buffer_app_read(Buf *buffer, void *buf, int len)
{
    /*如果没有这个结构体和指针 报错*/
    if (!buffer || !buf)
    {
        log_warn("buffer or buf not volid");
        return -1;
    }

    pthread_mutex_lock(&buffer->lock);
    len = buffer->len < len ? buffer->len : len;

    if (buffer->start + len > buffer->size)
    {
        // 拷贝两次
        int first_cp = buffer->size - buffer->start;
        memcpy(buf, buffer->ptr + buffer->start, first_cp);
        memcpy(buf + first_cp, buffer->ptr, len - first_cp);
        buffer->start = len - first_cp;
    }
    else
    {
        memcpy(buf, buffer->ptr + buffer->start, len);
        buffer->start += len;
    }

    buffer->len -= len;

    pthread_mutex_unlock(&buffer->lock);
    log_trace("buffer: %p start: %d len: %d", buffer, buffer->start, buffer->len);

    return len;
}

/*将数据写入缓存*/
int buffer_app_write(Buf *buffer, void *buf, int len)
{
    /*如果没有这个结构体和指针 报错*/
    if (!buffer || !buf)
    {
        log_warn("buffer or buf not volid");
        return -1;
    }

    pthread_mutex_lock(&buffer->lock);

    if (buffer->size - buffer->len < len)
    {

        pthread_mutex_unlock(&buffer->lock);
        log_warn("storage not enough");
        return -1;
    }

    int start_w = buffer->start + buffer->len;

    if (start_w > buffer->size)
    {
        start_w -= buffer->size;
    }

    if (start_w + len > buffer->size)
    {
        int first_cp = buffer->size - start_w;
        memcpy(buffer->ptr + start_w, buf, first_cp);
        memcpy(buffer->ptr, buf + first_cp, len - first_cp);
    }
    else
    {
        /*写一次*/
        memcpy(buffer->ptr + start_w, buf, len);
    }

    buffer->len += len;

    pthread_mutex_unlock(&buffer->lock);
    log_trace("buffer: %p start: %d len: %d", buffer, buffer->start, buffer->len);

    return 0;
}