#ifndef OS_SIMULATOR_LIST_H
#define OS_SIMULATOR_LIST_H

typedef struct ListNode {
  void *content;
  struct ListNode *next;
  struct ListNode *prev;
} list_node_t;

typedef struct {
  list_node_t *head;
  list_node_t *tail;
  int size;
} list_t;

list_node_t *list_node_create(void *);
list_node_t *list_remove_head(list_t *);
void list_remove_node(list_t *, list_node_t *);
list_node_t *list_remove_tail(list_t *);

list_t *list_init(void);
void list_add(list_t *, void *, int);
int is_list_empty(list_t *);

#endif // OS_SIMULATOR_LIST_H
