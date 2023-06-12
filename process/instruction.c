#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../helpers/constants.h"
#include "../semaphores/semaphores.h"
#include "instruction.h"

void parse_semaphore_instruction(instruction_t *instruction, char *line, semaphore_table_t *sem_table) {
  semaphore_t *sem;
  char *sem_name = duplicate_str(line);

  sem_name = sem_name + 2;
  sem_name[strlen(sem_name) - 2] = '\0';

  sem = semaphore_find(sem_table, sem_name);

  if (!sem) {
    printf("\n--> Error: A semaphore with the name %s has not been registered.\n", sem_name);
    exit(EXIT_FAILURE);
  }

  instruction->op = line[0] == 'P' ? SEM_P : SEM_V;
  instruction->semaphore = sem_name;
}

void parse_instruction(instruction_t *instruction, char *line, int *remaining_time, semaphore_table_t *sem_table) {
  if (line[0] == 'P' || line[0] == 'V') {
    parse_semaphore_instruction(instruction, line, sem_table);
    (*remaining_time) += 200;
  } else {
    char *dupline = duplicate_str(line);
    char *left_op = strtok(dupline, " ");
    int right_op = atoi(strtok(NULL, " "));

    if (strcmp(left_op, "exec") == 0) {
      instruction->op = EXEC;
    } else if (strcmp(left_op, "read") == 0) {
      instruction->op = READ;
    } else if (strcmp(left_op, "write") == 0) {
      instruction->op = WRITE;
    } else if (strcmp(left_op, "print") == 0) {
      instruction->op = PRINT;
    }

    (*remaining_time) += right_op;
    instruction->value = right_op;
    instruction->semaphore = NULL;
  }
}
