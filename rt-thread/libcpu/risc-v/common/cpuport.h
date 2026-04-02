#ifndef CPUPORT_H__
#define CPUPORT_H__

#include <rtconfig.h>

#ifdef ARCH_CPU_64BIT
#define STORE sd
#define LOAD ld
#define REGBYTES 8
#else
#define STORE sw
#define LOAD lw
#define REGBYTES 4
#endif

#ifdef ARCH_RISCV_FPU
#ifdef ARCH_RISCV_FPU_D
#define FSTORE fsd
#define FLOAD fld
#define FREGBYTES 8
#define rv_floatreg_t rt_int64_t
#endif
#ifdef ARCH_RISCV_FPU_S
#define FSTORE fsw
#define FLOAD flw
#define FREGBYTES 4
#define rv_floatreg_t rt_int32_t
#endif
#endif

#endif
