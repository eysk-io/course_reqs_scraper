#include <stdbool.h> 
#include <pthread.h>

#ifndef __TPOOL_H__
#define __TPOOL_H__

typedef void (*thread_func_t)(void *arg);

typedef struct tpool_work {
    thread_func_t func;
    void* arg;
    struct tpool_work* next;
} tpool_work_t;

typedef struct tpool {
    tpool_work_t* first;
    tpool_work_t* last;
    pthread_mutex_t work_mutex;
    pthread_cond_t has_work;
    pthread_cond_t no_working_threads;
    size_t working_thread_count;
    size_t alive_thread_count;
    bool stop;
} tpool_t;


tpool_t* tpool_create(size_t num);
void tpool_destroy(tpool_t* tm);
bool tpool_add_work(tpool_t* tm, thread_func_t func, void* arg);
void tpool_wait(tpool_t* tm);
tpool_work_t* tpool_work_create(thread_func_t func, void* arg);
void tpool_work_destroy(tpool_work_t* work);
tpool_work_t* tpool_work_get(tpool_t* tm);
void* tpool_worker(void* arg);

#endif
