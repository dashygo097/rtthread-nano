/*
 * context_gcc.S — demu RV32I context switch for RT-Thread Nano
 */

#include "cpuport.h"

    .text
    .align 2

/* ── rt_hw_interrupt_disable ──────────────────────────────────────────────── */
    .globl rt_hw_interrupt_disable
rt_hw_interrupt_disable:
    csrrci  a0, mstatus, 8
    ret

/* ── rt_hw_interrupt_enable ───────────────────────────────────────────────── */
    .globl rt_hw_interrupt_enable
rt_hw_interrupt_enable:
    csrw    mstatus, a0
    ret

/* ── rt_hw_context_switch_to ──────────────────────────────────────────────── */
/*
 * void rt_hw_context_switch_to(rt_ubase_t to);
 * a0 = &to_thread->sp
 */
    .globl rt_hw_context_switch_to
rt_hw_context_switch_to:
    LOAD    sp,  0(a0)
    j       rt_hw_context_switch_exit

/* ── rt_hw_context_switch ───────────────���─────────────────────────────────── */
/*
 * void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to);
 * a0 = &from_thread->sp
 * a1 = &to_thread->sp
 */
    .globl rt_hw_context_switch
rt_hw_context_switch:
    addi    sp,  sp, -32 * REGBYTES

    /* save sp of from-thread */
    STORE   sp,  0(a0)

    /* slot 0: epc — for a voluntary switch, return address is ra */
    STORE   x1,  0 * REGBYTES(sp)
    /* slot 1: ra */
    STORE   x1,  1 * REGBYTES(sp)

    /* slot 2: mstatus — encode whether MIE was on as MPIE bit (0x80) */
    csrr    t0,  mstatus
    andi    t0,  t0, 8
    beqz    t0,  1f
    li      t0,  0x80
1:
    STORE   t0,  2 * REGBYTES(sp)

    STORE   x4,   4 * REGBYTES(sp)
    STORE   x5,   5 * REGBYTES(sp)
    STORE   x6,   6 * REGBYTES(sp)
    STORE   x7,   7 * REGBYTES(sp)
    STORE   x8,   8 * REGBYTES(sp)
    STORE   x9,   9 * REGBYTES(sp)
    STORE   x10, 10 * REGBYTES(sp)
    STORE   x11, 11 * REGBYTES(sp)
    STORE   x12, 12 * REGBYTES(sp)
    STORE   x13, 13 * REGBYTES(sp)
    STORE   x14, 14 * REGBYTES(sp)
    STORE   x15, 15 * REGBYTES(sp)
    STORE   x16, 16 * REGBYTES(sp)
    STORE   x17, 17 * REGBYTES(sp)
    STORE   x18, 18 * REGBYTES(sp)
    STORE   x19, 19 * REGBYTES(sp)
    STORE   x20, 20 * REGBYTES(sp)
    STORE   x21, 21 * REGBYTES(sp)
    STORE   x22, 22 * REGBYTES(sp)
    STORE   x23, 23 * REGBYTES(sp)
    STORE   x24, 24 * REGBYTES(sp)
    STORE   x25, 25 * REGBYTES(sp)
    STORE   x26, 26 * REGBYTES(sp)
    STORE   x27, 27 * REGBYTES(sp)
    STORE   x28, 28 * REGBYTES(sp)
    STORE   x29, 29 * REGBYTES(sp)
    STORE   x30, 30 * REGBYTES(sp)
    STORE   x31, 31 * REGBYTES(sp)

    /* load to-thread sp */
    LOAD    sp,  0(a1)
    j       rt_hw_context_switch_exit

/* ── rt_hw_context_switch_exit ────────────────────────────────────────────── */
/*
 * Restore full register frame from sp and mret.
 *
 * CRITICAL ordering to avoid spurious re-entry:
 *   1. Set mepc first (no side-effects)
 *   2. Restore all GPRs
 *   3. Write mstatus LAST — this re-enables MIE; the very next instruction
 *      must be mret so no interrupt can sneak in between.
 */
    .globl rt_hw_context_switch_exit
rt_hw_context_switch_exit:
    /* 1. restore mepc */
    LOAD    t0,  0 * REGBYTES(sp)
    csrw    mepc, t0

    /* 2. restore GPRs (skip t0/x5 for now — restore after mstatus is staged) */
    LOAD    x1,   1 * REGBYTES(sp)   /* ra  */
    /* slot 2 (mstatus) loaded into t0 below — do it last */
    LOAD    x4,   4 * REGBYTES(sp)
    /* x5/t0 = slot 5 — load AFTER we finish using t0 for mstatus */
    LOAD    x6,   6 * REGBYTES(sp)
    LOAD    x7,   7 * REGBYTES(sp)
    LOAD    x8,   8 * REGBYTES(sp)
    LOAD    x9,   9 * REGBYTES(sp)
    LOAD    x10, 10 * REGBYTES(sp)
    LOAD    x11, 11 * REGBYTES(sp)
    LOAD    x12, 12 * REGBYTES(sp)
    LOAD    x13, 13 * REGBYTES(sp)
    LOAD    x14, 14 * REGBYTES(sp)
    LOAD    x15, 15 * REGBYTES(sp)
    LOAD    x16, 16 * REGBYTES(sp)
    LOAD    x17, 17 * REGBYTES(sp)
    LOAD    x18, 18 * REGBYTES(sp)
    LOAD    x19, 19 * REGBYTES(sp)
    LOAD    x20, 20 * REGBYTES(sp)
    LOAD    x21, 21 * REGBYTES(sp)
    LOAD    x22, 22 * REGBYTES(sp)
    LOAD    x23, 23 * REGBYTES(sp)
    LOAD    x24, 24 * REGBYTES(sp)
    LOAD    x25, 25 * REGBYTES(sp)
    LOAD    x26, 26 * REGBYTES(sp)
    LOAD    x27, 27 * REGBYTES(sp)
    LOAD    x28, 28 * REGBYTES(sp)
    LOAD    x29, 29 * REGBYTES(sp)
    LOAD    x30, 30 * REGBYTES(sp)
    LOAD    x31, 31 * REGBYTES(sp)

    /* 3. stage mstatus in t0, restore t0 (x5), advance sp, write mstatus, mret */
    LOAD    t0,  2 * REGBYTES(sp)    /* t0 = saved mstatus slot (0 or 0x80)  */
    ori     t0,  t0, 0x88            /* force MPIE=1 | MIE=1                 */
    /* now restore x5 slot before we lose sp */
    /* we need a scratch — use the about-to-be-overwritten sp area safely:   */
    /* save t0 (our mstatus value) into mscratch temporarily                 */
    csrw    mscratch, t0
    LOAD    x5,  5 * REGBYTES(sp)    /* restore t0/x5 from its own slot      */
    addi    sp,  sp, 32 * REGBYTES   /* deallocate frame                     */
    /* now: all GPRs restored, sp correct, mstatus value in mscratch         */
    csrrw   t0,  mscratch, x0        /* t0 = mstatus value; mscratch = 0     */
    csrw    mstatus, t0              /* re-enable interrupts — mret must follow immediately */
    mret

/* ── IRQ_Handler ──────────────────────────────────────────────────────────── */
/*
 * Raw M-mode trap entry. Installed into mtvec by board.c (direct mode).
 * Must be 4-byte aligned.
 *
 * On entry: all registers live, sp = interrupted thread's sp, MIE=0 (CPU
 * cleared it on trap entry per spec).
 */
    .globl IRQ_Handler
    .align 2
IRQ_Handler:
    /* ── allocate frame ── */
    addi    sp,  sp, -32 * REGBYTES

    /* ── save ra first (we'll overwrite it with mepc via csrr) ── */
    STORE   x1,  1 * REGBYTES(sp)

    /* slot 0: mepc */
    csrr    x1,  mepc
    STORE   x1,  0 * REGBYTES(sp)

    /* slot 2: mstatus MPIE (bit 7) — this is what MIE was before the trap */
    csrr    x1,  mstatus
    andi    x1,  x1, 0x80
    STORE   x1,  2 * REGBYTES(sp)

    /* restore ra now that we've used x1 as scratch */
    LOAD    x1,  1 * REGBYTES(sp)

    /* ── save all other GPRs ── */
    STORE   x4,   4 * REGBYTES(sp)
    STORE   x5,   5 * REGBYTES(sp)
    STORE   x6,   6 * REGBYTES(sp)
    STORE   x7,   7 * REGBYTES(sp)
    STORE   x8,   8 * REGBYTES(sp)
    STORE   x9,   9 * REGBYTES(sp)
    STORE   x10, 10 * REGBYTES(sp)
    STORE   x11, 11 * REGBYTES(sp)
    STORE   x12, 12 * REGBYTES(sp)
    STORE   x13, 13 * REGBYTES(sp)
    STORE   x14, 14 * REGBYTES(sp)
    STORE   x15, 15 * REGBYTES(sp)
    STORE   x16, 16 * REGBYTES(sp)
    STORE   x17, 17 * REGBYTES(sp)
    STORE   x18, 18 * REGBYTES(sp)
    STORE   x19, 19 * REGBYTES(sp)
    STORE   x20, 20 * REGBYTES(sp)
    STORE   x21, 21 * REGBYTES(sp)
    STORE   x22, 22 * REGBYTES(sp)
    STORE   x23, 23 * REGBYTES(sp)
    STORE   x24, 24 * REGBYTES(sp)
    STORE   x25, 25 * REGBYTES(sp)
    STORE   x26, 26 * REGBYTES(sp)
    STORE   x27, 27 * REGBYTES(sp)
    STORE   x28, 28 * REGBYTES(sp)
    STORE   x29, 29 * REGBYTES(sp)
    STORE   x30, 30 * REGBYTES(sp)
    STORE   x31, 31 * REGBYTES(sp)

    /* ── call C handler: SystemIrqHandler(mcause) ── */
    csrr    a0,  mcause
    call    SystemIrqHandler

    /* ── check for deferred context switch ── */
    la      t0,  rt_thread_switch_interrupt_flag
    lw      t1,  0(t0)
    beqz    t1,  irq_no_switch

    sw      zero, 0(t0)             /* clear flag                            */

    /*
     * Save current sp into *rt_interrupt_from_thread.
     * rt_interrupt_from_thread holds &from_thread->sp  (set by cpuport.c).
     * *rt_interrupt_from_thread = sp  saves the frame pointer.
     */
    la      t0,  rt_interrupt_from_thread
    lw      t1,  0(t0)              /* t1 = &from_thread->sp                 */
    sw      sp,  0(t1)              /* from_thread->sp = sp (save frame)     */

    /*
     * Load new sp from *rt_interrupt_to_thread.
     * rt_interrupt_to_thread holds &to_thread->sp.
     * sp = *rt_interrupt_to_thread  loads the saved frame pointer.
     */
    la      t0,  rt_interrupt_to_thread
    lw      t1,  0(t0)              /* t1 = &to_thread->sp                   */
    lw      sp,  0(t1)              /* sp = to_thread->sp (restore frame)    */

irq_no_switch:
    j       rt_hw_context_switch_exit
