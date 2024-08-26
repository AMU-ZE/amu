#include "pool_app.h"
#include <mqueue.h>  /*mq_open()*/
#include <pthread.h> /*pthread_create()*/
#include <stdlib.h>  /*malloc()*/
#include <stdio.h>
static pthread_t *executors = NULL;
static int exector_count;
mqd_t mq;

char *name = "/pool_mq";


void *executor_func(void *argv)
{
    while (1)
    {
        Task task;
        int res = mq_receive(mq, (char *)&task, sizeof(Task), 0);
        if (res >= 0)
        {
            task.task_func(task.argv);

        }
    }
}

/**
 * 参数为后台线程个数
 *返回值 0成功 -1失败 */

int pool_init(int count)
{
    /*初始化消息队列*/
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Task);

    mq = mq_open(name, O_CREAT | O_RDWR, 0644, &attr);

    /*开启后台线程executor*/
    exector_count = count;

    executors = malloc(exector_count * sizeof(pthread_t));
    if (!executors)
    {
        /*申请失败清理线程*/
        mq_unlink(name);
        perror("malloc失败");
        return -1;
    }
    for (int i = 0; i < exector_count; i++)
    {
        int result = pthread_create(executors + i, NULL, executor_func, NULL);
        if (result < 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int pool_registerTask(Task *task)
{
    return mq_send(mq, (char *)task, sizeof(Task), 0);
}

void pool_close()
{
    for (int i = 0; i < exector_count; i++)
    {
        pthread_cancel(executors[i]);
        pthread_join(executors[i], NULL);
    }
    free(executors);
    mq_unlink(name);
}
