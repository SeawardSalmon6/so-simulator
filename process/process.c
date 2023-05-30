#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../helpers/constants.h"
#include "../helpers/list.h"
#include "../kernel/kernel.h"
#include "../memory/memory.h"
#include "../terminal/terminal.h"

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
  code = read_code(fp, buf, process);

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

instruction_t *read_code(FILE *fp, char *buf, process_t *process) {
  int i, buflen;
  long int code_section;
  instruction_t *code;

  code_section = ftell(fp);
  process->code_length = 0;

  while (fgets(buf, MAX_STRING_SIZE, fp)) {
    process->code_length++;
  }

  fseek(fp, code_section, SEEK_SET);

  code = (instruction_t *)malloc(process->code_length * sizeof(instruction_t));

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

    parse_instruction(&code[i], buf, &process->remaining, &kernel->sem_table);
    i++;
  }

  return code;
}

list_node_t *process_list_node_search(list_t *list, int proc_id) {
  list_node_t *current = list->head;
  process_t *content;

  while (current) {
    content = (process_t *)content;

    if (content->id == proc_id) {
      return current;
    }

    current = current->next;
  }

  return NULL;
}

void process_finish(process_t *proc) {
  if (proc) {
    list_node_t *bcp_proc_node = process_list_node_search(kernel->proc_table, proc->id);
    list_remove_node(kernel->proc_table, bcp_proc_node);

    if (kernel->scheduler.scheduled_process->id == proc->id) {
      sys_call(PROCESS_INTERRUPT, (void *)NONE);
    }

    segment_free(&kernel->seg_table, proc->segment_id);

    list_node_t *scheduled_proc_node;

    if ((scheduled_proc_node = process_list_node_search(kernel->scheduler.high_queue->queue, proc->id))) {
      list_remove_node(kernel->scheduler.high_queue->queue, scheduled_proc_node);
    } else if ((scheduled_proc_node = process_list_node_search(kernel->scheduler.low_queue->queue, proc->id))) {
      list_remove_node(kernel->scheduler.low_queue->queue, scheduled_proc_node);
    } else if ((scheduled_proc_node = process_list_node_search(kernel->scheduler.blocked_queue->queue, proc->id))) {
      list_remove_node(kernel->scheduler.blocked_queue->queue, scheduled_proc_node);
    }

    if (is_process_log_active) {
      printf("\nProcess (%s - %d): Stopped executing!", proc->name, proc->id);
    }

    free(proc->name);
    free(proc->semaphores);
    free(proc);
  }
}

void segment_free(segment_table_t *seg_table, int seg_id) {
  list_node_t *seg_node = seg_table->segment_list->head;
  segment_t *seg;

  while (seg_node) {
    if (((segment_t *)seg_node->content)->id == seg_id) {
      break;
    }
    seg_node = seg_node->next;
  }

  if (!seg_node) {
    printf("\n--> Error: An unregistered segment was request to be freed!");
    exit(EXIT_FAILURE);
  }

  list_remove_node(seg_table->segment_list, seg_node);

  seg = (segment_t *)seg_node->content;

  seg_table->remaining = MAX_MEM_SIZE <= seg_table->remaining + seg->size ? MAX_MEM_SIZE : seg_table->remaining + seg->size;

  free(seg->page_table);
  free(seg);
  free(seg_node);
}
