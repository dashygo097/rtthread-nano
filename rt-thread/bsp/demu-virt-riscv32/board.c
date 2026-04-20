#include <rtconfig.h>
#include <rthw.h>
#include <rtthread.h>

#define UART_TXD_REG (*(volatile unsigned char *)0x10000004u)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x20004000u)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x20004004u)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x2000BFF8u)
#define CLINT_MTIME_HI (*(volatile unsigned int *)0x2000BFFCu)

#define TICK_CYCLES 500000u

extern void IRQ_Handler(void);

void update_timer(void) {
  unsigned int lo = CLINT_MTIME_LO;
  unsigned int hi = CLINT_MTIME_HI;
  unsigned long long current_mtime = ((unsigned long long)hi << 32) | lo;
  unsigned long long next_mtime = current_mtime + TICK_CYCLES;

  CLINT_MTIMECMP_HI = 0xFFFFFFFFu;
  CLINT_MTIMECMP_LO = (unsigned int)(next_mtime & 0xFFFFFFFFu);
  CLINT_MTIMECMP_HI = (unsigned int)(next_mtime >> 32);
}

void SystemIrqHandler(rt_uint32_t mcause) {
  if (mcause == 0x80000007u) {
    update_timer();
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
  } else {
    while (1)
      ;
  }
}

void rt_hw_board_init(void) {
  void *heap_begin = (void *)0x80041000u;
  void *heap_end = (void *)0x80078000u;

  rt_system_heap_init(heap_begin, heap_end);

  asm volatile("csrw mtvec, %0" ::"r"(IRQ_Handler));

  update_timer();

  /* Enable Machine Timer Interrupt */
  asm volatile("li t0, 128 \n"
               "csrs mie, t0 \n" ::
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
