#include "cJSON/cJSON.h"
#include <stdio.h>


int main(int argc, char const*argv[])
{
    char str[100];
    scanf("%s",str);
    /**
    * @brief 从 JSON 字符串中解析出 cJSON 对象
    * 
    * @param value 指向 JSON 字符串的指针
    * @return cJSON* 返回解析后的 cJSON 对象，解析失败则返回 NULL*/
    cJSON *item = cJSON_Parse(str);
    if (!item)
    {
        perror("Json parse failed");
    }
    
    /**
    * @brief 将指定的数字值添加到 cJSON 对象中
    * 
    * @param object cJSON 对象
    * @param name 键名称
    * @param number 要添加的数字值
    * @return cJSON* 返回指向添加的 cJSON 对象的指针，如果添加失败则返回 NULL*/
    // 给JSON Object添加自定义属性
    cJSON_AddNumberToObject(item,"prop-01",12);

    /**
    * @brief 将 cJSON 对象以未格式化的形式转换为字符串
    * 
    * @param item cJSON 对象
    * @return char* 返回转换后的字符串，如果转换失败则返回 NULL*/
    char *output = cJSON_PrintUnformatted(item);

    // 使用完毕的JSON Object一定要释放
    cJSON_Delete(item);
    printf("res:%s\n",output);

    // 就是标准库的free
    cJSON_free(output); 

}