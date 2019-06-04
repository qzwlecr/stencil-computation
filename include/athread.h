#ifndef _ATHREAD_H
#define _ATHREAD_H

#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "share.h"
#include "intc.h"

#include "m_share.h"

#define SLAVE_FUN(x)        slave_##x
#define athread_spawn(y,z) __real_athread_spawn(slave_##y,z)
//#define athread_spawn64(y,z) athread_spawn64(slave_##y,z)
#define athread_spawn_task(y,z) __real_athread_spawn(((unsigned long)slave_##y|0x1ULL),z)
#define athread_spawn_group(x,y,z) __real_athread_spawn_group(x,slave_##y,z)
#define athread_create(x,y,z) __real_athread_create(x,slave_##y,z)
#ifndef XIAOQ20140307
int sys_m_en(int cgid, int en_no, void* dt);
#define EN_GET_USER_MODE        2
typedef struct en_get_user_mod{
        int user_mode;
}en_get_user_mode_t;      
unsigned long START_ADDR; 
#endif
typedef void * (*start_routine)(void *);

/* master core athread interface */
extern int athread_init(void);

extern int __real_athread_create(int id, void* fpc,void *arg);
extern int athread_wait(int id);
extern int athread_end(int id);

extern int __real_athread_spawn(void* fpc,void *arg);
extern int athread_join(void);
extern int athread_halt(void);

extern int __real_athread_spawn_group(unsigned long gmask,void* fpc,void *arg);
extern int athread_join_group(unsigned long gmask);
extern int athread_try_join_group(unsigned long gmask);
extern unsigned long athread_idle_group();


extern int athread_task(void* fpc,void *arg);
extern void athread_master_sync();

//extern int athread_get_core(int id);
extern int athread_get_max_threads(void);
extern int athread_set_num_threads(int num_threads);
extern int athread_get_num_threads(void);

extern int athread_cancel(int id);
extern int athread_signal(int signo,void * fpc);

#ifndef XIAOQ201406
extern void* get_tdata_slave_addr(int cgid, void* addr);
#endif
#ifndef WANGF20110117
extern int athread_expt_signal(int sig, void *fpc);
#endif
#ifndef XIAOQ20120112
extern void *malloc_cross (size_t __size) __attribute__ ((__malloc__));
#endif
#ifndef WANGF20110618

#define h2ldm(element, penum, cgn) *(typeof(element) *)(((unsigned long )0x8002000000 | ((unsigned long)cgn << 36) | (penum<<16) )+(long)(&element)-(long)(&_tdata_local_start) + *((long *)((long)&_tdata_local_start-8)))

#define IO_addr(element, penum, cgn) (unsigned long )(typeof(element) *)(((unsigned long )0x8002000000 | ((unsigned long)cgn << 36) | (penum<<16) )+(long)(&element)-(long)(&_tdata_local_start) + *((long *)((long)&_tdata_local_start-8)))

#endif

#ifndef XIAOQ20141229
#define __thread_mix __thread
#endif

#endif //_ATHREAD_H 
