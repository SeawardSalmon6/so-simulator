#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../helpers/constants.h"
#include "../kernel/kernel.h"
#include "terminal.h"

void clear_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void show_menu() {
  int op;
  char filename[MAX_STRING_SIZE];

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

    switch (op) {
      case TERM_CREATE_PROCESS: {
        printf("\n--> Insert filename (or path): ");
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
        break;
      }

      case TERM_SHOW_MEMORY_STATUS: {
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
