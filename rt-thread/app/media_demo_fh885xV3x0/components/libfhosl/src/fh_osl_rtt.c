#if 0
#ifdef __RTTHREAD_OS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/fh_osl.h"

#define APP_THREAD_PRIORITY 130 /*only for RTThread*/

void *fh_osl_malloc(unsigned int size)
{
    return rt_malloc(size);
}

void *fh_osl_memset(void *s, int c, unsigned int n)
{
    return rt_memset(s, c, n);
}

void *fh_osl_memcpy(void *dst, void *src, unsigned int n)
{
    return rt_memcpy(dst, src, n);
}

void fh_osl_free(void *ptr)
{
    rt_free(ptr);
}

unsigned int fh_osl_sleep(unsigned int seconds)
{
    rt_thread_delay(seconds * RT_TICK_PER_SECOND);
    return 0;
}

int fh_osl_usleep(unsigned int micro_seconds)
{
    rt_thread_delay(rt_tick_from_millisecond(micro_seconds / 1000));
    return 0;
}

int fh_osl_create_thread(fh_osl_thread_t *thread, fh_osl_thread_attr_t *attr, thread_proc_t proc, void *arg, char* name)
{
    if (!thread || !proc)
    	return -1;

    rt_uint32_t stack_size = 4096;
    rt_uint8_t priority = APP_THREAD_PRIORITY;
    char thread_name[16];
    static int th_cnt;

    if (attr != NULL)
    {
        stack_size = attr->stack_size;
        priority = attr->priority;
    }

    if (!name)
    	name = "";

    fh_osl_snprintf(thread_name, sizeof(thread_name), "th%d_%s", th_cnt, name);
    th_cnt++;
    *thread = rt_thread_create(thread_name, (void (*)(void *))proc, arg, stack_size, priority, 5);
    if (*thread != NULL)
    {
        rt_thread_startup(*thread);
        return 0;
    }

    return -1;
}

int fh_osl_destroy_thread(fh_osl_thread_t thread)
{
    return rt_thread_delete(thread);
}

void fh_osl_gettimeofday(struct timeval *tv)
{
    do_gettimeofday(tv);
}

void fh_osl_init_lock(fh_osl_lock_t *lock)
{
    char name[RT_NAME_MAX];
    static int _lock_count = 0;

    fh_osl_snprintf(name, sizeof(name), "fhosl%2d", _lock_count);
    _lock_count++;

    rt_mutex_init(lock, name, RT_IPC_FLAG_FIFO);
}

void fh_osl_take_lock(fh_osl_lock_t *lock)
{
    rt_mutex_take(lock, RT_WAITING_FOREVER);
}

void fh_osl_release_lock(fh_osl_lock_t *lock)
{
    rt_mutex_release(lock);
}

void fh_osl_destroy_lock(fh_osl_lock_t *lock)
{
    rt_mutex_detach(lock);
}

#endif /*__RTTHREAD_OS__*/
#endif