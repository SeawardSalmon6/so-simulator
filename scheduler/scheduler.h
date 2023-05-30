#ifndef OS_SIMULATOR_SCHEDULER_H
#define OS_SIMULATOR_SCHEDULER_H

#include "../process/process.h"

#define QUANTUM_HIGH_QUEUE (1000)
#define QUANTUM_LOW_QUEUE (2000)

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
  list_t *queue;
  int quantum;
} scheduler_queue_t;

typedef struct {
  process_t *scheduled_process;
  scheduler_queue_t *low_queue;
  scheduler_queue_t *high_queue;
  scheduler_queue_t *blocked_queue;
} scheduler_t;

void scheduler_init(scheduler_t *);
scheduler_queue_t *scheduler_queue_init(const int);
void schedule_process(scheduler_t *, int);
void schedule_unblock_process(scheduler_t *, process_t *, scheduler_queue_flag_t);

#endif // OS_SIMULATOR_SCHEDULER_H
