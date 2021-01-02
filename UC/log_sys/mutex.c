/* Copyright © 2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */

#include "lib.h"


#define LOCK_MINUTE_THRESHOLD       8
#define DEADLOCK_MINUTE_THRESHOLD   60


static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct list_head mutex_list = LIST_HEAD_INIT(mutex_list);


static BOOL mutex_deadlock_exception(MUTEX* mutex)
{
    return mutex->func != NULL && (strcmp(mutex->func, "thread_loop") == 0 || 
                                   strcmp(mutex->func, "queue_get") == 0   || 
                                   strcmp(mutex->func, "ui_lock") == 0);
}


static void mutex_sanity(MUTEX* mutex, const char* func, int line)
{
   if (!mutex) {
        printf( "INVALID MUTEX-%p, func='%s', line=%d", mutex, func, line);
        return;
   }

   if (memcmp(mutex->head, "MHEAD", 6) != 0 ||
       memcmp(mutex->tail, "MTAIL", 6) != 0) {
        printf( "INVALID MUTEX-%p, %s().%d, [%02x %02x %02x %02x][%02x %02x %02x %02x]",
            mutex, func, line, 
            mutex->head[0], mutex->head[1], mutex->head[2], mutex->head[3],
            mutex->tail[0], mutex->tail[1], mutex->tail[2], mutex->tail[3]);
   } else {
        time_t now = monotonic_time();
        if (mutex->locked && /*rtc_time_valid() &&*/ now - mutex->locked > LOCK_MINUTE_THRESHOLD*60) {
            if (!mutex_deadlock_exception(mutex)) {
                printf( "MUTEX-%p locked by %s().%d for more than %d minutes (%d - %d = %d)\n", 
                     mutex, mutex->func? mutex->func:"<NULL>", mutex->line, LOCK_MINUTE_THRESHOLD, now, mutex->locked, now - mutex->locked);
            }
            mutex->locked = now;
        }
    }
}


void mutex_lock(MUTEX* mutex, const char* func, int line)
{
    mutex_sanity(mutex, func, line);
    if (pthread_mutex_lock(&mutex->mutex) < 0)
        printf( "func='%s', line=%d", func, line);
    else {
        mutex->func = (char*)func;
        mutex->line = line;
        mutex->locked = monotonic_time();
    }
}


int mutex_trylock(MUTEX* mutex, const char* func, int line)
{
    int rc;

    mutex_sanity(mutex, func, line);
    rc = pthread_mutex_trylock(&mutex->mutex);
    if (rc == 0) {
        mutex->func = (char*)func;
        mutex->line = line;
        mutex->locked = monotonic_time();
    }
    return rc; 
}


void mutex_unlock(MUTEX* mutex, const char* func, int line)
{
    mutex_sanity(mutex, func, line);
    if (pthread_mutex_unlock(&mutex->mutex) < 0)
        printf( "func='%s', line=%d", func, line);
    else {
        mutex->line = line > 0? -line : -1;
        mutex->func = (char*)func;
        mutex->locked = 0;
    }
}


void mutex_init(MUTEX* mutex, const char* name, const char* func, int line)
{ 
    if (pthread_mutex_init(&mutex->mutex, NULL) != 0) {
        printf( "func='%s', line=%d", func, line);
        return;
    }
    memcpy(mutex->head, "MHEAD", 6);
    memcpy(mutex->tail, "MTAIL", 6);
    mutex->name = (char*)name;
    mutex->func = NULL;
    mutex->line = 0;
    mutex->locked = 0;

    pthread_mutex_lock(&list_mutex);
    list_add_tail(&mutex->list, &mutex_list);
    pthread_mutex_unlock(&list_mutex);
}


void mutex_destroy(MUTEX* mutex, const char* func, int line)
{
    mutex_sanity(mutex, func, line);
    if (pthread_mutex_destroy(&mutex->mutex) < 0) {
        printf( "func='%s', line=%d", func, line);
    } else {
        memcpy(mutex->head, "mhead", 6);
        memcpy(mutex->tail, "mtail", 6);
        mutex->locked = 0;

        pthread_mutex_lock(&list_mutex);
        list_del(&mutex->list);
        pthread_mutex_unlock(&list_mutex);
    }
}

BOOL rtc_time_valid(void)
{
    //return network_ready && synced;
	return FALSE;
}

void mutex_deadlock_check(void)
{
    if (rtc_time_valid()) {
        struct list_head * this; 
        time_t now = monotonic_time();
        int dead = 0;

        pthread_mutex_lock(&list_mutex);
        list_for_each(this, &mutex_list) {
            MUTEX* mutex = list_entry(this, MUTEX, list);
            if (!rtc_time_valid()) break;
            if (mutex->locked && now - mutex->locked > DEADLOCK_MINUTE_THRESHOLD * 60) {
                if (!mutex_deadlock_exception(mutex)) { // exceptions that can block forever...
                    printf( "MUTEX-%p locked by %s().%d for more than %d minutes (%d - %d = %d)\n", 
                         mutex, mutex->func? mutex->func:"<NULL>", mutex->line, DEADLOCK_MINUTE_THRESHOLD, now, mutex->locked, now - mutex->locked);
                    ++dead;
                }
                mutex->locked = now;
            }
        }
        pthread_mutex_unlock(&list_mutex);

        #if 0
        // thread function can block for a long time, so we don't want to reboot until we solve that issue... TODO
        if (rtc_time_valid() && dead) {
            printf( "%d MUTEXES LOCKED FOR MORE THAN AN HOUR!", dead);
        }
        #endif
    }
}


void mutex_dump(void)
{
    struct list_head * this; 
    time_t now = monotonic_time();

    pthread_mutex_lock(&list_mutex);
    list_for_each(this, &mutex_list) {
        MUTEX* mutex = list_entry(this, MUTEX, list);
        if (mutex->func != NULL && mutex->line > 0) {
            printf( "MUTEX-%s(%p) locked by %s().%d for %d seconds\n", 
                 mutex->name, mutex, mutex->func, mutex->line, now - mutex->locked);
        }
    }
    pthread_mutex_unlock(&list_mutex);
}


void mutex_initialize(void)
{
}
