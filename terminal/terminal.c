#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../cpu/cpu.h"
#include "../helpers/constants.h"
#include "../kernel/kernel.h"
#include "../memory/memory.h"
#include "terminal.h"

void clear_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void show_menu(void) {
  int op;
  char filename[MAX_STRING_SIZE];

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
        print_memory_usage();
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
  } while (op != TERM_EXIT);
}

void print_tasks(void) {
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

      printf("| %02d | %s | %d | %s | %s\n",
        proc->id,
        proc->name,
        proc->remaining,
        proc->priority ? "yep" : "nope",
        proc->state == NEW ? "new" : proc->state == BLOCKED ? "blocked" : proc->state == READY ? "ready" : "running"
      );

      aux = aux->next;
    }
  } else {
    printf("                  No process here                \n");
  }

  printf("---------------------------------------------------\n");
}

void print_memory_usage(void) {
  char c;

  do {
    if (kernel->seg_table.remaining == MAX_MEM_SIZE) {
      printf("\n\n--> There is no memory usage at the moment!");
      break;
    }

    system(CLEAR);

    printf("\n\n======== Memory Usage");

    printf("\n\n-----> Total Memory Usage:\n");
    printf("  > Total Memory: %.2f KB\n", ((double)MAX_MEM_SIZE) / KILOBYTE);

    printf("  > Memory In Use: %.2f KB (%.2f%%)\n",
      ((double)(MAX_MEM_SIZE - kernel->seg_table.remaining)) / KILOBYTE,
      (((double)(MAX_MEM_SIZE - kernel->seg_table.remaining)) / KILOBYTE) / MAX_MEM_SIZE * 100
    );

    printf("  > Available Memory: %.2f KB\n", ((double)kernel->seg_table.remaining / KILOBYTE));

    print_list_with_segments_info(kernel->seg_table.segment_list);

    printf("\n\n--> Want to go back to main menu (y/n)? ");
    scanf(" %c", &c);
  } while (c != 'y');
}

void print_list_with_segments_info(list_t *list) {
  segment_t *seg;
  list_node_t *aux = list->head;

  printf("\n\n-----> Segments Table\n");
  printf("----------------------------\n");
  printf("| id | seg_size | page_count\n");
  printf("----------------------------\n");

  while (aux) {
    seg = (segment_t *)aux->content;

    printf("| %02d | %dKB | %d\n", seg->id, seg->size / KILOBYTE, seg->page_count);
    print_list_with_pages_info(seg->page_table, seg->page_count);

    aux = aux->next;
  }

  printf("----------------------------\n");
}

void print_list_with_pages_info(page_t *list, int list_size) {
  for (int i = 0; i < list_size; i++) {
    printf("   > page_%02d (%s)\n", i, list[i].used ? "used" : "not used");
  }
  printf("\n");
}
