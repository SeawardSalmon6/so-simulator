#include "../kernel/kernel.h"
#include "../process/process.h"
#include "print.h"

void print_request(process_t *proc, int duration) {
  proc->remaining -= duration;
  interrupt_control(PRINT_FINISH, (void *)proc);
}
