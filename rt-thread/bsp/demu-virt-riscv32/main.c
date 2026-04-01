#include <rtthread.h>

volatile int global_counter = 0;

int main(void) {
  rt_kprintf("\n==================================\n");
  rt_kprintf("Hello RT-Thread RISC-V World!\n");
  rt_kprintf("==================================\n\n");

  while (1) {
    rt_kprintf("[MAIN] Thread Tick %d...\n", global_counter);
    global_counter++;

    rt_thread_mdelay(10);
  }

  return 0;
}
