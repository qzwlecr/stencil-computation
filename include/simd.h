#ifndef _SIMD_H_INCLUDED
#define _SIMD_H_INCLUDED

/********************************************************************************/
/*         Declare the HOST AND SLAVE TARGET THE SAME SIMD operations           */
/********************************************************************************/
#include<stdio.h>

typedef int intv8 __attribute__ ((__mode__(__V8SI__)));
typedef unsigned uintv8 __attribute__ ((__mode__(__V8SI__)));
typedef int int256 __attribute__ ((__mode__(__V1OI__)));
typedef unsigned uint256 __attribute__ ((__mode__(__V1OI__)));
typedef float floatv4 __attribute__ ((__mode__(__V4SF__)));
typedef double doublev4 __attribute__ ((__mode__(__V4DF__)));

static __inline void
simd_fprint_intv8 (FILE *fp, intv8 a)
{
  union {
    int __a[8];
    intv8 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %d, %d, %d, %d, %d, %d, %d, %d ]\n", __u.__a[7], __u.__a[6], __u.__a[5], __u.__a[4], __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void
simd_fprint_uintv8 (FILE *fp, uintv8 a)
{
  union {
    unsigned int __a[8];
    uintv8 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %u, %u, %u, %u, %u, %u, %u, %u ]\n", __u.__a[7], __u.__a[6], __u.__a[5], __u.__a[4], __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

/*test.c:intv8 va; int256 vb; vb=va; simd_print_int256(vb);
 *-O3 will generate: vstd $f10,16($15)
 *so add volatile
 */
static __inline void
simd_fprint_int256 (FILE *fp, int256 a)
{
  volatile union {
    long __a[4];
    int256 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ 0x%lx, 0x%lx, 0x%lx, 0x%lx ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void
simd_fprint_uint256 (FILE *fp, uint256 a)
{
  volatile union {
    unsigned long __a[4];
    uint256 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ 0x%lx, 0x%lx, 0x%lx, 0x%lx ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void
simd_fprint_doublev4 (FILE *fp, doublev4 a)
{
  union {
    double __a[4];
    doublev4 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %e, %e, %e, %e ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

/* test.c:floatv4 va; doublev4 vb; va=vb; simd_print_floatv4(va);
 * -O3 will error in reload:
 *  (insn 58 8 53 2 (set (reg:V4DF 4 $4 [85])
 *          (reg:V4DF 42 $f10)) 4.c:9 391 {*movv4df}
 *               (nil))
 */
static __inline void
simd_fprint_floatv4 (FILE *fp, floatv4 a)
{
  volatile union {
    float __a[4];
    floatv4 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %e, %e, %e, %e ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline intv8
simd_set_intv8 (int __S, int __T, int __U, int __V, int __W, int __X, int __Y, int __Z)
{
    union {
    int __a[8] __attribute__ ((aligned (32)));
    intv8 __v;
  } __u;

  __u.__a[0] = __S;
  __u.__a[1] = __T;
  __u.__a[2] = __U;
  __u.__a[3] = __V;
  __u.__a[4] = __W;
  __u.__a[5] = __X;
  __u.__a[6] = __Y;
  __u.__a[7] = __Z;

  return __u.__v;
}

static __inline uintv8
simd_set_uintv8 (unsigned int __S, unsigned int __T, unsigned int __U, unsigned int __V, unsigned int __W, unsigned int __X, unsigned int __Y, unsigned int __Z)
{
    union {
    unsigned int __a[8] __attribute__ ((aligned (32)));
    uintv8 __v;
  } __u;

  __u.__a[0] = __S;
  __u.__a[1] = __T;
  __u.__a[2] = __U;
  __u.__a[3] = __V;
  __u.__a[4] = __W;
  __u.__a[5] = __X;
  __u.__a[6] = __Y;
  __u.__a[7] = __Z;

  return __u.__v;
}

static __inline int256
simd_set_int256 (long __W, long __X, long __Y, long __Z)
{
    union {
    long __a[4];
    int256 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;
  __u.__a[2] = __Y;
  __u.__a[3] = __Z;

  return __u.__v;
}

static __inline uint256
simd_set_uint256 (unsigned long __W, unsigned long __X, unsigned long __Y, unsigned long __Z)
{
    union {
    unsigned long __a[4];
    uint256 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;
  __u.__a[2] = __Y;
  __u.__a[3] = __Z;

  return __u.__v;
}

static __inline floatv4
simd_set_floatv4 (float __W, float __X, float __Y, float __Z)
{
    union {
    float __a[4] __attribute__ ((aligned (32)));
    floatv4 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;
  __u.__a[2] = __Y;
  __u.__a[3] = __Z;

  return __u.__v;
}

static __inline doublev4
simd_set_doublev4 (double __W, double __X, double __Y, double __Z)
{
    union {
    double __a[4] __attribute__ ((aligned (32)));
    doublev4 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;
  __u.__a[2] = __Y;
  __u.__a[3] = __Z;

  return __u.__v;
}

static __inline void simd_print_intv8 (intv8 a) { simd_fprint_intv8 (stdout, a); }
static __inline void simd_print_uintv8 (uintv8 a) { simd_fprint_uintv8 (stdout, a); }
static __inline void simd_print_int256 (int256 a) { simd_fprint_int256 (stdout, a); }
static __inline void simd_print_uint256 (uint256 a) { simd_fprint_uint256 (stdout, a); }
static __inline void simd_print_floatv4 (floatv4 a) { simd_fprint_floatv4 (stdout, a); }
static __inline void simd_print_doublev4 (doublev4 a) { simd_fprint_doublev4 (stdout, a); }



#define simd_load(dest,src) 				\
  ({						\
	(dest) = __builtin_sw_load ((src));	\
	(dest);				\
  })

//#define simd_loadl(dest,src)				\
  ({						\
	(dest) = __builtin_sw_loadul ((src));	\
	(dest);                                    \
  })

//#define simd_loadh(dest,src)				\
  ({						\
	(dest) = __builtin_sw_loaduh ((src));	\
	(dest);                                    \
  })

#define simd_loadu(dest,src)				\
  ({						\
	(dest) = __builtin_sw_loadu ((src));	\
	(dest);                                    \
  })

#define simd_loade(dest,src)				\
  ({						\
	(dest) = __builtin_sw_loade ((src));	\
	(dest);					\
  })

#define simd_store(src,dest) __builtin_sw_store (dest, src)
#define simd_storeu(src,dest) __builtin_sw_storeu (dest, src)
//#define simd_storeul(src,dest) __builtin_sw_storeul (dest, src)
//#define simd_storeuh(src,dest) __builtin_sw_storeuh (dest, src)

static __inline intv8 __attribute__((__always_inline__))
simd_vsrlw (intv8 __A, int __B) 
{
        return (intv8)  (__A >> __B);
}
//#define simd_vsrlwi(x,y) x>>y
static __inline intv8 __attribute__((__always_inline__))
simd_vsrlwi (intv8 __A, const int __B) 
{
        return (intv8)  (__A >> __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsraw (intv8 __A, int __B) 
{
        return (intv8)  (__A >> __B);
}
//#define simd_vsrawi(x,y) x>>y
static __inline intv8 __attribute__((__always_inline__))
simd_vsrawi (intv8 __A, const int __B) 
{
        return (intv8)  (__A >> __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vaddw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A + __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsubw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A - __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsllw (intv8 __A, int __B) 
{
        return (intv8)  (__A << __B);
}
//#define simd_vsllwi(x,y) x<<y
static __inline intv8 __attribute__((__always_inline__))
simd_vsllwi (intv8 __A, const int __B) 
{
        return (intv8)  (__A << __B);
}

static __inline int256 __attribute__((__always_inline__))
simd_vaddl (int256 __A, int256 __B)
{
	return (int256)  (__A + __B);
}

static __inline int256 __attribute__((__always_inline__))
simd_vsubl (int256 __A, int256 __B)
{
	return (int256)  (__A - __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vandw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A & __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vbicw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A &~ __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vbisw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A | __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vornotw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A |~ __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vxorw (intv8 __A, intv8 __B)
{
	return (intv8)  (__A ^ __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_veqvw (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw_veqvw(__A, __B);
}

static __inline int256 __attribute__((__always_inline__))
simd_sllow (int256 __A, int __B) 
{
        return (int256)  (__A<<__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_sllowi (int256 __A, const int __B) 
{
        return (int256)  (__A<<__B);
}

static __inline int256 __attribute__((__always_inline__))
simd_srlow (int256 __A, int __B) 
{
        return (int256)  (__A>>__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_srlowi (int256 __A, const int __B) 
{
        return (int256)  (__A>>__B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vadds (floatv4 __A, floatv4 __B)
{
	return (floatv4)(__A + __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vsubs (floatv4 __A, floatv4 __B)
{
	return (floatv4)(__A - __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vmuls (floatv4 __A, floatv4 __B)
{
	return (floatv4)(__A * __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vdivs (floatv4 __A, floatv4 __B)
{
	return (floatv4)(__A / __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vaddd (doublev4 __A, doublev4 __B)
{
	return (doublev4)(__A + __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vsubd (doublev4 __A, doublev4 __B)
{
	return (doublev4)(__A - __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vmuld (doublev4 __A, doublev4 __B)
{
	return (doublev4)(__A * __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vdivd (doublev4 __A, doublev4 __B)
{
	return (doublev4)(__A / __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vsqrts (floatv4 __x)
{
	return (floatv4) __builtin_sw_vsqrts (__x);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vsqrtd (doublev4 __x)
{
	return (doublev4) __builtin_sw_vsqrtd (__x);
}

#define simd_vfcmpeq(x,y) __builtin_sw_vfcmpeq(x,y) 

#define simd_vfcmple(x,y) __builtin_sw_vfcmple(x,y) 

#define simd_vfcmplt(x,y) __builtin_sw_vfcmplt(x,y) 

#define simd_vfcmpun(x,y) __builtin_sw_vfcmpun(x,y) 

static __inline floatv4 __attribute__((__always_inline__))
simd_vfcmpeqs (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vfcmpeqs(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vfcmples (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vfcmples(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vfcmplts (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vfcmplts(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vfcmpuns (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vfcmpuns(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vfcmpeqd (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vfcmpeqd(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vfcmpled (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vfcmpled(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vfcmpltd (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vfcmpltd(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vfcmpund (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vfcmpund(__A, __B);
}

#define simd_vcpys(x,y)  __builtin_sw_vcpys(x,y) 

#define simd_vcpyse(x,y)  __builtin_sw_vcpyse(x,y) 

#define simd_vcpysn(x,y)  __builtin_sw_vcpysn(x,y) 

static __inline floatv4 __attribute__((__always_inline__))
simd_vcpyss (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vcpyss(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vcpysns (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vcpysns(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vcpyses (floatv4 __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vcpyses(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vcpysd (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vcpysd(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vcpysnd (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vcpysnd(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vcpysed (doublev4 __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vcpysed(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vmas (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vmas(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vmss (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vmss(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vnmas (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vnmas(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vnmss (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vnmss(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vmad (doublev4 __A, doublev4 __B, doublev4 __C)
{
        return (doublev4)__builtin_sw_vmad(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vmsd (doublev4 __A, doublev4 __B, doublev4 __C)
{
        return (doublev4)__builtin_sw_vmsd(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vnmad (doublev4 __A, doublev4 __B, doublev4 __C)
{
        return (doublev4)__builtin_sw_vnmad(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vnmsd (doublev4 __A, doublev4 __B, doublev4 __C)
{
        return (doublev4)__builtin_sw_vnmsd(__A, __B, __C);
}

#define simd_vinsf(x,y,z) __builtin_sw_vinsf(x,y,z) 
#define simd_vinsf0(x,y) __builtin_sw_vinsf(x,y,0) 
#define simd_vinsf1(x,y) __builtin_sw_vinsf(x,y,1) 
#define simd_vinsf2(x,y) __builtin_sw_vinsf(x,y,2) 
#define simd_vinsf3(x,y) __builtin_sw_vinsf(x,y,3) 

static __inline floatv4 __attribute__((__always_inline__))
simd_vinsfs (float __A, floatv4 __B, const int __C) 
{
        return (floatv4)__builtin_sw_vinsfs(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vinsfs0 (float __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vinsfs(__A, __B, 0);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vinsfs1 (float __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vinsfs(__A, __B, 1);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vinsfs2 (float __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vinsfs(__A, __B, 2);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vinsfs3 (float __A, floatv4 __B) 
{
        return (floatv4)__builtin_sw_vinsfs(__A, __B, 3);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vinsfd (double __A, doublev4 __B, const int __C) 
{
        return (doublev4)__builtin_sw_vinsfd(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vinsfd0 (double __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vinsfd(__A, __B, 0);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vinsfd1 (double __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vinsfd(__A, __B, 1);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vinsfd2 (double __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vinsfd(__A, __B, 2);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vinsfd3 (double __A, doublev4 __B) 
{
        return (doublev4)__builtin_sw_vinsfd(__A, __B, 3);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw (int __A, intv8 __B, const int __C) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, __C);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw0 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 0);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw1 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 1);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw2 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 2);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw3 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 3);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw4 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 4);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw5 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 5);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw6 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 6);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vinsw7 (int __A, intv8 __B) 
{
        return (intv8)__builtin_sw_vinsw(__A, __B, 7);
}

#define simd_vextf(x,y) __builtin_sw_vextf(x,y) 
#define simd_vextf0(x) __builtin_sw_vextf(x,0) 
#define simd_vextf1(x) __builtin_sw_vextf(x,1) 
#define simd_vextf2(x) __builtin_sw_vextf(x,2) 
#define simd_vextf3(x) __builtin_sw_vextf(x,3) 

static __inline float __attribute__((__always_inline__))
simd_vextfs (floatv4 __A, const int __B) 
{
        return (float)__builtin_sw_vextfs(__A, __B);
}

static __inline float __attribute__((__always_inline__))
simd_vextfs0 (floatv4 __A) 
{
        return (float)__builtin_sw_vextfs(__A, 0);
}

static __inline float __attribute__((__always_inline__))
simd_vextfs1 (floatv4 __A) 
{
        return (float)__builtin_sw_vextfs(__A, 1);
}

static __inline float __attribute__((__always_inline__))
simd_vextfs2 (floatv4 __A) 
{
        return (float)__builtin_sw_vextfs(__A, 2);
}

static __inline float __attribute__((__always_inline__))
simd_vextfs3 (floatv4 __A) 
{
        return (float)__builtin_sw_vextfs(__A, 3);
}

static __inline double __attribute__((__always_inline__))
simd_vextfd (doublev4 __A, const int __B) 
{
        return (double)__builtin_sw_vextfd(__A, __B);
}

static __inline double __attribute__((__always_inline__))
simd_vextfd0 (doublev4 __A) 
{
        return (double)__builtin_sw_vextfd(__A, 0);
}

static __inline double __attribute__((__always_inline__))
simd_vextfd1 (doublev4 __A) 
{
        return (double)__builtin_sw_vextfd(__A, 1);
}

static __inline double __attribute__((__always_inline__))
simd_vextfd2 (doublev4 __A) 
{
        return (double)__builtin_sw_vextfd(__A, 2);
}

static __inline double __attribute__((__always_inline__))
simd_vextfd3 (doublev4 __A) 
{
        return (double)__builtin_sw_vextfd(__A, 3);
}

static __inline int __attribute__((__always_inline__))
simd_vextw (intv8 __A, const int __B) 
{
        return (int)__builtin_sw_vextw(__A, __B);
}

static __inline int __attribute__((__always_inline__))
simd_vextw0 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 0);
}

static __inline int __attribute__((__always_inline__))
simd_vextw1 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 1);
}

static __inline int __attribute__((__always_inline__))
simd_vextw2 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 2);
}

static __inline int __attribute__((__always_inline__))
simd_vextw3 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 3);
}

static __inline int __attribute__((__always_inline__))
simd_vextw4 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 4);
}

static __inline int __attribute__((__always_inline__))
simd_vextw5 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 5);
}

static __inline int __attribute__((__always_inline__))
simd_vextw6 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 6);
}

static __inline int __attribute__((__always_inline__))
simd_vextw7 (intv8 __A) 
{
        return (int)__builtin_sw_vextw(__A, 7);
}


//#define simd_vseleq(x,y,z) __builtin_sw_vseleq(x,y,z) 
//#define simd_vselle(x,y,z) __builtin_sw_vselle(x,y,z) 
//#define simd_vsellt(x,y,z) __builtin_sw_vsellt(x,y,z) 

static __inline floatv4 __attribute__((__always_inline__))
simd_vseleqs (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vseleqs(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vselles (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vselles(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vsellts (floatv4 __A, floatv4 __B, floatv4 __C) 
{
        return (floatv4)__builtin_sw_vsellts(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vseleqd (doublev4 __A, doublev4 __B, doublev4 __C) 
{
        return (doublev4)__builtin_sw_vseleqd(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vselled (doublev4 __A, doublev4 __B, doublev4 __C) 
{
        return (doublev4)__builtin_sw_vselled(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vselltd (doublev4 __A, doublev4 __B, doublev4 __C) 
{
        return (doublev4)__builtin_sw_vselltd(__A, __B, __C);
}


static __inline floatv4 __attribute__((__always_inline__))
simd_vbiss (floatv4 __A, floatv4 __B)
{
	return (floatv4)  (__A | __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vbisd (doublev4 __A, doublev4 __B)
{
	return (doublev4)  (__A | __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vslls1 (floatv4 __A) 
{
        return (floatv4)  __builtin_sw_vslls(__A, 1);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vslls2 (floatv4 __A) 
{
        return (floatv4)  __builtin_sw_vslls(__A, 2);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vslls3 (floatv4 __A) 
{
        return (floatv4)  __builtin_sw_vslls(__A, 3);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vslld1 (doublev4 __A) 
{
        return (doublev4)  __builtin_sw_vslld(__A, 1);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vslld2 (doublev4 __A) 
{
        return (doublev4)  __builtin_sw_vslld(__A, 2);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vslld3 (doublev4 __A) 
{
        return (doublev4)  __builtin_sw_vslld(__A, 3);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vsrls1 (floatv4 __A) 
{
        return (floatv4)  __builtin_sw_vsrls(__A, 1);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vsrls2 (floatv4 __A) 
{
        return (floatv4)  __builtin_sw_vsrls(__A, 2);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vsrls3 (floatv4 __A) 
{
        return (floatv4)  __builtin_sw_vsrls(__A, 3);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vsrld1 (doublev4 __A) 
{
        return (doublev4)  __builtin_sw_vsrld(__A, 1);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vsrld2 (doublev4 __A) 
{
        return (doublev4)  __builtin_sw_vsrld(__A, 2);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vsrld3 (doublev4 __A) 
{
        return (doublev4)  __builtin_sw_vsrld(__A, 3);
}

#ifdef _SIMD_HOST

/********************************************************************************/
/*         Declare the HOST TARGET SIMD operations                              */
/********************************************************************************/
static __inline intv8 __attribute__((__always_inline__))
simd_vrolw (intv8 __A, int __B) 
{
        return (intv8)  __builtin_sw_vrotlw(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vrolwi (intv8 __A, const int __B) 
{
        return (intv8)  __builtin_sw_vrotlwi(__A, __B);
}


static __inline int __attribute__((__always_inline__))
simd_vcmpgew (intv8 __A, intv8 __B)
{
        return (int)__builtin_sw_host_vcmpgew(__A, __B);
}
static __inline int __attribute__((__always_inline__))
simd_vcmpgewi (intv8 __A, const int __B)
{
        return (int)__builtin_sw_host_vcmpgewi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmpeqw (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw_host_vcmpeqw(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vcmpeqwi (intv8 __A, const int __B)
{
        return (intv8)__builtin_sw_host_vcmpeqwi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmplew (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw_host_vcmplew(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vcmplewi (intv8 __A, const int __B)
{
        return (intv8)__builtin_sw_host_vcmplewi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmpltw (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw_host_vcmpltw(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vcmpltwi (intv8 __A, const int __B)
{
        return (intv8)__builtin_sw_host_vcmpltwi(__A, __B);
}

static __inline uintv8 __attribute__((__always_inline__))
simd_vcmpulew (uintv8 __A, uintv8 __B)
{
        return (uintv8)__builtin_sw_host_vcmpulew(__A, __B);
}
static __inline uintv8 __attribute__((__always_inline__))
simd_vcmpulewi (uintv8 __A, const unsigned int __B)
{
        return (uintv8)__builtin_sw_host_vcmpulewi(__A, __B);
}

static __inline uintv8 __attribute__((__always_inline__))
simd_vcmpultw (uintv8 __A, uintv8 __B)
{
        return (uintv8)__builtin_sw_host_vcmpultw(__A, __B);
}
static __inline uintv8 __attribute__((__always_inline__))
simd_vcmpultwi (uintv8 __A, const unsigned int __B)
{
        return (uintv8)__builtin_sw_host_vcmpultwi(__A, __B);
}

static __inline int __attribute__((__always_inline__))
simd_ctpopow (int256 __A)
{
        return (int) __builtin_sw_host_ctpopow(__A);
}

static __inline int __attribute__((__always_inline__))
simd_ctlzow (int256 __A)
{
        return (int) __builtin_sw_host_ctlzow(__A);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucaddw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw_host_vucaddw(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vucaddwi (intv8 __A, const int __B)
{
	return (intv8)  __builtin_sw_host_vucaddwi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucsubw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw_host_vucsubw(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vucsubwi (intv8 __A, const int __B)
{
	return (intv8)  __builtin_sw_host_vucsubwi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucaddh (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw_host_vucaddh(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vucaddhi (intv8 __A, const int __B)
{
	return (intv8)  __builtin_sw_host_vucaddhi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucsubh (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw_host_vucsubh(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vucsubhi (intv8 __A, const int __B)
{
	return (intv8)  __builtin_sw_host_vucsubhi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucaddb (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw_host_vucaddb(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vucaddbi (intv8 __A, const int __B)
{
	return (intv8)  __builtin_sw_host_vucaddbi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucsubb (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw_host_vucsubb(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vucsubbi (intv8 __A, const int __B)
{
	return (intv8)  __builtin_sw_host_vucsubbi(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vlog (const int i,intv8 __x, intv8 __y, intv8 __z)
{
	return (intv8) __builtin_inst_opt_vlogzz_v8si(__x, __y, __z, i);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vseleqw (intv8 __A, intv8 __B, intv8 __C)
{
        return (intv8)__builtin_sw_host_vseleqw(__A, __B, __C);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vseleqwi (intv8 __A, intv8 __B, const int __C)
{
        return (intv8)__builtin_sw_host_vseleqwi(__A, __B, __C);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vselltw (intv8 __A, intv8 __B, intv8 __C)
{
        return (intv8)__builtin_sw_host_vselltw(__A, __B, __C);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vselltwi (intv8 __A, intv8 __B, const int __C)
{
        return (intv8)__builtin_sw_host_vselltwi(__A, __B, __C);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsellew (intv8 __A, intv8 __B, intv8 __C)
{
        return (intv8)__builtin_sw_host_vsellew(__A, __B, __C);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vsellewi (intv8 __A, intv8 __B, const int __C)
{
        return (intv8)__builtin_sw_host_vsellewi(__A, __B, __C);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsellbcw (intv8 __A, intv8 __B, intv8 __C)
{
        return (intv8)__builtin_sw_host_vsellbcw(__A, __B, __C);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vsellbcwi (intv8 __A, intv8 __B, const int __C)
{
        return (intv8)__builtin_sw_host_vsellbcwi(__A, __B, __C);
}

#define simd_vcpyf(x) __builtin_sw_host_vcpyf(x) 

static __inline floatv4 __attribute__((__always_inline__))
simd_vcpyfs (floatv4 __A) 
{
        return (floatv4)__builtin_sw_host_vcpyfs(__A);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vcpyfd (doublev4 __A) 
{
        return (doublev4)__builtin_sw_host_vcpyfd(__A);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcpyw (intv8 __A) 
{
        return (intv8)__builtin_sw_host_vcpyw(__A);
}

//static __inline intv8 __attribute__((__always_inline__))
//simd_vconw (intv8 __A, intv8 __B,  void * __C)
//{
//        return (intv8)__builtin_sw_host_vconw(__A, __B, __C);
//}
//
//static __inline floatv4 __attribute__((__always_inline__))
//simd_vcons (floatv4 __A, floatv4 __B, void * __C)
//{
//        return (floatv4)__builtin_sw_host_vcons(__A, __B, __C);
//}
//
//static __inline doublev4 __attribute__((__always_inline__))
//simd_vcond (doublev4 __A, doublev4 __B, void * __C)
//{
//        return (doublev4)__builtin_sw_host_vcond(__A, __B, __C);
//}
//
//static __inline intv8 __attribute__((__always_inline__))
//simd_vshfw (intv8 __A, intv8 __B, double __C)
//{
//        return (intv8)__builtin_sw_host_vshfw(__A, __B, __C);
//}
#endif /*_SIMD_HOST*/

#ifdef _SIMD_SLAVE

/********************************************************************************/
/*         Declare the SLAVE TARGET SIMD operations                              */
/********************************************************************************/
static __inline intv8 __attribute__((__always_inline__))
simd_vrotlw (intv8 __A, int __B) 
{
        return (intv8)  __builtin_sw_vrotlw(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vrotlwi (intv8 __A, const int __B) 
{
        return (intv8)  __builtin_sw_vrotlwi(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vlog2x(const int __C,intv8 __A,intv8 __B)
{
	if(__C>=0 && __C<8)
		return (intv8)__builtin_sw_slave_vlog2xi(__A,__B,__C);
	else
	{
		printf("simd_vlog2xi:Invalid arg 3\n");
		return __A;
	}
}

static __inline intv8 __attribute__((__always_inline__))
simd_vlog3x(const int __D,intv8 __A,intv8 __B,intv8 __C)
{
	if(__D>=0 && __D<8)
		return (intv8)__builtin_sw_slave_vlog3ri(__A,__B,__C,__D);
	else
	{
		printf("simd_vlog3ri:Invalid arg 4\n");
		return __A;
	}
}

static __inline int256 __attribute__((__always_inline__))
simd_uaddo_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_uaddo_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_usubo_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_usubo_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_uaddo_take_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_uaddo_take_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_usubo_take_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_usubo_take_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_addo_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_addo_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_subo_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_subo_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_addo_take_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_addo_take_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_subo_take_carry(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_subo_take_carry(__A,__B);
}
static __inline int256 __attribute__((__always_inline__))
simd_umulqa(int256 __A,int256 __B)
{
	return (int256)__builtin_sw_slave_umulqa(__A,__B);
}

#define simd_vshuffle(x,y,z) __builtin_sw_slave_vshuffle(x,y,z)

static __inline intv8 __attribute__((__always_inline__))
simd_vshufflew(intv8 __A,intv8 __B,int __C)
{
	 return (intv8) __builtin_sw_slave_vshufflew(__A,__B,__C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vshuffles(floatv4 __A,floatv4 __B,int __C)
{
	 return (floatv4) __builtin_sw_slave_vshuffles(__A,__B,__C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vshuffled(doublev4 __A,doublev4 __B,int __C)
{
	 return (doublev4) __builtin_sw_slave_vshuffled(__A,__B,__C);
}

//static __inline intv8 __attribute__((__always_inline__))
//simd_selldw(intv8 __A,int * __B,int __C)
//{
//	return (intv8) __builtin_sw_slave_selldw( __A, __B, __C);
//}
//
//static __inline void __attribute__((__always_inline__))
//simd_lookup(intv8 __A,int * __B,intv8 __C)
//{
//	return  __builtin_sw_slave_selldw( __A, __B, __C);
//}
//new end

#endif /*_SIMD_SLAVE*/

 
#endif /*_SIMD_H_INCLUDED*/
