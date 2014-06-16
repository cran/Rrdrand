/*
 * 
 * Rrdrand : physical random number if R on Intel CPU ivy bridge or later
 * Author: Eiji Nakama<nakama@com-one.com>, Junji Nakano <nakanoj@ism.ac.jp>
 * Copyright(C) 2014
 */

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Random.h>

#ifdef WIN32
#if defined(__i386__) || defined(__x86_64__)
#define HAVE_X86_CPUID 1
#endif
#else
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#define POOLSIZE 4096
static int poolsize=POOLSIZE;
static int poolpoint=0;
static int poolavail=0;
static double poolbuf[POOLSIZE];

__inline static void mycpuid(int op, int *eax, int *ebx, int *ecx, int *edx){
#if defined(HAVE_X86_CPUID) && HAVE_X86_CPUID == 1
        __asm__ __volatile__("cpuid"
                             : "=a" (*eax),
                               "=b" (*ebx),
                               "=c" (*ecx),
                               "=d" (*edx) : "0" (op));
#endif
}

__inline static int has_rdrand(void)
{
   static int have_rdrand = -1;
#if defined(HAVE_X86_CPUID) && HAVE_X86_CPUID == 1
   int eax, ebx, ecx, edx;
   if (have_rdrand!=-1) return(have_rdrand);
   mycpuid(1,&eax, &ebx, &ecx, &edx);
   if((ecx & ( 1 << 30)) != 0) have_rdrand = 1;
   else  have_rdrand = 0;
#else
   have_rdrand = 0;
#endif
   return(have_rdrand);
}

SEXP Rrdrand_has_rdrand(void)
{
  SEXP has;
  PROTECT(has = allocVector(LGLSXP, 1));
  LOGICAL(has)[0]=has_rdrand();
  UNPROTECT(1);
  return(has);
}

__inline static unsigned int rdrand(void)
{
    unsigned int v;
    unsigned char r;
    /* rdrandl */
    if(has_rdrand()!=1) return (0);
#if defined(HAVE_X86_CPUID) && HAVE_X86_CPUID == 1
    do{
      r=0;
      __asm__ __volatile__ ( ".byte 0x0f,0xc7,0xf0;" "setc %0" : "=qm" (r), "=a" (v));
    }while(r==0);
#else
    v=0;
#endif
    return(v);
}


static double rn=0.0;
double *user_unif_rand ()
{
    int i;

    if(has_rdrand()!=1){
      rn=R_NaN;
      return(&rn);
    }
    if(poolavail == 0 ){
      for ( i=0; i<POOLSIZE ; i++ ){ /* 1.0/2^32  [0,1)*/
	poolbuf[i]=(double)rdrand() * 2.3283064365386963e-10;
      }
      poolavail=poolsize;poolpoint=0;
    }
    rn=poolbuf[poolpoint];
    poolavail--;
    poolpoint++;

    return &rn;
}
