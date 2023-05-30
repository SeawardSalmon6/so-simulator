#include "cpu/cpu.h"
#include "kernel/kernel.h"
#include "terminal/terminal.h"

int main(void) {
  kernel_init();
  cpu_init();
  show_menu();
  return 0;
}
