#include <string.h>
#include <stdio.h>


int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Usage:./main.c <app|daemon|ota>\n");
        return -1;
    }


    if (strcmp(argv[1],"app") == 0)
    {

    }
    else if (strcmp(argv[1],"ota") == 0)
    {
        
    }
    else if (strcmp(argv[1],"daemon") == 0)
    {
        
    }
    else
    {
        printf("Usage:./main.c <app|daemon|ota>\n");
        return -1;
    }
    
    
    return 0;
    
}