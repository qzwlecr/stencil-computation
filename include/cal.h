#include <stdarg.h>
#include "slave.h"
typedef struct cal_lock{
  int req, cur;
} cal_lock_t;
__attribute__((weak)) int cal_req_id = 0, cal_cur_id = 0;
static void cal_global_lock(){
  int req, cur;
  asm ("faaw %0, 0(%1)\n\t" : "=r"(req): "r"(&cal_req_id));
  asm ("ldw %0, 0(%1)\n\t" : "=r"(cur): "r"(&cal_cur_id));
  while (cur != req){
    asm ("ldw %0, 0(%1)\n\t" : "=r"(cur): "r"(&cal_cur_id));
  }
}
static void cal_global_unlock(){
  asm ("faaw $31, 0(%0)\n\t" : : "r"(&cal_cur_id));
}

static void cal_lock(cal_lock_t *lock){
  int req, cur;
  asm ("faaw %0, 0(%1)\n\t" : "=r"(req): "r"(&(lock->req)));
  asm ("ldw %0, 0(%1)\n\t" : "=r"(cur): "r"(&(lock->cur)));
  while (cur != req){
    asm("ldw %0, 0(%1)\n\t" : "=r"(cur): "r"(&(lock->cur)));
  }
}

static void cal_unlock(cal_lock_t *lock){
  asm ("faaw $31, 0(%0)\n\t" : : "r"(&(lock->cur)));
}

static void cal_locked_printf(char *fmt, ...){
  volatile long vsprintf_addr = (long)vsprintf;
  volatile long printf_addr = (long)printf;
  int (*vsprintf_ptr)(char *, const char *, va_list) = (void*)vsprintf_addr;
  int (*printf_ptr)(const char *, ...) = (void*)printf_addr;
  char buf[256];
  va_list va;
  va_start(va, fmt);
  vsprintf_ptr(buf, fmt, va);
  va_end(va);
  cal_global_lock();
  fputs(buf, stdout);
  fflush(stdout);
  cal_global_unlock();
}
