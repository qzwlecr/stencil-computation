#ifndef _SIMD_H_INCLUDED
#define _SIMD_H_INCLUDED

#include <stdio.h>

/********************************************************************************/
/*	Declare the SIMD floating point operations			    	*/
/*		by guoy 2003-4-29						*/
/********************************************************************************/

#ifdef SW1
typedef int __m128 __attribute__ ((__mode__(__V2DF__)));
typedef int __v2df __attribute__ ((__mode__(__V2DF__)));
typedef int __v2sf __attribute__ ((__mode__(__V2SF__)));

typedef int __v4si __attribute__ ((__mode__(__V4SI__)));

typedef int intv4 __attribute__ ((__mode__(__V4SI__)));
typedef unsigned uintv4 __attribute__ ((__mode__(__V4SI__)));
typedef int int128 __attribute__ ((__mode__(__VI16__)));
typedef unsigned uint128 __attribute__ ((__mode__(__VI16__)));
typedef int doublev2 __attribute__ ((__mode__(__V2DF__)));
typedef int floatv2 __attribute__ ((__mode__(__V2SF__)));
#endif
#if (defined SW2) || (defined SW5)
typedef int intv8 __attribute__ ((__mode__(__V8SI__)));
typedef unsigned uintv8 __attribute__ ((__mode__(__V8SI__)));
//typedef int int256 __attribute__ ((__mode__(__VI32__)));
//typedef unsigned uint256 __attribute__ ((__mode__(__VI32__)));
typedef int int256 __attribute__ ((__mode__(__V1OI__)));
typedef unsigned uint256 __attribute__ ((__mode__(__V1OI__)));
typedef float floatv4 __attribute__ ((__mode__(__V4SF__)));
typedef double doublev4 __attribute__ ((__mode__(__V4DF__)));
#endif

#ifndef WPH20120327
typedef struct {
int va[4];
} __attribute__ ((aligned (16))) intv4;

typedef struct {
unsigned int va[4];
} __attribute__ ((aligned (16))) uintv4;

typedef struct {
long va[2];
} __attribute__ ((aligned (16))) int128;

typedef struct {
unsigned long va[2];
} __attribute__ ((aligned (16))) uint128;

typedef struct {
float va[2];
} __attribute__ ((aligned (8))) floatv2;

typedef struct {
double va[2];
} __attribute__ ((aligned (16))) doublev2;
#endif

#ifdef SW1
static __inline intv4 __attribute__((__always_inline__))
simd_set_intv4 (int __W, int __X, int __Y, int __Z)
{
    union {
    int __a[4];
    intv4 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;
  __u.__a[2] = __Y;
  __u.__a[3] = __Z;

  return __u.__v;
}

static __inline uintv4 __attribute__((__always_inline__))
simd_set_uintv4 (unsigned int __W, unsigned int __X, unsigned int __Y, unsigned int __Z)
{
    union {
    unsigned int __a[4];
    uintv4 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;
  __u.__a[2] = __Y;
  __u.__a[3] = __Z;

  return __u.__v;
}

static __inline int128 __attribute__((__always_inline__))
simd_set_int128 (long __W, long __X)
{
    union {
    long __a[2];
    int128 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;

  return __u.__v;
}

static __inline uint128 __attribute__((__always_inline__))
simd_set_uint128 (unsigned long __W, unsigned long __X)
{
    union {
    unsigned long __a[2];
    uint128 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;

  return __u.__v;
}

static __inline floatv2 __attribute__((__always_inline__))
simd_set_floatv2 (float __W, float __X)
{
    union {
    float __a[2] __attribute__ ((aligned (16)));
    floatv2 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;

  return __u.__v;
}

static __inline doublev2 __attribute__((__always_inline__))
simd_set_doublev2 (double __W, double __X)
{
    union {
    double __a[2] __attribute__ ((aligned (16)));
    doublev2 __v;
  } __u;

  __u.__a[0] = __W;
  __u.__a[1] = __X;

  return __u.__v;
}
#endif

#if (defined SW2) || (defined SW5)
static __inline intv8 __attribute__((__always_inline__))
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

static __inline uintv8 __attribute__((__always_inline__))
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

static __inline int256 __attribute__((__always_inline__))
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

static __inline uint256 __attribute__((__always_inline__))
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

static __inline floatv4  __attribute__((__always_inline__))
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

static __inline doublev4 __attribute__((__always_inline__))
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
#endif

#ifdef SW1
static __inline intv4 __attribute__((__always_inline__))
simd_vaddl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vaddl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vsubl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vsubl(__A, __B);
}
static __inline intv4 __attribute__((__always_inline__))
simd_vucaddl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vucaddl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vucsubl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vucsubl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vslll (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vslll(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vsrll (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vsrll(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vsral (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vsral(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vandl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vandl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vbicl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vbicl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vornotl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vornotl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vorl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vorl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vxorl (intv4 __A, intv4 __B)
{
	return (intv4)  __builtin_sw64_vxorl(__A, __B);
}

static __inline intv4 __attribute__((__always_inline__))
simd_vabsl (intv4 __A)
{
	return (intv4)  __builtin_sw64_vabsl(__A);
}

static __inline int128 __attribute__((__always_inline__))
simd_ando (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_ando(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_biso (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_oro(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_bico (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_bico(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_xoro (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_xoro(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_andnoto (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_andnoto (__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_ornoto (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_ornoto (__A, __B);
}

/*
static __inline int128 __attribute__((__always_inline__))
simd_xornoto (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_xornoto(__A, __B);
}
*/

static __inline int128 __attribute__((__always_inline__))
simd_sllo (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_sllo(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_srlo (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_srlo(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_srao (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_srao(__A, __B);
}

static __inline int128 __attribute__((__always_inline__))
simd_eqvo (int128 __A, int128 __B)
{
	return (int128)  __builtin_sw64_eqvo(__A, __B);
}

static __inline long __attribute__((__always_inline__))
simd_ctlzo (int128 __A)
{
	return (long)  __builtin_sw64_ctlzo(__A);
}

static __inline long __attribute__((__always_inline__))
simd_cttzo (int128 __A)
{
	return (long)  __builtin_sw64_cttzo(__A);
}

static __inline long __attribute__((__always_inline__))
simd_ctpopo (int128 __A)
{
	return (long)  __builtin_sw64_ctpopo(__A);
}

static __inline long __attribute__((__always_inline__))
simd_ctoddo (int128 __A)
{
	return (long)  __builtin_sw64_ctoddo(__A);
}
/*
static __inline floatv2 __attribute__((__always_inline__))
simd_vadds (floatv2 __A, floatv2 __B)
{
	return (floatv2)__builtin_sw64_vadds(__A, __B);
}

static __inline floatv2 __attribute__((__always_inline__))
simd_vsubs (floatv2 __A, floatv2 __B)
{
	return (floatv2)__builtin_sw64_vsubs(__A, __B);
}

static __inline floatv2 __attribute__((__always_inline__))
simd_vmuls (floatv2 __A, floatv2 __B)
{
	return (floatv2)__builtin_sw64_vmuls(__A, __B);
}

static __inline floatv2 __attribute__((__always_inline__))
simd_vdivs (floatv2 __A, floatv2 __B)
{
	return (floatv2)__builtin_sw64_vdivs(__A, __B);
}

static __inline floatv2 __attribute__((__always_inline__))
simd_vsqrts (floatv2 __A)
{
	return (floatv2)__builtin_sw64_vsqrts(__A);
}*/

static __inline floatv2 __attribute__((__always_inline__))
simd_vfmas (floatv2 __A, floatv2 __B, floatv2 __C)
{
	return (floatv2)__builtin_sw64_vfmas(__A, __B, __C);
}

static __inline floatv2 __attribute__((__always_inline__))
simd_vfmss (floatv2 __A, floatv2 __B, floatv2 __C)
{
	return (floatv2)__builtin_sw64_vfmss( __A, __B, __C);
}

static __inline floatv2 __attribute__((__always_inline__))
simd_vfnmas (floatv2 __A, floatv2 __B, floatv2 __C)
{
	return (floatv2)__builtin_sw64_vfnmas(__A, __B, __C);
}
/*
static __inline doublev2 __attribute__((__always_inline__))
simd_vaddd (doublev2 __A, doublev2 __B)
{
	return (doublev2)__builtin_sw64_vaddd(__A, __B);
}

static __inline doublev2 __attribute__((__always_inline__))
simd_vsubd (doublev2 __A, doublev2 __B)
{
	return (doublev2)__builtin_sw64_vsubd(__A, __B);
}

static __inline doublev2 __attribute__((__always_inline__))
simd_vmuld (doublev2 __A, doublev2 __B)
{
	return (doublev2)__builtin_sw64_vmuld(__A, __B);
}

static __inline doublev2 __attribute__((__always_inline__))
simd_vdivd (doublev2 __A, doublev2 __B)
{
	return (doublev2)__builtin_sw64_vdivd(__A, __B);
}

static __inline doublev2 __attribute__((__always_inline__))
simd_vsqrtd (doublev2 __A)
{
	return (doublev2)__builtin_sw64_vsqrtd(__A);
}*/

static __inline doublev2 __attribute__((__always_inline__))
simd_vfmad (doublev2 __A, doublev2 __B, doublev2 __C)
{
	return (doublev2)__builtin_sw64_vfmad(__A, __B, __C);
}

static __inline doublev2 __attribute__((__always_inline__))
simd_vfmsd (doublev2 __A, doublev2 __B, doublev2 __C)
{
	return (doublev2)__builtin_sw64_vfmsd (__A, __B, __C);
}

static __inline doublev2 __attribute__((__always_inline__))
simd_vfnmad (doublev2 __A, doublev2 __B, doublev2 __C)
{
	return (doublev2)__builtin_sw64_vfnmad(__A, __B, __C);
}
#endif

#if (defined SW2) || (defined SW5)
static __inline intv8 __attribute__((__always_inline__))
simd_vaddw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vaddw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsubw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vsubw(__A, __B);
}

static __inline int256 __attribute__((__always_inline__))
simd_vaddl (int256 __A, int256 __B)
{
	return (int256)  __builtin_sw64_vaddl2(__A, __B);
}

static __inline int256 __attribute__((__always_inline__))
simd_vsubl (int256 __A, int256 __B)
{
	return (int256)  __builtin_sw64_vsubl2(__A, __B);
}
#ifndef SW5
static __inline intv8 __attribute__((__always_inline__))
simd_vucaddw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vucaddw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucsubw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vucsubw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucaddh (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vucaddh(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucsubh (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vucsubh(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucaddb (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vucaddb(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vucsubb (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vucsubb(__A, __B);
}
#endif

static __inline intv8 __attribute__((__always_inline__))
simd_vandw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vandw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vbicw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vbicw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vbisw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vorw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vornotw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vornotw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vxorw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_vxorw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_veqvw (intv8 __A, intv8 __B)
{
	return (intv8)  __builtin_sw64_veqvw(__A, __B);
}
/*
static __inline intv8 __attribute__((__always_inline__))
simd_vlog (int __A, intv8 __B, intv8 __C, intv8 __D)
{
	return (intv8)  __builtin_sw64_vlog(__A, __B, __C, __D);
}
*/
#define simd_vlog(__A, __B, __C, __D) __builtin_sw64_vlog(__A, __B, __C, __D)

static __inline intv8 __attribute__((__always_inline__))
simd_vsllw (intv8 __A, int __B)
{
	return (intv8)  __builtin_sw64_vsllw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsrlw (intv8 __A, int __B)
{
	return (intv8)  __builtin_sw64_vsrlw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vsraw (intv8 __A, int __B)
{
	return (intv8)  __builtin_sw64_vsraw(__A, __B);
}
/*
static __inline intv8 __attribute__((__always_inline__))
simd_vrolw (intv8 __A, int __B)
{
	return (intv8)  __builtin_sw64_vrolw(__A, __B);
}
*/
#define simd_vrolw(__A, __B) __builtin_sw64_vrolw(__A, __B)

static __inline int256 __attribute__((__always_inline__))
simd_sllow (int256 __A, int __B)
{
	return (int256)  __builtin_sw64_sllow(__A, __B);
}

static __inline int256 __attribute__((__always_inline__))
simd_srlow (int256 __A, int __B)
{
	return (int256)  __builtin_sw64_srlow(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_v4adds (floatv4 __A, floatv4 __B)
{
	return (floatv4)__builtin_sw64_v4adds(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_v4addd (doublev4 __A, doublev4 __B)
{
	return (doublev4)__builtin_sw64_v4addd(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_v4subs (floatv4 __A, floatv4 __B)
{
	return (floatv4)__builtin_sw64_v4subs(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_v4subd (doublev4 __A, doublev4 __B)
{
	return (doublev4)__builtin_sw64_v4subd(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_v4muls (floatv4 __A, floatv4 __B)
{
	return (floatv4)__builtin_sw64_v4muls(__A, __B);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_v4muld (doublev4 __A, doublev4 __B)
{
	return (doublev4)__builtin_sw64_v4muld(__A, __B);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vmas (floatv4 __A, floatv4 __B, floatv4 __C)
{
	return (floatv4)__builtin_sw64_vmas(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vmss (floatv4 __A, floatv4 __B, floatv4 __C)
{
	return (floatv4)__builtin_sw64_vmss(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vnmas (floatv4 __A, floatv4 __B, floatv4 __C)
{
	return (floatv4)__builtin_sw64_vnmas(__A, __B, __C);
}

static __inline floatv4 __attribute__((__always_inline__))
simd_vnmss (floatv4 __A, floatv4 __B, floatv4 __C)
{
	return (floatv4)__builtin_sw64_vnmss(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vmad (doublev4 __A, doublev4 __B, doublev4 __C)
{
	return (doublev4)__builtin_sw64_vmad(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vmsd (doublev4 __A, doublev4 __B, doublev4 __C)
{
	return (doublev4)__builtin_sw64_vmsd(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vnmad (doublev4 __A, doublev4 __B, doublev4 __C)
{
	return (doublev4)__builtin_sw64_vnmad(__A, __B, __C);
}

static __inline doublev4 __attribute__((__always_inline__))
simd_vnmsd (doublev4 __A, doublev4 __B, doublev4 __C)
{
	return (doublev4)__builtin_sw64_vnmsd(__A, __B, __C);
}

/*
static __inline intv8  __attribute__((__always_inline__))
simd_vinsw (intv8 __A, intv8 __B, int __C)
{
	return (intv8)__builtin_sw64_vinsw(__A, __B, __C);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw (intv8 __A, int __B)
{
	return (intv8)__builtin_sw64_vextw(__A, __B);
}
*/
#ifndef SW5
static __inline intv8  __attribute__((__always_inline__))
simd_vcpyw (intv8 __A)
{
	return (intv8)__builtin_sw64_vcpyw(__A);
}
#endif
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw0 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw0(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw1 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw1(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw2 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw2(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw3 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw3(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw4 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw4(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw5 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw5(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw6 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw6(__A, __B);
}
static __inline intv8 __attribute__((__always_inline__))
simd_vinsw7 (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vinsw7(__A, __B);
}
#define simd_vinsf0(x,y) __builtin_sw64_ins0 (x, y)
#define simd_vinsf1(x,y) __builtin_sw64_ins1 (x, y)
#define simd_vinsf2(x,y) __builtin_sw64_ins2 (x, y)
#define simd_vinsf3(x,y) __builtin_sw64_ins3 (x, y)

static __inline intv8  __attribute__((__always_inline__))
simd_vextw0 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw0(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw1 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw1(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw2 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw2(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw3 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw3(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw4 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw4(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw5 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw5(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw6 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw6(__A);
}
static __inline intv8  __attribute__((__always_inline__))
simd_vextw7 (intv8 __A)
{
	return (intv8)__builtin_sw64_vextw7(__A);
}
#define simd_vextf0(x) __builtin_sw64_extf0 (x)
#define simd_vextf1(x) __builtin_sw64_extf1 (x)
#define simd_vextf2(x) __builtin_sw64_extf2 (x)
#define simd_vextf3(x) __builtin_sw64_extf3 (x)
#ifndef SW5
static __inline int __attribute__((__always_inline__))
simd_vcmpgew (intv8 __A, intv8 __B)
{
        return (int)__builtin_sw64_vcmpgew(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmpeqw (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vcmpeqw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmplew (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vcmplew(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmpltw (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vcmpltw(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmpulew (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vcmpulew(__A, __B);
}

static __inline intv8 __attribute__((__always_inline__))
simd_vcmpultw (intv8 __A, intv8 __B)
{
        return (intv8)__builtin_sw64_vcmpultw(__A, __B);
}

static __inline intv8  __attribute__((__always_inline__))
simd_vseleqw (intv8 __A, intv8 __B, intv8 __C)
{
	return (intv8)__builtin_sw64_vseleqw(__A, __B, __C);
}

static __inline intv8  __attribute__((__always_inline__))
simd_vselltw (intv8 __A, intv8 __B, intv8 __C)
{
	return (intv8)__builtin_sw64_vselltw(__A, __B, __C);
}

static __inline intv8  __attribute__((__always_inline__))
simd_vsellew (intv8 __A, intv8 __B, intv8 __C)
{
	return (intv8)__builtin_sw64_vsellew(__A, __B, __C);
}

static __inline intv8  __attribute__((__always_inline__))
simd_vsellbcw (intv8 __A, intv8 __B, intv8 __C)
{
	return (intv8)__builtin_sw64_vsellbcw(__A, __B, __C);
}

static __inline int  __attribute__((__always_inline__))
simd_ctlzow (int256 __A)
{
	return (int)  __builtin_sw64_ctlzow(__A);
}

static __inline int  __attribute__((__always_inline__))
simd_ctpopow (int256 __A)
{
	return (int)  __builtin_sw64_ctpopow(__A);
}
#endif
#endif

#ifndef JJSIMD20081011
#define simd_vadds(x,y) __builtin_sw64_adds (x, y)
#define simd_vaddd(x,y) __builtin_sw64_ddda (x, y)
#define simd_vsubs(x,y) __builtin_sw64_ssub (x, y)
#define simd_vsubd(x,y) __builtin_sw64_sdbu (x, y)
#define simd_vmuls(x,y) __builtin_sw64_smul (x, y)
#define simd_vmuld(x,y) __builtin_sw64_dlum (x, y)
#define simd_vdivs(x,y) __builtin_sw64_sdiv (x, y)
#define simd_vdivd(x,y) __builtin_sw64_dldiv (x, y)
#define simd_vsqrts(x) __builtin_sw64_sgqrts (x)
#define simd_vsqrtd(x) __builtin_sw64_dlsqrt (x)
#ifndef GML20150617
#define simd_vabsd(x)  __builtin_sw64_vabsd (x)
#define simd_vabss(x)  __builtin_sw64_vabss (x)
#endif
//#define simd_vinsf(x,y,z) __builtin_sw64_ins (x, y, z)
#define simd_vextf(x,y) __builtin_sw64_ext (x, y)
#define simd_vcpyf(x) __builtin_sw64_cpy (x)
#endif
#ifndef JJSIMD20081013
#define simd_vseleq(x,y,z) __builtin_sw64_sleq(x, y, z)
#define simd_vsellt(x,y,z) __builtin_sw64_sllt(x, y, z)
#define simd_vselle(x,y,z) __builtin_sw64_elles(x, y, z)
#endif
#ifdef JJSIMD20090531
#define simd_vselne(x,y,z) __builtin_sw64_slne(x, y, z)
#define simd_vselgt(x,y,z) __builtin_sw64_selgt(x, y, z)
#define simd_vselge(x,y,z) __builtin_sw64_sellge(x, y, z)
#endif
#ifndef JJSIMD20090826
#define simd_vfcmpeq(x,y) __builtin_sw64_fcmpeq (x, y)
#define simd_vfcmple(x,y) __builtin_sw64_lefcmp (x, y)
#define simd_vfcmplt(x,y) __builtin_sw64_tlpmcf (x, y)
#define simd_vfcmpun(x,y) __builtin_sw64_fcmpun (x, y)
#endif
#define simd_vcpys(x,y) __builtin_sw64_sypc (x, y)
#define simd_vcpysn(x,y) __builtin_sw64_cpysn (x, y)
#define simd_vcpyse(x,y) __builtin_sw64_esypc (x, y)
#ifndef JJSIMD20101123
#define simd_vsll1(x) __builtin_sw64_vsllyi(x)
#define simd_vsll2(x) __builtin_sw64_slltwo(x)
#define simd_vsll3(x) __builtin_sw64_sllthree(x)
#define simd_vsrl1(x) __builtin_sw64_srlfone(x)
#define simd_vsrl2(x) __builtin_sw64_srlftwo(x)
#define simd_vsrl3(x) __builtin_sw64_srlfthree(x)
#define simd_vbis(x,y) __builtin_sw64_bisf(x,y)
#endif
#if 0
#define simd_load(x,y) 				\
  ({						\
	(x) = __builtin_sw64_load ((x), (y));	\
	(x);				\
  })
#else
#define simd_load(x,y) 				\
  ({						\
	(x) = __builtin_sw64_load ((x), (y));	\
  })
#endif
#define simd_loadl(x,y)				\
  ({						\
	(x) = __builtin_sw64_loadl ((x), (y));	\
	(x);                                    \
  })

#define simd_loade(x,y)				\
  ({						\
	(x) = __builtin_sw64_lde ((x), (y));		\
	(x);					\
  })

#define simd_loadh(x,y)				\
  ({						\
	(x) = __builtin_sw64_loadh ((x), (y));	\
	(x);                                    \
  })
#ifndef JJSIMD20090908
#define simd_loadu(x,y)				\
  ({						\
	(x) = __builtin_sw64_loadu ((x), (y));	\
	(x);                                    \
  })
#endif
#ifndef JJSIMD20101009
#define simd_atov(x,y) 				\
  ({						\
	(x) = __builtin_sw64_arraytov ((x), (y));	\
	(x);				\
  })

#define simd_atovu(x,y) 				\
  ({						\
	(x) = __builtin_sw64_arraytovu ((x), (y));	\
	(x);				\
  })
#endif
/*
#define simd_mergex(x,y)						\
  ({									\
	typeof ((x)) __tmp__;						\
	__asm__("mergex %1,%2,%0" : "=f"(__tmp__) : "f"(x) , "f"(y));	\
	__tmp__;							\
  })
*/

/************************************************************
 *                                                          *
 *  print functions for simd type                           *
 *                                                          *
 ************************************************************/
#ifdef SW1
static __inline void __attribute__((__always_inline__))
simd_fprint_intv4 (FILE *fp, intv4 a)
{
  union {
    int __a[4];
    intv4 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %d, %d, %d, %d ]\n", __u.__a[0], __u.__a[1], __u.__a[2], __u.__a[3]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_uintv4 (FILE *fp, uintv4 a)
{
  union {
    unsigned int __a[4];
    uintv4 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %u, %u, %u, %u ]\n", __u.__a[0], __u.__a[1], __u.__a[2], __u.__a[3]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_int128 (FILE *fp, int128 a)
{
  union {
    long __a[2];
    int128 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ 0x%lx, 0x%lx ]\n", __u.__a[0], __u.__a[1]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_uint128 (FILE *fp, uint128 a)
{
  union {
    unsigned long __a[2];
    uint128 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ 0x%lx, 0x%lx ]\n", __u.__a[0], __u.__a[1]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_doublev2 (FILE *fp, doublev2 a)
{
  union {
    double __a[2];
    doublev2 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %e, %e ]\n", __u.__a[0], __u.__a[1]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_floatv2 (FILE *fp, floatv2 a)
{
  union {
    float __a[2];
    floatv2 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %e, %e ]\n", __u.__a[0], __u.__a[1]);
}
#endif
#if (defined SW2) || (defined SW5)
static __inline void __attribute__((__always_inline__))
simd_fprint_intv8 (FILE *fp, intv8 a)
{
  union {
    int __a[8];
    intv8 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %d, %d, %d, %d, %d, %d, %d, %d ]\n", __u.__a[7], __u.__a[6], __u.__a[5], __u.__a[4], __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_uintv8 (FILE *fp, uintv8 a)
{
  union {
    unsigned int __a[8];
    uintv8 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %u, %u, %u, %u, %u, %u, %u, %u ]\n", __u.__a[7], __u.__a[6], __u.__a[5], __u.__a[4], __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_int256 (FILE *fp, int256 a)
{
  union {
    long __a[4];
    int256 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ 0x%lx, 0x%lx, 0x%lx, 0x%lx ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_uint256 (FILE *fp, uint256 a)
{
  union {
    unsigned long __a[4];
    uint256 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ 0x%lx, 0x%lx, 0x%lx, 0x%lx ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_doublev4 (FILE *fp, doublev4 a)
{
  union {
    double __a[4];
    doublev4 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %e, %e, %e, %e ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}

static __inline void __attribute__((__always_inline__))
simd_fprint_floatv4 (FILE *fp, floatv4 a)
{
  union {
    float __a[4];
    floatv4 __v;
  } __u;
  __u.__v = a;
  fprintf (fp, "[ %e, %e, %e, %e ]\n", __u.__a[3], __u.__a[2], __u.__a[1], __u.__a[0]);
}
#endif

#ifdef SW1
static __inline void __attribute__((__always_inline__)) simd_print_doublev2 (doublev2 a) { simd_fprint_doublev2 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_floatv2 (floatv2 a) { simd_fprint_floatv2 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_intv4 (intv4 a) { simd_fprint_intv4 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_uintv4 (uintv4 a) { simd_fprint_uintv4 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_int128 (int128 a) { simd_fprint_int128 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_uint128 (uint128 a) { simd_fprint_uint128 (stdout, a); }
#endif
#if (defined SW2) || (defined SW5)
static __inline void __attribute__((__always_inline__)) simd_print_doublev4 (doublev4 a) { simd_fprint_doublev4 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_floatv4 (floatv4 a) { simd_fprint_floatv4 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_intv8 (intv8 a) { simd_fprint_intv8 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_uintv8 (uintv8 a) { simd_fprint_uintv8 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_int256 (int256 a) { simd_fprint_int256 (stdout, a); }
static __inline void __attribute__((__always_inline__)) simd_print_uint256 (uint256 a) { simd_fprint_uint256 (stdout, a); }
#endif

#define simd_store(x,y) __builtin_sw64_store (x, y)
#ifndef JJSIMD20090908
#define simd_storeu(x,y) __builtin_sw64_storeu (x, y)
#endif
#ifndef JJSIMD20101009
#define simd_vtoa(x,y) __builtin_sw64_vectoa (x, y)
#define simd_vtoau(x,y) __builtin_sw64_vectoau (x, y)
#endif

#ifdef SW5  //XQSIMD201406

#define simd_putr(x,y)  ({asm volatile ("#nop":::"memory");__builtin_sw64_putR(x,y);})
#define simd_putc(x,y)  ({asm volatile ("#nop":::"memory");__builtin_sw64_putC(x,y);})
#define simd_getr(x)    ({asm volatile ("#nop":::"memory");__builtin_sw64_getxR(x);})
#define simd_getc(x)    ({asm volatile ("#nop":::"memory");__builtin_sw64_getxC(x);})

#define simd_loadr(x,y)                         \
  ({                                            \
        (x) = __builtin_sw64_loadrow ((x), (y));  \
        (x);                                    \
  })   
#define simd_loadc(x,y)                         \
  ({                                            \
        (x) = __builtin_sw64_loadcol ((x), (y));  \
        (x);                                    \
  })    
#define simd_loader(x,y)                        \
  ({                                            \
        (x) = __builtin_sw64_loaderow ((x), (y));   \
        (x);                                    \
  })
#define simd_loadec(x,y)                        \
  ({                                            \
        (x) = __builtin_sw64_loadecol ((x), (y));   \
        (x);                                    \
  })

#define simd_vlog3x(n,x,y,z)    __builtin_sw64_vlogx3(n,x,y,z)
#define simd_vlog2x(n,x,y)      __builtin_sw64_vlogx2(n,x,y)

#define simd_vshff(x,y,n) __builtin_sw64_shff(x,y,n)
#define simd_selldw(x,y,n)  __builtin_sw64_selldw (x,y,n) 

static __inline int256 __attribute__((__always_inline__))
simd_uaddo_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_uaddo_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_usubo_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_usubo_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_uaddo_take_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_uaddo_take_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_usubo_take_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_usubo_take_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_addo_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_addo_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_subo_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_subo_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_addo_take_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_addo_take_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_subo_take_carry (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_subo_take_carry(__A, __B);
}
static __inline int256 __attribute__((__always_inline__))
simd_umulqa (int256 __A, int256 __B)
{
        return (int256)  __builtin_sw64_umulqa(__A, __B);
}

#endif

#ifdef SW1
#define simd_storel(x,y) __builtin_sw64_storel (x, y)
#define simd_storeh(x,y) __builtin_sw64_storeh (x, y)
#define simd_mergec(x,y) __builtin_sw64_egrem (x, y)
#define simd_mergel(x,y) __builtin_sw64_legrem (x, y)
#define simd_mergeh(x,y) __builtin_sw64_mgh (x, y)
#define simd_mergex(x,y) __builtin_sw64_mergex (x, y)

static __inline long __attribute__((__always_inline__))
simd_get_int128_high (int128 __x)
{
        return
          ({
              unsigned long __ret__;
              __asm__("ftoidh %1,%0" : "=r"(__ret__) : "f"(__x));
              __ret__;
          });
}

static __inline long __attribute__((__always_inline__))
simd_get_int128_low (int128 __x)
{
        return
          ({
              unsigned long __ret__;
              __asm__("ftoid %1,%0" : "=r"(__ret__) : "f"(__x));
              __ret__;
          });
}

static __inline double __attribute__((__always_inline__))
simd_get_doublev2_low (doublev2 __x)
{
        return
          ({
              double __ret__;
              __asm__("fmov %1,%0" : "=f"(__ret__) : "f"(__x));
              __ret__;
          });
}

static __inline double __attribute__((__always_inline__))
simd_get_doublev2_high (doublev2 __x)
{
        return
          ({
              double __ret__;
              __asm__("mergeh %1,%1,%0" : "=f"(__ret__) : "f"(__x));
              __ret__;
          });
}

static __inline float __attribute__((__always_inline__))
simd_get_floatv2_low (floatv2 __x)
{
        return
          ({
              float __ret__;
              __asm__("fmov %1,%0" : "=f"(__ret__) : "f"(__x));
              __ret__;
          });
}

static __inline float __attribute__((__always_inline__))
simd_get_floatv2_high (floatv2 __x)
{
        return
          ({
              float __ret__;
              __asm__("mergeh %1,%1,%0" : "=f"(__ret__) : "f"(__x));
              __ret__;
          });
}

#define simd_set_int128_high(dst,src) __asm__("itofdh %1,%0":"+f"((dst)):"r"((src)))
#define simd_set_int128_low(dst,src) __asm__("itofd %1,%0":"+f"((dst)):"r"((src)))
#define simd_set_doublev2_high(dst,src) __asm__("mergel %0,%1,%0":"+f"((dst)):"f"((src)))
#define simd_set_doublev2_low(dst,src)					\
{									\
	doublev2 __tmp;							\
 	__asm__("mergel %1,%1,%0":"=f"((__tmp)):"f"((src)));		\
	__asm__("mergeh %1,%0,%0":"+f"((dst)):"f"((__tmp)));		\
}

#define simd_set_floatv2_high(dst,src) __asm__("mergel %0,%1,%0":"+f"((dst)):"f"((src)))
#define simd_set_floatv2_low(dst,src)					\
{									\
	floatv2 __tmp;							\
 	__asm__("mergel %1,%1,%0":"=f"((__tmp)):"f"((src)));		\
	__asm__("mergeh %1,%0,%0":"+f"((dst)):"f"((__tmp)));		\
}
#endif

#endif /*_SIMD_H_INCLUDED*/
