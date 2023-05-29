#ifndef OS_SIMULATOR_TERMINAL_H
#define OS_SIMULATOR_TERMINAL_H

#define TERM_CREATE_PROCESS_LABEL "Create New Process"
#define TERM_SHOW_TASKS_LABEL "Show Running Processes"
#define TERM_SHOW_MEMORY_STATUS_LABEL "Show Memory Status"
#define TERM_EXIT_LABEL "Exit"

typedef enum {
  TERM_EXIT,
  TERM_CREATE_PROCESS,
  TERM_SHOW_TASKS,
  TERM_SHOW_MEMORY_STATUS,
  TERM_OPTIONS_LENGTH // If there is no = above, indicates the size of menu
} terminal_options_t;

void show_menu();

#endif // OS_SIMULATOR_TERMINAL_H
