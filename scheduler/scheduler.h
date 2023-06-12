#ifndef OS_SIMULATOR_SCHEDULER_H
#define OS_SIMULATOR_SCHEDULER_H

#include "../process/process.h"

typedef enum {
  NONE,
  IO_REQUESTED,
  QUANTUM_COMPLETED,
  SEMAPHORE_BLOCKED
} scheduler_flag_t;

typedef enum {
  LOW_QUEUE,
  HIGH_QUEUE
} scheduler_queue_flag_t;

typedef struct {
  process_t *scheduled_process;
  list_t *low_queue;
  list_t *high_queue;
  list_t *blocked_queue;
} scheduler_t;

void scheduler_init(scheduler_t *);
void schedule_process(scheduler_t *, scheduler_flag_t);
void schedule_unblock_process(scheduler_t *, process_t *, scheduler_queue_flag_t);

#endif // OS_SIMULATOR_SCHEDULER_H
