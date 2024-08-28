#include "ota_memchunk.h"
#include <stdlib.h>
#include <string.h>


int ota_memchunk_init(MemChunk *chunk)
{
    ota_memchunk_free(chunk);

    chunk->data = malloc(1);
    if (!chunk->data)
    {
        return -1;
    }
    
    chunk->len = 0;
    /*初始化*/
    chunk->data[0] = 0;

    return 0;

}

int ota_memchunk_append(MemChunk *chunk,void *data,int len)
{
    char *temp = realloc(chunk->data,chunk->len + len + 1);
    if (!temp)
    {
        return -1;
    }
    
    chunk->data = temp;
    memcpy(chunk->data + chunk->len,data,len);
    chunk->len += len;
    /*给字符串结尾添加0 封个口 方便后面的扩展*/
    chunk->data[chunk->len] = 0;


}

void ota_memchunk_free(MemChunk *chunk)
{
    if (chunk->data)
    {
        free(chunk->data);
        chunk->data = NULL;
    }
    
}