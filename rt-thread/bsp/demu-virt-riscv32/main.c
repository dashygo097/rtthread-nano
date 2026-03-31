#include <rtthread.h>

int main(void) {
  rt_kprintf("Hello World!\n");

  int counter = 0;
  while (1) {
    rt_kprintf("[MAIN] Thread Tick %d...\n", counter++);
    rt_thread_mdelay(1000);
  }

  return 0;
}
