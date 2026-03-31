#include <rthw.h>
#include <rtthread.h>

#define UART_TXD_REG (*(volatile unsigned char *)0x10000004)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x20004000)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x20004004)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x2000BFF8)

#define TICK_CYCLES 1000

extern int _ebss;

void rt_os_tick_callback(void) {
  rt_interrupt_enter();
  rt_tick_increase();
  rt_interrupt_leave();
}

void raw_print(const char *str) {
  while (*str) {
    UART_TXD_REG = *str++;
  }
}

void system_trap_handler(rt_ubase_t mcause, rt_ubase_t mepc) {
  if (mcause == 0x80000007) {
    unsigned int next = CLINT_MTIME_LO + TICK_CYCLES;
    CLINT_MTIMECMP_LO = next;
    volatile unsigned int dummy = CLINT_MTIMECMP_LO;
    (void)dummy;

    rt_os_tick_callback();
  } else {
    raw_print("\n[FATAL] UNHANDLED TRAP!\n");
    while (1)
      ;
  }
}

__attribute__((naked, aligned(4))) void trap_entry(void) {
  asm volatile("addi sp, sp, -16 \n"
               "sw ra, 0(sp)     \n"
               "csrr a0, mcause  \n"
               "csrr a1, mepc    \n"
               "call system_trap_handler \n"
               "lw ra, 0(sp)     \n"
               "addi sp, sp, 16  \n"
               "mret             \n");
}

void rt_hw_board_init(void) {
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
  rt_system_heap_init((void *)&_ebss, (void *)0x80040000);
#endif

  asm volatile("csrw mtvec, %0" ::"r"(trap_entry));

  CLINT_MTIMECMP_LO = CLINT_MTIME_LO + TICK_CYCLES;
  CLINT_MTIMECMP_HI = 0;

  asm volatile("li t0, 128 \n"
               "csrs mie, t0 \n"
               :
               :
               : "t0");

#ifdef RT_USING_COMPONENTS_INIT
  rt_components_board_init();
#endif
}

#ifdef RT_USING_CONSOLE
void rt_hw_console_output(const char *str) {
  while (*str) {
    if (*str == '\n')
      UART_TXD_REG = '\r';
    UART_TXD_REG = *str++;
  }
}
#endif
