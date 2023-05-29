#include <stdio.h>
#include <stdlib.h>

#include "../memory/memory.h"
#include "../scheduler/scheduler.h"
#include "../semaphores/semaphores.h"
#include "kernel.h"

kernel_t *kernel;

void kernel_init() {
  kernel = (kernel_t *)malloc(sizeof(kernel_t));

  if (!kernel) {
    printf("\n--> Error: Not enough memory to allocate kernel!");
    exit(EXIT_FAILURE);
  }

  kernel->next_proc_id = 1;
  kernel->proc_table = list_init();

  segment_table_init(&kernel->seg_table);
  scheduler_init(&kernel->scheduler);
  semaphore_table_init(&kernel->sem_table);
}

void sys_call(kernel_function_t func, void *arg) {
  switch (func) {
    case PROCESS_INTERRUPT: {
      break;
    }

    case PROCESS_CREATE: {
      process_create((char *)arg);
      break;
    }

    case PROCESS_FINISH: {
      break;
    }

    case MEM_LOAD_REQ: {
      memory_request_load((memory_request_t *)arg, &kernel->seg_table);
      interrupt_control(MEM_LOAD_FINISH, arg);
      break;
    }

    case SEMAPHORE_P: {
      break;
    }

    case SEMAPHORE_V: {
      break;
    }

    default: {
      printf("\n--> Unrecognized kernel function!");
      break;
    }
  }
}

void interrupt_control(kernel_function_t func, void *arg) {
  switch (func) {
    case MEM_LOAD_FINISH: {
      memory_request_t *request = (memory_request_t *)arg;
      process_t *process = request->process;

      list_add(kernel->proc_table, (void *)process);

      process->state = READY;
      if (process->priority == 1) {
        list_add(kernel->scheduler.high_queue->queue, (void *)process);
      } else {
        list_add(kernel->scheduler.low_queue->queue, (void *)process);
      }

      break;
    }

    default: {
      printf("\n--> Unrecognized kernel function!");
      break;
    }
  }
}
