#ifndef _OTA_MEMCHUNK_H_
#define _OTA_MEMCHUNK_H_


typedef struct 
{
    char *data;
    int len;
}MemChunk;

int ota_memchunk_init(MemChunk *chunk);

int ota_memchunk_append(MemChunk *chunk,void *data,int len);


void ota_memchunk_free(MemChunk *chunk);



#endif