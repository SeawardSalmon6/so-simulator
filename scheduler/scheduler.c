#include <stdio.h>
#include <stdlib.h>

#include "../helpers/list.h"
#include "../process/process.h"
#include "scheduler.h"

void scheduler_init(scheduler_t *scheduler) {
  scheduler->scheduled_process = NULL;
  scheduler->high_queue = list_init();
  scheduler->low_queue = list_init();
  scheduler->blocked_queue = list_init();
}

void schedule_process(scheduler_t *scheduler, scheduler_flag_t reason) {
  process_t *current_scheduled = scheduler->scheduled_process;
  process_t *new_scheduled = NULL;

  if (!is_list_empty(scheduler->high_queue)) {
    new_scheduled = (process_t *)list_remove_head(scheduler->high_queue)->content;
  } else if (!is_list_empty(scheduler->low_queue)) {
    new_scheduled = (process_t *)list_remove_head(scheduler->low_queue)->content;
  }

  if (current_scheduled) {
    if ((reason == IO_REQUESTED) || (reason == SEMAPHORE_BLOCKED)) {
      list_add(scheduler->blocked_queue, (void *)current_scheduled, 0);
      current_scheduled->state = BLOCKED;
    } else if ((reason == QUANTUM_COMPLETED)) {
      if (current_scheduled->priority) {
        list_add(scheduler->high_queue, (void *)current_scheduled, 1);
      } else {
        list_add(scheduler->low_queue, (void *)current_scheduled, 1);
      }

      current_scheduled->state = READY;
    }
  }

  if (new_scheduled) {
    new_scheduled->state = RUNNING;
  }

  scheduler->scheduled_process = new_scheduled;
}

void schedule_unblock_process(scheduler_t *scheduler, process_t *proc, scheduler_queue_flag_t queue_flag) {
  list_node_t *proc_node = process_list_node_search(scheduler->blocked_queue, proc->id);

  if (!proc_node) {
    return;
  }

  list_remove_node(scheduler->blocked_queue, proc_node);

  switch (queue_flag) {
    case HIGH_QUEUE:
    {
      list_add(scheduler->high_queue, (void *)proc, 1);
      break;
    }
    case LOW_QUEUE:
    {
      list_add(scheduler->low_queue, (void *)proc, 1);
      break;
    }
    default:
    {
      printf("\n--> Error: Unknown scheduler queue.");
      exit(EXIT_FAILURE);
    }
  }
}
