#ifndef OS_SIMULATOR_PROCESS_H
#define OS_SIMULATOR_PROCESS_H

#include <stdio.h>

#include "../helpers/list.h"
#include "../process/instruction.h"
#include "../semaphores/semaphores.h"

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
instruction_t *read_code(FILE *, char *, int *);

#endif // OS_SIMULATOR_PROCESS_H
