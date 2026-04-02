/*
 * cpuport.h — demu RV32IM libcpu port header
 *
 * Selects 32-bit register width for context_gcc.S macros.
 * No FPU, no SMP.
 */

#ifndef CPUPORT_H__
#define CPUPORT_H__

#include <rtconfig.h>

/* 32-bit registers */
#define STORE sw
#define LOAD lw
#define REGBYTES 4

#endif /* CPUPORT_H__ */
