#ifndef OS_SIMULATOR_TERMINAL_H
#define OS_SIMULATOR_TERMINAL_H

#ifdef WIN32
#define CLEAR "cls"
#elif WIN64
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#include "../helpers/list.h"
#include "../process/process.h"

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

extern int is_process_log_active;
extern int is_memory_log_active;

void show_menu(void);
void print_tasks(void);
void print_list_with_processes_info(const char *, list_t *);
void print_process_info(process_t *);

#endif // OS_SIMULATOR_TERMINAL_H
