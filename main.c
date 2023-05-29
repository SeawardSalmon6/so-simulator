#include <stdio.h>

#include "kernel/kernel.h"
#include "terminal/terminal.h"

int main() {
  kernel_init();

  show_menu();
  return 0;
}
