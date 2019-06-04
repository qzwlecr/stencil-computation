#ifndef _SHARE_H     
#define _SHARE_H

#include <stddef.h>
#include <stdio.h>

#if defined(_LDM_BLOCK)
#define LDM_SIZE	(_LDM_BLOCK*1024)
#else
#define LDM_SIZE        0x4000 //16KB
#endif 

#define LDM_SIZE_8      (8*LDM_SIZE)

#define SIGCORE         50
#define MAX_CORES       64
#define MAX_THREADS     64

#define BitGet(mask,num)\
( ((unsigned long)mask >> num) & 1ULL )

#define BitSet(mask,num)\
( mask |= (1ULL << num) )

#define BitClr(mask,num)\
( mask &= ~(1ULL << num) )

#define BitCount(mask) \
({\
	int __count__;\
	asm volatile ("ctpop	%1,%0\n\t"\
		      :"=&r"(__count__)\
		      :"r"(mask)\
		      :"memory"	);\
	__count__;\
}) 


#ifdef QH20150123
#ifdef WANGF20120501
typedef enum{false,true} boolean;
#else
#ifndef __cplusplus
typedef enum{false,true} boolean;
#endif
#endif
#endif

typedef unsigned char           uint8_t;
typedef unsigned int            uint32_t;
typedef unsigned short          uint16_t;
typedef unsigned long           uint64_t;
# ifndef __int8_t_defined
# define __int8_t_defined
typedef int                     int32_t;
typedef short                   int16_t;
typedef long                    int64_t;
#endif 


//===================================
//	Define the internal signo	
//===================================
enum {
        SW3_SIGSYSC = 1,        /*1: host side syscall agent */
        SW3_SIGHALT,            /*2: slave core group halt */
        SW3_SIGEND,             /*3: slave core end */
	SW3_SIGERR,		/*4: slave error report */
	SW3_SIGPRINT = 11,	/*11: for parallel*/
	SW3_SIGEXPT = 62,	/*62: for exception */
        SW3_SIGMAX = 63,        /*63: core signal max number */
};

#ifndef WANGF20110117
//===================================
//	Define the exception signo
//===================================
//===================================
//	Revised at 2012-11-21
//===================================

#define SIGEXPT 55

#ifdef WANGF20121121
enum Exception_Kind_1 {
        UNALIGN = 0,
        ILLEGAL,
        FPE,
        IOV,
        PCOV,
        CLASSE,
        SELLDWE,
        DFLOWE,
        IFLOWE,
        ATOMOV,
        SBMDE,
        SYNE,
        RCE,
        CHANNAL,
        SRSETE,
        IOE,
#ifndef WCH20110530
	OUTRANK,
	MTAG,
#endif
	_SYS_NSIG,
};

enum Ekind_FPE_2 {
        OVI = 0,
        INE,
        OVF,
        UNF,
        DZE,
        INV,
        DNO,
};

enum Ekind_DFLOW_2 {
        RCSRINV = 0,
        LDME,
#ifndef WCH20110530
	OTHERS,
	MEMANS = 5,
	CODC,
	CLASS_ST,
#else
        SDLBE,
        MEMOV,
        NOMEMTARG,
        RESPERR,
#endif
};

enum Ekind_SBMD_2 {
        MATCHE = 0,
#ifndef WCH20110530
	CMBIO,
	CMBCK,
#endif
        OTHER,
};

enum Ekind_SYN_2 {
        NOSELF = 0,
        SWITHNO,
};

enum Ekind_CHANNAL_2 {
        WRCHE = 0,
        DMAE,
        DMAW,
        DMAMEM,
        DMALDM,
#ifndef WCH20110530
	ANSWD,
	CHNO,
	SYNVEC,
#endif
};

typedef struct slave_expt {
        int coreno;//从核核号
        unsigned long expt_vector_1; //从核异常向量1
        unsigned long expt_vector_2; //从核异常向量2
        unsigned long expt_pc;//从核异常pc，如有则会记录
        unsigned long addr;  // 造成从核异常的可能访问的地址
        int tmp_val;
} slave_expt;
#else
enum Exception_Kind {
        UNALIGN = 0,
        ILLEGAL,
        OVI,
	INE,
        UNF,
	OVF,
	DBZ,
        INV,
	DNO,
	CLASSE1,
	CLASSE2,
        SELLDWE,
	LDME,
	SDLBE,
	MABC,
	MATNO,
	RAMAR,
        IFLOWE,
        SBMDE1,
        SBMDE2,
        SYNE1,
        SYNE2,
	RCE,
	DMAE1,
	DMAE2,
	DMAE3,
	DMAE4,
	DMAE5,
        IOE1,
        IOE2,
        IOE3,
	OTHERE,
	_SYS_NSIG,
};

typedef struct slave_expt {
    int coreno;  //记录的从核号
    unsigned long expt_vector;	    //异常向量
    unsigned long expt_pc;	    //异常pc
    unsigned long dma_expt_type;    //dma异常类型
    unsigned long dma0;		    //dma描述符0
    unsigned long dma1;		    //dma描述符1
    unsigned long dma2;		    //dma描述符2
    unsigned long dma3;		    //dma描述符3
    unsigned long tc_sdlb_err_spot; //如果是sdlb异常的化，此位有效
    unsigned long reserve_data;	    //预留的数据位
} slave_expt;
#endif

#endif


//=================================
//      Define the .tdata_* section addr
//===================================
extern long *_tdata_local_start __attribute__ ((weak));
extern long *_tdata_local_end __attribute__ ((weak));
extern long *_tdata_private_start __attribute__ ((weak));
extern long *_tdata_private_end __attribute__ ((weak));
extern long *_tdata_local_fix_end __attribute__ ((weak));

typedef struct s_thread_info{
        char valid;             //the thread info is valid or not
        int thread_id;
        int core_num;
        int state_flag;         //run :1 ; finish:0
        void * pc;
        void * arg;
        char fini_sig;          //finish type
        long gid;
        int team_size;
}thread_info_t;

typedef struct s_core_type {
	unsigned long coremask_fault;    //  
	unsigned long coremask_employ;   //  
	unsigned long coremask_free;     //
	unsigned long  spe_access;       //指示该从核可以被访问
}core_type_t;

typedef struct s_msg{
        unsigned long own;  //占用位，1:主核处理中，0：主核处理完 
        int type;
        char * msg;
        unsigned long msg_size;
}msg_t;

#define CG_NUM 4
extern void slave_start();
#if 0
extern unsigned long __core_fault_mask;
extern unsigned long __core_employ_mask;
extern unsigned long __core_unemploy_mask;
extern unsigned long __core_spawn_mask;
extern unsigned long __id_spawn_mask;
extern int __spawn_tag;
extern int __spawn_thread_num;
extern int __sig_join_tag ;
extern int __sig_end_tag[MAX_CORES];
extern thread_info_t * __v_thread_info;
extern int __max_threads;
extern unsigned long __id_use_mask;
extern unsigned long __core_use_mask;
extern char __id_core_map[MAX_CORES];
extern char __core_id_map[MAX_CORES];
extern thread_info_t * __v_core_thread_map[MAX_CORES];
#else
extern char __id_core_map[4][MAX_CORES];
extern char __core_id_map[4][MAX_CORES];
extern thread_info_t * __v_core_thread_map[4][MAX_CORES];
#endif
#ifndef SHENL20130117
extern volatile unsigned long *__total_tasks;
extern int __cgid;
extern int __cgnum;
#endif 
#endif //_SHARE_H 
