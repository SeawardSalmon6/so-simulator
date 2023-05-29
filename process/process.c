#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../helpers/constants.h"
#include "../kernel/kernel.h"
#include "../memory/memory.h"

#include "instruction.h"
#include "process.h"

void process_create(char *filename) {
  char buf[MAX_STRING_SIZE];
  process_t *process;
  instruction_t *code;
  FILE *fp = fopen(filename, "r");

  if (!fp) {
    printf("\n--> Error: The synthetic file at %s could not be opened!", filename);
    exit(EXIT_FAILURE);
  }

  process = parse_synthetic_program(fp, buf);
  code = read_code(fp, buf, &process->code_length);

  memory_request_t memory_request;
  memory_request_init(&memory_request, process, code);

  sys_call(MEM_LOAD_REQ, (void *)&memory_request);
}

process_t *parse_synthetic_program(FILE *fp, char *buf) {
  process_t *process = (process_t *)malloc(sizeof(process_t));

  if (!process) {
    printf("\n--> Error: Not enough memory to allocate process!");
    exit(EXIT_FAILURE);
  }

  process->id = kernel->next_proc_id++;
  process->pc = 0;
  process->state = NEW;
  process->remaining = 0;

  fgets(buf, MAX_STRING_SIZE, stdin);
  buf[strlen(buf) - 1] = '\0';
  process->name = duplicate_str(buf);

  fgets(buf, MAX_STRING_SIZE, stdin);
  process->segment_id = atoi(buf);

  fgets(buf, MAX_STRING_SIZE, stdin);
  process->priority = atoi(buf);

  fgets(buf, MAX_STRING_SIZE, stdin);
  process->segment_size = atoi(buf);

  fgets(buf, MAX_STRING_SIZE, stdin);

  if (strcmp(buf, "\n") != 0) {
    buf[strlen(buf) - 1] = '\0';
    read_semaphores(process, buf);

    fgets(buf, MAX_STRING_SIZE, fp);
  }

  return process;
}

void read_semaphores(process_t *process, char *line) {
  const int length = strlen(line);
  int semaphores_count = 1;
  int i;

  for (i = 0; i < length; i++) {
    if (line[i] == ' ') {
      semaphores_count++;
    }
  }

  process->semaphores = (char **)malloc(sizeof(char *) * semaphores_count);

  if (!process->semaphores) {
    printf("\n--> Error: Not enough memory to allocate semaphores!");
    exit(EXIT_FAILURE);
  }

  char *tok = strtok(line, " ");

  i = 0;
  do {
    process->semaphores[i] = duplicate_str(tok);
    semaphore_register(&kernel->sem_table, process->semaphores[i]);
    i++;
  } while ((tok = strtok(NULL, " ")));
}

instruction_t *read_code(FILE *fp, char *buf, int *code_length) {
  int i, buflen;
  long int code_section;
  instruction_t *code;

  code_section = ftell(fp);
  (*code_length) = 0;

  while (fgets(buf, MAX_STRING_SIZE, fp)) {
    (*code_length)++;
  }

  fseek(fp, code_section, SEEK_SET);

  code = (instruction_t *)malloc((*code_length) * sizeof(instruction_t));

  if (!code) {
    printf("\n--> Error: Not enough memory to allocate code!");
    exit(EXIT_FAILURE);
  }

  i = 0;
  while (fgets(buf, MAX_STRING_SIZE, fp)) {
    buflen = strlen(buf);

    if (!buflen) {
      continue;
    }

    if ((strcmp(buf, "\n") == 0) || (strcmp(buf, "\r\n") == 0)) {
      continue;
    }

    parse_instruction(&code[i], buf, &kernel->sem_table);
    i++;
  }

  return code;
}
