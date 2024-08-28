#include "ota_http.h"
#include "ota_memchunk.h"
#include <stdio.h>
#include <string.h>
#include "thirdparty/log/log.h"
#include "thirdparty/cJSON/cJSON.h"
#include <curl/curl.h>


static int ota_http_calculateSHA1(char *filename,)
{

}

/*请求url 放在memchunk*/
static int ota_http_requestToChunk(char *url,MemChunk *chunk)
{

}
 /*请求url 下载到文件*/
static int ota_http_requestToFile(char *url,FILE *file)
{

}
int ota_http_getVersion(char *url,Version *version)
{
    MemChunk chunk;
    memset(&chunk,0,sizeof(MemChunk));

    if (ota_memchunk_init(&chunk) < 0)
    {
        log_warn("ota_memchunk_init failed");
        return -1;
    }   
    
    /*解析memchunkl 字符串*/
    cJSON * version_obj = cJSON_Parse(chunk.data);
    ota_memchunk_free(&chunk);

    if(version_obj == NULL)
    {
        log_warn("cJSON_Parse failed");
        return -1;
    }

    /*解析版本信息*/
    cJSON *major_obj = cJSON_GetObjectItem(version_obj,"major");
    cJSON *minor_obj = cJSON_GetObjectItem(version_obj,"minor");
    cJSON *patch_obj = cJSON_GetObjectItem(version_obj,"patch");

    /*只要有一个没解析出来，就认为解析失败  */
    if (!major_obj || !minor_obj || !patch_obj)
    {
        log_warn("cJSON_Parse failed");
        cJSON_Delete(version_obj);
        return-1;
    }
    
    version->major = major_obj->valueint;
    version->minor = minor_obj->valueint;
    version->patch = patch_obj->valueint;

}

int ota_http_downloadFirmware(char *firmware_url,char *digest_url,char *file)
{
    //下载到固件
    FILE *firmware = fopen(file,"wb");

    if (firmware == NULL)
    {
        log_error("fopen failed");
        return -1;
    }

    if (ota_http_requestToFile(url,firmware) < 0)
    {
        log_error("ota_http_requestToFile failed");
        fclose(firmware);
        goto OTA_DOWNLOAD_FAIL;
    }








OTA_DOWNLOAD_FAIL:





}
