#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "../kernel/kernel.h"
#include "../process/instruction.h"
#include "cpu.h"

void cpu_init(void) {
  pthread_t cpu_id;
  pthread_attr_t cpu_attr;

  pthread_attr_init(&cpu_attr);
  pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&cpu_id, NULL, cpu, NULL);
}

void *cpu(void *arg) {
  struct timespec start, end;
  int number_of_processes = 0;

  while (!kernel)
    ;


  clock_gettime(CLOCK_REALTIME, &start);

  while (1) {
    if (kernel->scheduler.scheduled_process) {
      if (!number_of_processes) {
        number_of_processes = 1;
      }

      schedule_process(&kernel->scheduler, NONE);
    } else {
      number_of_processes = 0;

      do {
        clock_gettime(CLOCK_REALTIME, &end);
        const int elapsed = (end.tv_sec - start.tv_sec) * ONE_SECOND_IN_NS + (end.tv_nsec - start.tv_nsec);

        if (elapsed >= ONE_SECOND_IN_NS) {
          start = end;

          const int pc = kernel->scheduler.scheduled_process->pc++;
          const int page_number = PAGE_NUMBER(pc);
          const int page_offset = PAGE_OFFSET(pc);

          segment_t *seg = segment_find(&kernel->seg_table, kernel->scheduler.scheduled_process->segment_id);
          page_t *page = &seg->page_table[page_number];
          instruction_t instruction = page->code[page_offset];

          if (!page->used) {
            page->used = 1;
          }

          run_instruction(kernel->scheduler.scheduled_process, &instruction);
        }
      } while (
        kernel->scheduler.scheduled_process != NULL
        && kernel->scheduler.scheduled_process->remaining > 0
        && kernel->scheduler.scheduled_process->pc < kernel->scheduler.scheduled_process->code_length
        );

      if (kernel->scheduler.scheduled_process == NULL) {
        continue;
      }

      if (kernel->scheduler.scheduled_process->pc >= kernel->scheduler.scheduled_process->code_length) {
        sys_call(PROCESS_FINISH, (void *)kernel->scheduler.scheduled_process);
      } else {
        sys_call(PROCESS_INTERRUPT, (void *)QUANTUM_COMPLETED);
      }
    }
  }

  return NULL;
}
