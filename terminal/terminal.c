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
        filename[strlen(filename) - 1] = '\0';

        if (access(filename, F_OK) == -1) {
          printf("\n--> Error: file doesn't exist or can't be read!");
          break;
        }

        sys_call(PROCESS_CREATE, (void *)filename);

        break;
      }

      case TERM_SHOW_TASKS: {
        is_process_log_active = 1;

        clock_gettime(CLOCK_REALTIME, &start);

        printf("\n\n======== CPU Running Tasks");

        do {
          clock_gettime(CLOCK_REALTIME, &end);

          if ((end.tv_sec - start.tv_sec) * ONE_SECOND_IN_NS + (end.tv_nsec - start.tv_nsec) > 2 * ONE_SECOND_IN_NS) {
            printf("\n\n--> Want to quit (y/n)?");
            c = getchar();

            printf("\n");
          } else {
            start = end;
          }
        } while (c != 'y');

        break;
      }

      case TERM_SHOW_MEMORY_STATUS: {
        is_memory_log_active = 1;

        clock_gettime(CLOCK_REALTIME, &start);

        printf("\n\n======== Memory Usage Status");

        do {
          clock_gettime(CLOCK_REALTIME, &end);

          if ((end.tv_sec - start.tv_sec) * ONE_SECOND_IN_NS + (end.tv_nsec - start.tv_nsec) > 2 * ONE_SECOND_IN_NS) {
            printf("\n\n--> Want to quit (y/n)?");
            c = getchar();

            printf("\n");
          } else {
            start = end;
          }
        } while (c != 'y');

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
