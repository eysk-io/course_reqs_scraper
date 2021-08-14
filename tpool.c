#include <stdlib.h> 
#include <stdbool.h> 
#include <pthread.h>
#include "tpool.h"

tpool_t* tpool_create(size_t num) {
    tpool_t* tm;
    pthread_t thread;
    size_t i;

    if (num == 0) num = 2;

    tm = calloc(1, sizeof(*tm));
    tm->alive_thread_count = num;

    pthread_mutex_init(&(tm->work_mutex), NULL);
    pthread_cond_init(&(tm->has_work), NULL);
    pthread_cond_init(&(tm->no_working_threads), NULL);

    tm->first = NULL;
    tm->last  = NULL;

    for(i = 0; i < num; i++) {
        pthread_create(&thread, NULL, tpool_worker, tm);
        pthread_detach(thread);
    }

    return tm;
}

void tpool_destroy(tpool_t* tm) {
    tpool_work_t* work;
    tpool_work_t* work2;

    if(tm == NULL) return;

    pthread_mutex_lock(&(tm->work_mutex));
    work = tm->first;
    while(work != NULL) {
        work2 = work->next;
        tpool_work_destroy(work);
        work = work2;
    }
    tm->stop = true;
    pthread_cond_broadcast(&(tm->has_work));
    pthread_mutex_unlock(&(tm->work_mutex));

    tpool_wait(tm);

    pthread_mutex_destroy(&(tm->work_mutex));
    pthread_cond_destroy(&(tm->has_work));
    pthread_cond_destroy(&(tm->no_working_threads));

    free(tm);
}

bool tpool_add_work(tpool_t* tm, thread_func_t func, void* arg) {
    tpool_work_t* work;

    if(tm == NULL)
        return false;

    work = tpool_work_create(func, arg);
    if(work == NULL) return false;

    pthread_mutex_lock(&(tm->work_mutex));
    if(tm->first == NULL) {
        tm->first = work;
        tm->last = tm->first;
    } else {
        tm->last->next = work;
        tm->last = work;
    }

    pthread_cond_broadcast(&(tm->has_work));
    pthread_mutex_unlock(&(tm->work_mutex));

    return true;
}

void tpool_wait(tpool_t* tm) {
    if(tm == NULL) return;

    pthread_mutex_lock(&(tm->work_mutex));
    while(1) {
        if(
            (!tm->stop && tm->working_thread_count != 0) || 
            (tm->stop && tm->alive_thread_count != 0)
        ) {
            pthread_cond_wait(&(tm->no_working_threads), &(tm->work_mutex));
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(tm->work_mutex));
}

tpool_work_t* tpool_work_create(thread_func_t func, void* arg) {
    tpool_work_t* work;
    if (func == NULL) return NULL;
    
    work = malloc(sizeof(*work));
    work->func = func;
    work->arg = arg;
    work->next = NULL;
    return work;
}

void tpool_work_destroy(tpool_work_t* work) {
    if (work == NULL) return;
    free(work);
}

tpool_work_t* tpool_work_get(tpool_t* tm) {
    tpool_work_t* work;
    if (tm == NULL) return NULL;

    work = tm->first;
    if (work == NULL) return NULL;
    
    if (work->next == NULL) {
        tm->first = NULL;
        tm->last = NULL;
    } else {
        tm->first = work->next;
    }

    return work;
}

void* tpool_worker(void* arg) {
    tpool_t* tm = arg;
    tpool_work_t* work;

    while(1) {
        pthread_mutex_lock(&(tm->work_mutex));

        while(tm->first == NULL && !tm->stop)
            pthread_cond_wait(&(tm->has_work), &(tm->work_mutex));

        if(tm->stop) break;

        work = tpool_work_get(tm);
        tm->working_thread_count++;
        pthread_mutex_unlock(&(tm->work_mutex));

        if(work != NULL) {
            work->func(work->arg);
            tpool_work_destroy(work);
        }

        pthread_mutex_lock(&(tm->work_mutex));
        tm->working_thread_count--;
        if(
            !tm->stop && tm->working_thread_count == 0 &&
            tm->first == NULL
        ) {
            pthread_cond_signal(&(tm->no_working_threads));
        }
        pthread_mutex_unlock(&(tm->work_mutex));
    }

    tm->alive_thread_count--;
    pthread_cond_signal(&(tm->no_working_threads));
    pthread_mutex_unlock(&(tm->work_mutex));
    return NULL;
}
