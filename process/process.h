#ifndef OS_SIMULATOR_PROCESS_H
#define OS_SIMULATOR_PROCESS_H

#include <stdio.h>

#include "../helpers/list.h"

typedef enum {
  NEW,
  BLOCKED,
  READY,
  RUNNING,
  TERMINATED
} process_state_t;

typedef struct {
  char *name;
  int id;
  int pc;
  int code_length;
  int segment_id;
  int segment_size;
  int priority;
  int remaining;
  process_state_t state;
  char **semaphores;
} process_t;

void process_create(char *);
process_t *parse_synthetic_program(FILE *, char *);
void read_semaphores(process_t *, char *);
list_node_t *process_list_node_search(list_t *list, int proc_id);
void process_finish(process_t *);

#endif // OS_SIMULATOR_PROCESS_H
