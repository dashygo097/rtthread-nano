#include <rtthread.h>

static int tick = 0;

int main(void) {
  rt_kprintf("Hello World!\n");

  while (1) {
    rt_kprintf("[MAIN] tick %d\n", tick++);
    rt_thread_mdelay(1);
  }
  return 0;
}
