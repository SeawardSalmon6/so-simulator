#include <stdio.h>
#include <stdlib.h>

#include "../memory/memory.h"
#include "../scheduler/scheduler.h"
#include "../process/instruction.h"
#include "../process/process.h"
#include "../semaphores/semaphores.h"
#include "../terminal/terminal.h"
#include "kernel.h"

kernel_t *kernel;

void kernel_init(void) {
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
      schedule_process(&kernel->scheduler, (scheduler_flag_t)((__intptr_t)arg));
      break;
    }

    case PROCESS_CREATE: {
      process_create((char *)arg);
      break;
    }

    case PROCESS_FINISH: {
      process_finish((process_t *)arg);
      break;
    }

    case MEM_LOAD_REQ: {
      memory_request_load((memory_request_t *)arg, &kernel->seg_table);
      interrupt_control(MEM_LOAD_FINISH, arg);
      break;
    }

    case SEMAPHORE_P: {
      semaphore_P((semaphore_t *)arg, kernel->scheduler.scheduled_process, sleep_proc);
      break;
    }

    case SEMAPHORE_V: {
      semaphore_V((semaphore_t *)arg, wakeup_proc);
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

      list_add(kernel->proc_table, (void *)process, 0);

      process->state = READY;
      if (process->priority) {
        list_add(kernel->scheduler.high_queue, (void *)process, 1);
      } else {
        list_add(kernel->scheduler.low_queue, (void *)process, 1);
      }

      if (kernel->scheduler.scheduled_process != NULL && kernel->scheduler.scheduled_process != process) {
        if (kernel->scheduler.scheduled_process->remaining > process->remaining) {
          sys_call(PROCESS_INTERRUPT, (void *)(int)QUANTUM_COMPLETED);
        }
      }

      break;
    }

    default: {
      printf("\n--> Unrecognized kernel function!");
      break;
    }
  }
}

void run_instruction(process_t *proc, instruction_t *instr) {
  switch (instr->op) {
    case EXEC: {
      proc->remaining -= instr->value;
      break;
    }

    case READ: {
      proc->remaining -= instr->value;
      break;
    }

    case WRITE: {
      proc->remaining -= instr->value;
      break;
    }

    case PRINT: {
      if (is_process_log_active) {
        printf("\nProcess (%s - %d): I'm printing! [%d]", proc->name, proc->id, instr->value);
      }

      proc->remaining -= instr->value;
      break;
    }

    case SEM_P: {
      sys_call(SEMAPHORE_P, (void *)semaphore_find(&kernel->sem_table, instr->semaphore));

      if (proc->state != BLOCKED) {
        proc->remaining = 0 >= proc->remaining - 200 ? 0 : proc->remaining - 200;
      }

      break;
    }

    case SEM_V: {
      sys_call(SEMAPHORE_V, (void *)semaphore_find(&kernel->sem_table, instr->semaphore));

      proc->remaining = 0 >= proc->remaining - 200 ? 0 : proc->remaining - 200;

      break;
    }
  }
}

void sleep_proc(void) {
  sys_call(PROCESS_INTERRUPT, (void *)(int)SEMAPHORE_BLOCKED);
}

void wakeup_proc(process_t *proc) {
  schedule_unblock_process(&kernel->scheduler, proc, proc->priority ? HIGH_QUEUE : LOW_QUEUE);
}
