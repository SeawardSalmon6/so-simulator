#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "../kernel/kernel.h"
#include "../process/instruction.h"
#include "../terminal/terminal.h"
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
  int no_process = 0, elapsed, pc, page_number, page_offset;
  segment_t *seg;
  page_t *page;
  instruction_t instruction;

  while (!kernel)
    ;

  clock_gettime(CLOCK_REALTIME, &start);

  while (1) {
    if (!kernel->scheduler.scheduled_process) {
      if (!no_process) {
        no_process = 1;
      }

      schedule_process(&kernel->scheduler, NONE);
    } else {
      no_process = 0;

      do {
        clock_gettime(CLOCK_REALTIME, &end);
        elapsed = (end.tv_sec - start.tv_sec) * ONE_SECOND_IN_NS + (end.tv_nsec - start.tv_nsec);

        if (elapsed >= ONE_SECOND_IN_NS / 2) {
          start = end;

          pc = kernel->scheduler.scheduled_process->pc++;
          page_number = PAGE_NUMBER(pc);
          page_offset = PAGE_OFFSET(pc);

          seg = segment_find(&kernel->seg_table, kernel->scheduler.scheduled_process->segment_id);
          page = &seg->page_table[page_number];
          instruction = page->code[page_offset];

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
      }
    }
  }

  return NULL;
}
