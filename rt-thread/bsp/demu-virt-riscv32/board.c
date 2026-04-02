#include <rthw.h>
#include <rtthread.h>

#define UART_TXD (*(volatile unsigned int *)0x10000004u)
#define CLINT_MSIP (*(volatile unsigned int *)0x20000000u)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x20004000u)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x20004004u)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x2000BFF8u)
#define CLINT_MTIME_HI (*(volatile unsigned int *)0x2000BFFCu)

#ifndef TICK_CYCLES
#define TICK_CYCLES 1000u
#endif

extern unsigned int _ebss;
extern unsigned int __stack;

extern void IRQ_Handler(void);

void SystemIrqHandler(rt_uint32_t mcause) {
  if (mcause == 0x80000007u) {
    CLINT_MTIMECMP_HI = 0xFFFFFFFFu;
    CLINT_MTIMECMP_LO = CLINT_MTIME_LO + TICK_CYCLES;
    CLINT_MTIMECMP_HI = 0u;

    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
  } else if (mcause == 0x80000003u) {
    CLINT_MSIP = 0u;
  }
}

void rt_hw_board_init(void) {
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
  rt_uint8_t *heap_begin = (rt_uint8_t *)&_ebss;
  rt_uint8_t *heap_end = (rt_uint8_t *)((unsigned int)&__stack - 8192u);
  rt_system_heap_init(heap_begin, heap_end);
#endif

  CLINT_MTIMECMP_HI = 0xFFFFFFFFu;
  CLINT_MTIMECMP_LO = CLINT_MTIME_LO + TICK_CYCLES;
  CLINT_MTIMECMP_HI = 0u;

  __asm__ volatile("csrw mtvec, %0" ::"r"(IRQ_Handler) : "memory");

  __asm__ volatile("li   t0, 0x80  \n"
                   "csrw mie, t0   \n" ::
                       : "t0");

  __asm__ volatile("li   t0, 0x8       \n"
                   "csrs mstatus, t0   \n" ::
                       : "t0");

#ifdef RT_USING_COMPONENTS_INIT
  rt_components_board_init();
#endif
}

#ifdef RT_USING_CONSOLE
void rt_hw_console_output(const char *str) {
  while (*str) {
    if (*str == '\n')
      UART_TXD = (unsigned int)'\r';
    UART_TXD = (unsigned int)(unsigned char)*str++;
  }
}
#endif
