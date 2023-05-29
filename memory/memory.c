#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../helpers/list.h"
#include "../process/instruction.h"
#include "../process/process.h"
#include "memory.h"

void segment_table_init(segment_table_t *seg_table) {
  seg_table->segment_list = list_init();
  seg_table->segment_list_size = 0;
  seg_table->remaining = 1 * GIGABYTE;
}

void memory_request_init(memory_request_t *mem_req, process_t *process, instruction_t *code) {
  mem_req->process = process;
  mem_req->code = code;
}

void memory_request_load(memory_request_t *request, segment_table_t *seg_table) {
  segment_t *segment = segment_create(request);

  const int new_remaining = seg_table->remaining - segment->size;
  seg_table->remaining = 0 >= new_remaining ? 0 : new_remaining;

  if (new_remaining < 0) {
    seg_table->remaining += memory_page_swap(seg_table, segment);
  }

  segment_write_code(segment, request->code, request->process->code_length);
  segment_add(seg_table, segment);
}

int memory_page_swap(segment_table_t *seg_table, segment_t *seg) {
  int i;
  int freed = 0;
  list_node_t *curr_node;
  segment_t *seg_content;
  page_t *page;

  curr_node = seg_table->segment_list->head;

  while (curr_node) {
    seg_content = !curr_node ? NULL : (segment_t *)curr_node->content;

    for (i = 0; i < seg_content->page_count; i++) {
      page = seg_content->page_table + i;

      if (page->used) {
        page->used = 0;
      } else {
        freed += PAGE_SIZE;
      }

      if (freed >= seg->size) {
        break;
      }
    }

    curr_node = curr_node->next;
  }

  return freed;
}

segment_t *segment_create(memory_request_t *request) {
  segment_t *segment = (segment_t *)malloc(sizeof(segment_t));

  if (!segment) {
    printf("\n--> Error: Not enough memory to allocate a segment!");
    exit(EXIT_FAILURE);
  }

  segment->id = request->process->segment_id;
  segment->size = request->process->segment_size * KILOBYTE;
  segment->page_count = (int)ceil((double)request->process->code_length / INSTRUCTIONS_PER_PAGE);
  segment->page_table = (page_t *)malloc(sizeof(page_t) * segment->page_count);

  if (!segment->page_table) {
    printf("\n--> Error: Not enough memory to allocate a segment page table!");
    exit(EXIT_FAILURE);
  }

  segment->page_qtd = segment->size / PAGE_SIZE;

  return segment;
}

void segment_write_code(segment_t *seg, instruction_t *code, const int code_length) {
  int i;
  for (i = 0; i < code_length; i++) {
    seg->page_table[PAGE_NUMBER(i)].used = 0;
  }

  for (i = 0; i < code_length; i++) {
    seg->page_table[PAGE_NUMBER(i)].code[PAGE_OFFSET(i)] = code[i];
  }
}

void segment_add(segment_table_t *seg_table, segment_t *segment) {
  list_add(seg_table->segment_list, (void *)segment);
  seg_table->segment_list_size++;
}
