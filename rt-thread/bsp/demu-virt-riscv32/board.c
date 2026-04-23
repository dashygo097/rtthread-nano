#include <rthw.h>
#include <rtthread.h>

#define UART_TXD_REG (*(volatile unsigned char *)0x10000004u)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x20004000u)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x20004004u)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x2000BFF8u)

#define TICK_CYCLES 10000000u

extern void IRQ_Handler(void);

void SystemIrqHandler(rt_uint32_t mcause) {
  if (mcause == 0x80000007u) {
    unsigned int next = CLINT_MTIME_LO + TICK_CYCLES;
    CLINT_MTIMECMP_LO = next;

    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
  } else {
    while (1)
      ;
  }
}

void rt_hw_board_init(void) {
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
  rt_system_heap_init((void *)0x80041000u, (void *)0x80078000u);
#endif

  asm volatile("csrw mtvec, %0" ::"r"(IRQ_Handler));

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
