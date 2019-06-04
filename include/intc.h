#ifndef _INTC_H
#define _INTC_H

#define TRUE 1
#define FALSE 0


# define FmtAssert(Cond,ParmList) \
    ( Cond ? (int) 1 : \
    ( printf( "AHTREAD-LIB:Assertion Failed at file:%s line:%d \n::", __FILE__, __LINE__), \
      printf ParmList, \
      printf( "\n"), \
      exit(0) ) )


# define Is_True(Cond,ParmList) \
    ( Cond ? (int) 1 : \
    ( printf( "AHTREAD-LIB:Check Failed at file:%s line:%d \n::", __FILE__, __LINE__), \
      printf ParmList, \
      printf( "\n"),\
      (int) 0 ) )

#define DBG_printf( ParmList )\
    ( printf( "ATHREAD-DBG: %s %d :: ",__FUNCTION__, __LINE__), \
      printf ParmList )


#define  athread_get_core( id ) \
({char __core__;__core__=__id_core_map[id];__core__;})

#define athread_get_id( core )\
({char __id__;__id__=__core_id_map[core];__id__;})

#define HLINE "==================================================================\n"

//#define Is_True FmtAssert
#if 0

#define BitGet(mask,num)\
( ((unsigned long)mask >> num) & 1ULL )

#define BitSet(mask,num)\
( (unsigned long)mask | (1ULL << num) )

#define BitClr(mask,num)\
( (unsigned long)mask & ~(1ULL << num) )

#endif 

#ifdef SHARE_H
typedef struct s_thread_info{
	char valid;  		//the thread info is valid or not
	int thread_id;
	int core_num;
	volatile int state_flag;		//run :1 ; finish:0
	void * pc;
	void * arg;
	char fini_sig;		//finish type
	long gid;
	int team_size;
}thread_info_t;




extern void * slave_start;
extern unsigned long __core_fault_mask;
extern unsigned long __core_employ_mask;
extern unsigned long __core_unemploy_mask;
extern unsigned long __core_spawn_mask;
extern unsigned long __id_spawn_mask;
extern int __spawn_tag;
extern int __spawn_thread_num;
extern int __sig_join_tag ;
extern int __sig_end_tag[MAX_CORES];
extern char __id_core_map[MAX_CORES];
extern char __core_id_map[MAX_CORES];
extern thread_info_t * __v_core_thread_map[MAX_CORES];
extern thread_info_t * __v_thread_info;
extern int __max_threads;
extern unsigned long __id_use_mask;
extern unsigned long __core_use_mask;

#endif 

typedef void handler_t(int);
void (*sw3_sig_handler[SW3_SIGMAX+1])();
extern void *Signal(int signum, void * handler);
extern void __sig_handler(int sig, siginfo_t *sinfo, struct sigcontext *sigcontext);
extern void __halt_handler(int sig, siginfo_t *sinfo, struct sigcontext *sigcontext);
extern void __end_handler(int sig, siginfo_t *sinfo, struct sigcontext *sigcontext);
extern void __err_handler(int sig, siginfo_t *sinfo, struct sigcontext *sigcontext);
extern void __sysc_handler(int sig, siginfo_t *sinfo, struct sigcontext *sigcontext);

#ifndef WANGF20110117
void (*sw3_exp_handler[_SYS_NSIG])();
extern void __expt_handler(int sig, siginfo_t *sinfo, struct sigcontext *sigcontext);
#endif

#endif //_INTC_H
