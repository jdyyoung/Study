/* Copyright © 2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */


#ifndef _MUTEX_H_
#define _MUTEX_H_

typedef struct {
    char             head[8];
    struct list_head list;
    pthread_mutex_t  mutex;
    char*            name;
    char*            func;   // function that has the mutex locked (NULL if not locked)
    int              line;   // line# that has the mutex locked
    time_t           locked; // time when the mutex was locked
    char             tail[8];
} MUTEX;

#define MUTEX_LOCK(mutex)       mutex_lock(mutex, __FUNCTION__, __LINE__)
#define MUTEX_TRYLOCK(mutex)    mutex_trylock(mutex, __FUNCTION__, __LINE__)
#define MUTEX_UNLOCK(mutex)     mutex_unlock(mutex, __FUNCTION__, __LINE__)
#define MUTEX_INIT(mutex, name) mutex_init(mutex, name, __FUNCTION__, __LINE__)
#define MUTEX_DESTROY(mutex)    mutex_destroy(mutex, __FUNCTION__, __LINE__)

extern void  mutex_lock(MUTEX* mutex, const char* func, int line);
extern int   mutex_trylock(MUTEX* mutex, const char* func, int line);
extern void  mutex_unlock(MUTEX* mutex, const char* func, int line);
extern void  mutex_init(MUTEX* mutex, const char* name, const char* func, int line);
extern void  mutex_destroy(MUTEX* mutex, const char* func, int line);
extern void  mutex_deadlock_check(void);
extern void  mutex_dump(void);
extern void  mutex_initialize(void);

#endif
