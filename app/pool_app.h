#ifndef __POOL_H
#define __POOL_H

typedef struct 
{
    void (*task_func)(void*);
    void *argv;
}Task;



int pool_init(int count);

int pool_registerTask(Task *task);

void pool_close(void);

#endif

