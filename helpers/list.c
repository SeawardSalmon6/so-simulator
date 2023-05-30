#include <stdio.h>
#include <stdlib.h>

#include "list.h"

list_node_t *list_node_create(void *content) {
  list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));

  if (!node) {
    printf("--> Error: Not enough memory to allocate a list node!\n");
    exit(EXIT_FAILURE);
  }

  node->content = content;
  node->next = NULL;
  node->prev = NULL;

  return node;
}

list_node_t *list_remove_head(list_t *list) {
  list_node_t *node = list->head;

  if (list->head) {
    if (list->head->next) {
      list->head->next->prev = NULL;
    } else {
      list->tail = NULL;
    }

    list->head = list->head->next;

    // Free node from list
    node->prev = NULL;
    node->next = NULL;
    list->size--;
  }

  return node;
}

void list_remove_node(list_t *list, list_node_t *node) {
  if (node) {
    if (node->prev) {
      node->prev->next = node->next;
    } else if (node == list->head) {
      list->head = node->next;
    }

    if (node->next) {
      node->next->prev = node->prev;
    } else if (node == list->tail) {
      list->tail = node->prev;
    }

    list->size--;
  }
}

list_node_t *list_remove_tail(list_t *list) {
  list_node_t *node = list->tail;

  if (list->tail) {
    if (list->tail->prev) {
      list->tail->prev->next = NULL;
    } else {
      list->head = NULL;
    }

    list->tail = list->tail->prev;

    // Free node from list
    node->prev = NULL;
    node->next = NULL;
    list->size--;
  }

  return node;
}

list_t *list_init(void) {
  list_t *list = (list_t *)malloc(sizeof(list_t));

  if (!list) {
    printf("--> Error: Not enough memory to allocate a list!\n");
    exit(EXIT_FAILURE);
  }

  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void list_add(list_t *list, void *content) {
  list_node_t *node = list_node_create(content);

  if (!list->head) {
    list->head = node;
  } else {
    list->tail->next = node;
    node->prev = list->tail;
  }

  list->tail = node;
  list->size++;
}

int is_list_empty(list_t *list) {
  return !list->head;
}

void list_free(list_t *list) {
  if (list) {
    list_node_t *cur = list->head;
    list_node_t *tmp;

    while (cur) {
      tmp = cur->next;
      free(cur);
      cur = tmp;
    }

    free(list);
  }
}
