#include <rthw.h>
#include <rtthread.h>

#define UART_TXD_REG (*(volatile unsigned char *)0x10000000)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x02004000)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x02004004)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x0200BFF8)

#define TICK_CYCLES 100000

extern int _ebss;
extern int __stack;

void SystemIrqHandler(rt_uint32_t mcause) {
  if (mcause == 0x80000007) {
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

extern void IRQ_Handler(void);

void rt_hw_board_init(void) {
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
  rt_system_heap_init((void *)0x80100000, (void *)0x80900000);
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
