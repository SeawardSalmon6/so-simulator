#ifndef OS_SIMULATOR_MEMORY_H
#define OS_SIMULATOR_MEMORY_H

#include "../process/instruction.h"
#include "../process/process.h"

#define KILOBYTE (1024)
#define MEGABYTE (1024 * KILOBYTE)
#define GIGABYTE (1024 * MEGABYTE)

#define MAX_MEM_SIZE (1 * GIGABYTE)

#define INSTRUCTIONS_PER_PAGE (512)
#define PAGE_SIZE (8 * KILOBYTE)
#define PAGE_NUMBER(x) ((x) / INSTRUCTIONS_PER_PAGE)
#define PAGE_OFFSET(x) ((x) % INSTRUCTIONS_PER_PAGE)

typedef struct Page {
  instruction_t code[INSTRUCTIONS_PER_PAGE];
  int used;
} page_t;

typedef struct {
  int id;
  int size;
  page_t *page_table;
  int page_count;
  int page_qtd;
} segment_t;

typedef struct {
  list_t *segment_list;
  int segment_list_size;
  int remaining;
} segment_table_t;

typedef struct {
  process_t *process;
  instruction_t *code;
} memory_request_t;

void memory_request_init(memory_request_t *, process_t *, instruction_t *);
void memory_request_load(memory_request_t *, segment_table_t *);
int memory_page_swap(segment_table_t *, segment_t *);
void segment_table_init(segment_table_t *);
segment_t *segment_create(memory_request_t *);
void segment_write_code(segment_t *, instruction_t *, const int);
void segment_add(segment_table_t *, segment_t *);
segment_t *segment_find(segment_table_t *, int);
void segment_free(segment_table_t *, int);

#endif // OS_SIMULATOR_MEMORY_H
