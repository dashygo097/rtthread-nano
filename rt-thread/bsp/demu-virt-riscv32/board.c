#include <rthw.h>
#include <rtthread.h>

#define UART_TXD_REG (*(volatile unsigned char *)0x10000004)
#define CLINT_MTIMECMP_LO (*(volatile unsigned int *)0x02004000)
#define CLINT_MTIMECMP_HI (*(volatile unsigned int *)0x02004004)
#define CLINT_MTIME_LO (*(volatile unsigned int *)0x0200BFF8)

#define TICK_CYCLES 10000

extern int _ebss;
extern int __stack;

#define STACK_SIZE 0x2000

#define HEAP_BEGIN ((void *)&_ebss)
#define HEAP_END ((void *)((rt_uint32_t) & __stack - STACK_SIZE))

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
  rt_system_heap_init(HEAP_BEGIN, HEAP_END);
  rt_kprintf("Heap: 0x%08x - 0x%08x (%d bytes)\n", HEAP_BEGIN, HEAP_END,
             (rt_uint32_t)HEAP_END - (rt_uint32_t)HEAP_BEGIN);
#endif

  asm volatile("csrw mtvec, %0" ::"r"(IRQ_Handler));

  CLINT_MTIMECMP_LO = CLINT_MTIME_LO + TICK_CYCLES;
  CLINT_MTIMECMP_HI = 0;

  asm volatile("li t0, 0x80    \n"
               "csrs mie, t0   \n" ::
                   : "t0");

  asm volatile("li t0, 0x8     \n"
               "csrs mstatus, t0\n" ::
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
