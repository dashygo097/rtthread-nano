/*
 * cpuport.c — demu RV32IM libcpu port
 *
 * Implements:
 *   rt_hw_stack_init()               — initialise a new thread's stack
 *   rt_hw_context_switch_interrupt() — request a context switch from ISR
 *   rt_hw_cpu_shutdown()             — halt
 */

#include "cpuport.h"
#include <rthw.h>
#include <rtthread.h>

/* ── globals used by context_gcc.S ─────────────────────────────────────── */
volatile rt_ubase_t rt_interrupt_from_thread = 0;
volatile rt_ubase_t rt_interrupt_to_thread = 0;
volatile rt_uint32_t rt_thread_switch_interrupt_flag = 0;

/*
 * Stack frame layout — must match context_gcc.S slot offsets exactly.
 *
 *  slot  0 : epc       (entry point of the thread)
 *  slot  1 : ra        (texit — called if thread returns)
 *  slot  2 : mstatus   (0x88 = MPIE|MIE for new threads)
 *  slot  3 : (unused — gp not context-switched)
 *  slot  4 : tp  (x4)
 *  slot  5 : t0  (x5)
 *  ...
 *  slot 10 : a0  (x10) — first function argument = parameter
 *  ...
 *  slot 31 : t6  (x31)
 */
struct rt_hw_stack_frame {
  rt_ubase_t epc;     /* slot  0 */
  rt_ubase_t ra;      /* slot  1 */
  rt_ubase_t mstatus; /* slot  2 */
  rt_ubase_t gp;      /* slot  3 — unused, zero-filled */
  rt_ubase_t tp;      /* slot  4 */
  rt_ubase_t t0;      /* slot  5 */
  rt_ubase_t t1;      /* slot  6 */
  rt_ubase_t t2;      /* slot  7 */
  rt_ubase_t s0_fp;   /* slot  8 */
  rt_ubase_t s1;      /* slot  9 */
  rt_ubase_t a0;      /* slot 10 — parameter */
  rt_ubase_t a1;      /* slot 11 */
  rt_ubase_t a2;      /* slot 12 */
  rt_ubase_t a3;      /* slot 13 */
  rt_ubase_t a4;      /* slot 14 */
  rt_ubase_t a5;      /* slot 15 */
  rt_ubase_t a6;      /* slot 16 */
  rt_ubase_t a7;      /* slot 17 */
  rt_ubase_t s2;      /* slot 18 */
  rt_ubase_t s3;      /* slot 19 */
  rt_ubase_t s4;      /* slot 20 */
  rt_ubase_t s5;      /* slot 21 */
  rt_ubase_t s6;      /* slot 22 */
  rt_ubase_t s7;      /* slot 23 */
  rt_ubase_t s8;      /* slot 24 */
  rt_ubase_t s9;      /* slot 25 */
  rt_ubase_t s10;     /* slot 26 */
  rt_ubase_t s11;     /* slot 27 */
  rt_ubase_t t3;      /* slot 28 */
  rt_ubase_t t4;      /* slot 29 */
  rt_ubase_t t5;      /* slot 30 */
  rt_ubase_t t6;      /* slot 31 */
};

/*
 * rt_hw_stack_init — build an initial stack frame for a new thread.
 *
 * @tentry     : thread entry function
 * @parameter  : argument passed to tentry (goes into a0)
 * @stack_addr : TOP of the stack buffer (high address)
 * @texit      : called if tentry returns (goes into ra)
 *
 * Returns the new stack pointer (pointing at the bottom of the frame).
 */
rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter,
                             rt_uint8_t *stack_addr, void *texit) {
  struct rt_hw_stack_frame *frame;
  rt_uint8_t *stk;
  int i;

  /* align down to REGBYTES */
  stk = stack_addr + sizeof(rt_ubase_t);
  stk = (rt_uint8_t *)RT_ALIGN_DOWN((rt_ubase_t)stk, REGBYTES);
  stk -= sizeof(struct rt_hw_stack_frame);

  frame = (struct rt_hw_stack_frame *)stk;

  /* fill with 0xdeadbeef to make stack overflows visible */
  for (i = 0; i < (int)(sizeof(struct rt_hw_stack_frame) / sizeof(rt_ubase_t));
       i++)
    ((rt_ubase_t *)frame)[i] = 0xdeadbeef;

  frame->epc = (rt_ubase_t)tentry;
  frame->ra = (rt_ubase_t)texit;
  frame->a0 = (rt_ubase_t)parameter;
  frame->gp = 0;

  /*
   * mstatus for a new thread:
   *   MPIE = 1 (bit 7) — interrupts were "enabled before trap"
   *   MIE  = 1 (bit 3) — will be set by rt_hw_context_switch_exit via ori
   *
   * context_gcc.S's rt_hw_context_switch_exit does:
   *   ori a0, a0(mstatus_slot), 0x88   → forces MPIE|MIE = 1
   * So storing 0x80 here is fine; the exit path will OR in 0x88 anyway.
   */
  frame->mstatus = 0x00000080u; /* MPIE=1, MIE will be set on first restore */

  return stk;
}

/*
 * rt_hw_context_switch_interrupt — request a context switch from inside an ISR.
 *
 * Sets the global from/to pointers and raises the flag.
 * The actual switch happens at the bottom of IRQ_Handler in context_gcc.S.
 */
RT_WEAK void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to) {
  if (rt_thread_switch_interrupt_flag == 0)
    rt_interrupt_from_thread = from;

  rt_interrupt_to_thread = to;
  rt_thread_switch_interrupt_flag = 1;
}

/* rt_hw_cpu_shutdown — called on fatal error */
RT_WEAK void rt_hw_cpu_shutdown(void) {
  rt_base_t level;
  rt_kprintf("shutdown...\n");
  level = rt_hw_interrupt_disable();
  while (level)
    RT_ASSERT(0);
}
