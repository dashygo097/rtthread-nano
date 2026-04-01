#include <rthw.h>
#include <rtthread.h>

#define UART_TXD_REG (*(volatile unsigned char *)0x10000004)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x20004000)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x20004004)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x2000BFF8)

#define TICK_CYCLES 10000

extern int _ebss;

void raw_print(const char *str) {
  while (*str) {
    UART_TXD_REG = *str++;
  }
}

void SystemIrqHandler(rt_uint32_t mcause) {
  if (mcause == 0x80000007) {
    extern void raw_print(const char *);
    raw_print("[IRQ] Timer Hit!\n");

    unsigned int next = CLINT_MTIME_LO + TICK_CYCLES;
    CLINT_MTIMECMP_LO = next;
    volatile unsigned int dummy = CLINT_MTIMECMP_LO;
    (void)dummy;

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
  rt_system_heap_init((void *)&_ebss, (void *)0x8003F000);
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
