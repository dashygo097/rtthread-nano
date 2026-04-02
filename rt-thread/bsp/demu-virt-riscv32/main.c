#include <rtthread.h>

static int tick = 0;

int main(void) {
  rt_kprintf("RT-Thread Nano booted on demu RV32IM!\n");

  while (1) {
    rt_kprintf("[MAIN] tick %d\n", tick++);
    rt_thread_mdelay(10);
  }
  return 0;
}
