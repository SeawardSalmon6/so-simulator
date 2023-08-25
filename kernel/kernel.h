#ifndef OS_SIMULATOR_KERNEL_H
#define OS_SIMULATOR_KERNEL_H

#include <semaphore.h>

#include "../helpers/list.h"
#include "../memory/memory.h"
#include "../process/instruction.h"
#include "../process/process.h"
#include "../scheduler/scheduler.h"
#include "../semaphores/semaphores.h"

typedef enum {
  PROCESS_INTERRUPT = 1,
  PROCESS_CREATE = 2,
  PROCESS_FINISH = 3,
  DISK_READ_REQUEST = 4,
  DISK_WRITE_REQUEST = 16,
  DISK_FINISH = 5,
  MEM_LOAD_REQ = 6,
  MEM_LOAD_FINISH = 7,
  FS_REQUEST = 8,
  FS_FINISH = 9,
  SEMAPHORE_P = 10,
  SEMAPHORE_V = 11,
  PRINT_REQUEST = 14,
  PRINT_FINISH = 15
} kernel_function_t;

typedef struct {
  list_t *proc_table;
  int next_proc_id;
  segment_table_t seg_table;
  scheduler_t scheduler;
  semaphore_table_t sem_table;
  int pc;
} kernel_t;

extern sem_t mutex;
extern kernel_t *kernel;

void kernel_init(void);
void sys_call(kernel_function_t, void *);
void interrupt_control(kernel_function_t, void *);
void run_instruction(process_t *, instruction_t *);
void sleep_proc(void);
void wakeup_proc(process_t *);

#endif // OS_SIMULATOR_KERNEL_H
