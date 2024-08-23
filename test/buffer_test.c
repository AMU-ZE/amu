#include "app/buffer_app.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    Buf buffer;
    char buf[16];
    memset(&buffer,0,sizeof(Buf));

    buffer_app_init(&buffer, 16);


    buffer_app_write(&buffer, "are you ok?", 11);


    /* check  如果不满足条件 会报错*/
    assert(buffer.start == 0);
    assert(buffer.len == 11);

    buffer_app_read(&buffer, buf, 7);
    assert(buffer.start == 7); 
    assert(strncmp(buf, "are you", 7) == 0);

    buffer_app_write(&buffer, "apex qidong", 11);


    buffer_app_read(&buffer, buf, 11);

    assert(buffer.start == 2);
    assert(buffer.len == 4);
    assert(strncmp(buf, " ok?apex qi", 11) == 0);

    return 0; 
}