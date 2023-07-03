#include "../kernel/kernel.h"
#include "../process/process.h"
#include "disk.h"

void disk_read_request(process_t *proc, int track) {
  proc->remaining -= DISK_REQUEST_DURATION;
  interrupt_control(DISK_FINISH, (void *)proc);
}

void disk_write_request(process_t *proc, int track) {
  proc->remaining -= DISK_REQUEST_DURATION;
  interrupt_control(DISK_FINISH, (void *)proc);
}
