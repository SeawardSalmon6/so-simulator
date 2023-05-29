#include <stdio.h>
#include <stdlib.h>

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
