#ifndef OS_SIMULATOR_INSTRUCTION_H
#define OS_SIMULATOR_INSTRUCTION_H

#include "../semaphores/semaphores.h"

typedef enum {
  EXEC,
  READ,
  WRITE,
  PRINT,
  SEM_P,
  SEM_V
} instruction_op_t;

typedef struct {
  instruction_op_t op;
  int value;
  char *semaphore;
} instruction_t;

void parse_instruction(instruction_t *, char *, semaphore_table_t *);

#endif // OS_SIMULATOR_INSTRUCTION_H
