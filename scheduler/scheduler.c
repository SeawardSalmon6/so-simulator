#include <stdio.h>
#include <stdlib.h>

#include "../helpers/list.h"
#include "../process/process.h"
#include "scheduler.h"

void scheduler_init(scheduler_t *scheduler) {
  scheduler->scheduled_process = NULL;
  scheduler->high_queue = scheduler_queue_init(QUANTUM_HIGH_QUEUE);
  scheduler->low_queue = scheduler_queue_init(QUANTUM_LOW_QUEUE);
  scheduler->blocked_queue = scheduler_queue_init(-1);
}

scheduler_queue_t *scheduler_queue_init(const int quantum) {
  scheduler_queue_t *queue = (scheduler_queue_t *)malloc(sizeof(scheduler_queue_t));

  if (!queue) {
    printf("\n--> Error: Not enough memory to allocate a scheduler queue!");
    exit(EXIT_FAILURE);
  }

  queue->queue = list_init();
  queue->quantum = quantum;

  return queue;
}

void schedule_process(scheduler_t *scheduler, int reason) {
  process_t *current_scheduled = scheduler->scheduled_process;
  process_t *new_scheduled = NULL;

  if (!is_list_empty(scheduler->high_queue->queue)) {
    new_scheduled = (process_t *)list_remove_head(scheduler->high_queue->queue)->content;
    if (new_scheduled)
      new_scheduled->remaining =
      1000 >= (QUANTUM_HIGH_QUEUE <= new_scheduled->remaining + scheduler->high_queue->quantum ? QUANTUM_HIGH_QUEUE : new_scheduled->remaining + scheduler->high_queue->quantum)
      ? 1000 : (QUANTUM_HIGH_QUEUE <= new_scheduled->remaining + scheduler->high_queue->quantum ? QUANTUM_HIGH_QUEUE : new_scheduled->remaining + scheduler->high_queue->quantum);
  } else if (!is_list_empty(scheduler->low_queue->queue)) {
    new_scheduled = (process_t *)list_remove_head(scheduler->low_queue->queue)->content;
    if (new_scheduled)
      new_scheduled->remaining =
      0 >= (QUANTUM_LOW_QUEUE <= new_scheduled->remaining + scheduler->low_queue->quantum ? QUANTUM_LOW_QUEUE : new_scheduled->remaining + scheduler->low_queue->quantum)
      ? 0 : (QUANTUM_LOW_QUEUE <= new_scheduled->remaining + scheduler->low_queue->quantum ? QUANTUM_LOW_QUEUE : new_scheduled->remaining + scheduler->low_queue->quantum);
  }

  if (current_scheduled) {
    if ((reason == IO_REQUESTED) || (reason == SEMAPHORE_BLOCKED)) {
      list_add(scheduler->blocked_queue->queue, current_scheduled);
      current_scheduled->state = BLOCKED;
    } else if ((reason == QUANTUM_COMPLETED)) {
      list_add(scheduler->high_queue->queue, current_scheduled);
      current_scheduled->state = READY;
    }
  }

  if (new_scheduled) {
    new_scheduled->state = RUNNING;
  }

  scheduler->scheduled_process = new_scheduled;
}

void schedule_unblock_process(scheduler_t *scheduler, process_t *proc, scheduler_queue_flag_t queue_flag) {
  list_node_t *proc_node = process_list_node_search(scheduler->blocked_queue->queue, proc->id);

  if (!proc_node) {
    return;
  }

  list_remove_node(scheduler->blocked_queue->queue, proc_node);

  switch (queue_flag) {
    case HIGH_QUEUE:
    {
      list_add(scheduler->high_queue->queue, proc);
      break;
    }
    case LOW_QUEUE:
    {
      list_add(scheduler->low_queue->queue, proc);
      break;
    }
    default:
    {
      printf("\n--> Error: Unknown scheduler queue.");
      exit(EXIT_FAILURE);
    }
  }
}
