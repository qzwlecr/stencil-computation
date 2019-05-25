#ifndef _SLAVE_H
#define _SLAVE_H
#include <signal.h>
//#include <bits/types.h>
#include "share.h"
#include "slave_intc.h"
#include "slave_sig.h"

#ifdef _SW_COMPILER_VERSION
#define  __thread  __attribute__ ((section (".tdata_private")))          
#define  __thread_local  __attribute__ ((section (".tdata_local"))) 
#define  __thread_local_fix  __attribute__ ((section (".tdata_local_fix")))
#define  __thread_kernel(kname)  __attribute__ ((section ( ".tdata_kernel_"kname))) 
#else
#define  __thread_local __thread  __attribute__ ((section (".tdata_local")))
#define  __thread_local_fix __thread  __attribute__ ((section (".tdata_local_fix")))
#endif

#ifndef XIAOQ20141229
#define  __thread_mix  __attribute__ ((section (".tdata_private_mix"))) 
#endif

#define PRINT(x,y)\
{\
        asm volatile ("bis	%0,%1,$31\n\t"\
                        ::"r"(x),"r"(y):"memory"\
                        );\
}

#define RPCC(x)\
{asm volatile("rcsr %0,4":"=r"(x));}

#define RTC()\
({unsigned long __time__; asm volatile("rcsr	%0,4":"=r"(__time__)::"memory");__time__;})
#if 0
#define updt_addw_i(_n_, _addr_) \
{                                                               \
            unsigned long __value__;                                \
            asm volatile("ldi    %0, %1($31)\n\t"                   \
		                            "vshuffle       $31, %0, 0x01, %0\n\t"  \
		                            "ldi    %0, 4(%0)\n\t"                  \
		                            "updt   %0, 0(%2)\n\t"                  \
		                            :"=r"(__value__)                        \
		                            :"n"(_n_),"r"(_addr_):"memory");        \
}
#define updt_subw_i(_n_, _addr_) \
{                                                               \
            unsigned long __value__;                                \
            asm volatile("ldi    %0, %1($31)\n\t"                   \
		                            "vshuffle       $31, %0, 0x01, %0\n\t"  \
		                            "ldi    %0, 6(%0)\n\t"                  \
		                            "updt   %0, 0(%2)\n\t"                  \
		                            :"=r"(__value__)                        \
		                            :"n"(_n_),"r"(_addr_):"memory");        \
}

#endif 
#define updt_addw(_n_, _addr_) \
{           unsigned long __tmp__;                                                       \
            asm volatile(                   "vshff $31, %1, 0x01, %1\n\t"  \
                                            "ldi    %0, 4(%1)\n\t"                  \
                                            "updt   %0, 0(%2)\n\t"                  \
                                            :"=r"(__tmp__):"r"(_n_),"r"(_addr_):"memory");        \
}
#define updt_subw(_n_, _addr_) \
{           unsigned long __tmp__;                                                       \
            asm volatile(                   "vshff $31, %1, 0x01, %1\n\t"  \
                                            "ldi    %0, 6(%1)\n\t"                  \
                                            "updt   %0, 0(%2)\n\t"                  \
                                            :"=r"(__tmp__):"r"(_n_),"r"(_addr_):"memory");        \
}

#define updt_addw_i	updt_addw
#define updt_subw_i	updt_subw

#ifndef SHENL20130117
#define atomic_inc1_w(addr)\
        ({unsigned int __res__;asm volatile ("faaw %0, 0(%1)":"=r"(__res__):"r"(addr):"memory");__res__;})

#define atomic_inc1_l(addr)\
        ({unsigned long __res__;asm volatile ("faal %0, 0(%1)":"=r"(__res__):"r"(addr):"memory");__res__;})

#endif


#if 1
typedef enum {
        PE_MODE,
        BCAST_MODE,
        ROW_MODE,
        BROW_MODE,
        RANK_MODE
} dma_mode;

typedef enum {
        DMA_PUT,
        DMA_GET,
        DMA_PUT_P,
        DMA_GET_P,
        DMA_BARRIER = 5
} DMA_OP;

typedef enum {
	ROW_SCOPE,
	COL_SCOPE,
	ARRAY_SCOPE,
}scope;
#endif

#if 0
#if !defined _STRUCT_TIMEVAL
typedef long int time_t;
typedef long int suseconds_t;
struct timeval {
        time_t      tv_sec;      /* seconds */
        suseconds_t tv_usec;     /* microseconds */
};
#define _STRUCT_TIMEVAL 1
#endif
#endif

extern int athread_get(dma_mode mode, void *src, void *dest, int len, void *reply, char mask,int stride,int bsize);
extern int athread_put(dma_mode mode, void *src, void *dest, int len, void *reply, int stride,int bsize);
//extern int athread_sigqueue(int cgn,int signo,const union sigval value);
extern int athread_get_core( int id);
extern int athread_get_id(int core);
extern int athread_syn(scope scp,int mask);
#ifdef WANGF20111204
extern __thread_local char  _CGN,_ROW,_COL,_PEN;
extern __thread_local volatile void (*_PC)();
extern __thread_local int   _MYID;
#else
extern __thread_local_fix  char  _CGN,_ROW,_COL,_PEN;
extern __thread_local_fix volatile void (*_PC)();
extern __thread_local_fix  int   _MYID;
#endif
#endif //_SLAVE_H
