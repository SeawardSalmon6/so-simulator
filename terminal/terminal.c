#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../cpu/cpu.h"
#include "../helpers/constants.h"
#include "../kernel/kernel.h"
#include "terminal.h"

int is_process_log_active = 0;
int is_memory_log_active = 0;

void clear_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void show_menu(void) {
  int op;
  char c, filename[MAX_STRING_SIZE];

  struct timespec start;
  struct timespec end;

  do {
    system(CLEAR);
    fflush(stdout);

    do {
      printf("\n\n========= OS SIMULATOR");
      printf("\n(%d) %s", TERM_CREATE_PROCESS, TERM_CREATE_PROCESS_LABEL);
      printf("\n(%d) %s", TERM_SHOW_TASKS, TERM_SHOW_TASKS_LABEL);
      printf("\n(%d) %s", TERM_SHOW_MEMORY_STATUS, TERM_SHOW_MEMORY_STATUS_LABEL);
      printf("\n(%d) %s", TERM_EXIT, TERM_EXIT_LABEL);

      printf("\n\n--> Select an option: ");
      scanf(" %d", &op);

      clear_buffer(); // Clears stdin for string reading
    } while (op < 0 || op > TERM_OPTIONS_LENGTH - 1);

    system(CLEAR);

    switch (op) {
      case TERM_CREATE_PROCESS: {
        printf("\n--> Insert filename: ");
        fgets(filename, MAX_STRING_SIZE, stdin);
        filename[strcspn(filename, "\n")] = '\0';

        if (access(filename, F_OK) == -1) {
          printf("\n--> Error: file doesn't exist or can't be read!");
          break;
        }

        sys_call(PROCESS_CREATE, (void *)filename);

        break;
      }

      case TERM_SHOW_TASKS: {
        print_tasks();
        break;
      }

      case TERM_SHOW_MEMORY_STATUS: {
        c = 'y';
        is_memory_log_active = 1;

        clock_gettime(CLOCK_REALTIME, &start);

        printf("\n\n======== Memory Usage Status");

        do {
          clock_gettime(CLOCK_REALTIME, &end);

          if (kernel->seg_table.remaining == 1 * GIGABYTE) {
            printf("\n--> There is no memory usage at the moment!");
            break;
          }

          if ((end.tv_sec - start.tv_sec) * ONE_SECOND_IN_NS + (end.tv_nsec - start.tv_nsec) > 8 * ONE_SECOND_IN_NS) {
            is_memory_log_active = 0;

            printf("\n\n--> Want to continue (y/n)? ");
            scanf(" %c", &c);

            if (c == 'y') {
              start = end;
              is_memory_log_active = 1;
            }
          }
        } while (c != 'n');

        break;
      }

      case TERM_EXIT: {
        printf("\n--> We hope you enjoyed using our OS!\n\n");
        break;
      }

      default: {
        printf("\n--> Unrecognized option!");
        break;
      }
    }

    is_process_log_active = 0;
    is_memory_log_active = 0;
  } while (op != TERM_EXIT);
}

void print_tasks() {
  char c;

  do {
    if (!kernel->proc_table->head) {
      printf("\n\n--> There is no process running or scheduled!");
      break;
    }

    system(CLEAR);

    printf("\n\n======== CPU Running Tasks");

    if (kernel->scheduler.scheduled_process) {
      printf("\n\n-----> Current Process Running: %02d | %s\n",
        kernel->scheduler.scheduled_process->id,
        kernel->scheduler.scheduled_process->name
      );
    } else {
      printf("\n\n-----> Current Process Running: Scheduling...\n");
    }

    print_list_with_processes_info("Processes Table", kernel->proc_table);
    print_list_with_processes_info("High Queue", kernel->scheduler.high_queue);
    print_list_with_processes_info("Low Queue", kernel->scheduler.low_queue);
    print_list_with_processes_info("Blocked Queue", kernel->scheduler.blocked_queue);

    printf("\n\n--> Want to go back to main menu (y/n)? ");
    scanf(" %c", &c);
  } while (c != 'y');
}

void print_process_info(process_t *proc) {
  printf("| %02d | %s | %d | %s | %s\n",
    proc->id,
    proc->name,
    proc->remaining,
    proc->priority ? "yep" : "nope",
    proc->state == NEW ? "new" : proc->state == BLOCKED ? "blocked" : proc->state == READY ? "ready" : "running"
  );
}

void print_list_with_processes_info(const char *title, list_t *list) {
  process_t *proc;
  list_node_t *aux = list->head;

  printf("\n\n-----> %s\n", title);
  printf("---------------------------------------------------\n");
  printf("| id | name | remaining_time | is_priority | status\n");
  printf("---------------------------------------------------\n");

  if (list->head) {
    while (aux) {
      proc = (process_t *)aux->content;
      print_process_info(proc);
      aux = aux->next;
    }
  } else {
    printf("                  No process here                \n");
  }

  printf("---------------------------------------------------\n");
}
