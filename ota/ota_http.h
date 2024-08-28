#ifndef __OTA_HTTP_H__
#define __OTA_HTTP_H__

typedef struct 
{
    int major;
    int minor;
    int patch;
}Version;


int ota_http_getVersion(char *url,Version *version);

int ota_http_downloadFirmware(char *firmware_url,char *digest_url,char *file);











#endif