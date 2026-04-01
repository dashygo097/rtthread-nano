#include <rtthread.h>

int counter = 0;

int main(void) {
  rt_kprintf("Hello World!\n");

  while (1) {
    rt_kprintf("[MAIN] Thread Tick %d...\n", counter);
    counter++;

    rt_thread_mdelay(1000);
  }
  return 0;
}
