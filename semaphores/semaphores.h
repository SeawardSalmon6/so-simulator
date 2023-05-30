#ifndef OS_SIMULATOR_SEMAPHORES_H
#define OS_SIMULATOR_SEMAPHORES_H

#include <semaphore.h>

#include "../helpers/list.h"
#include "../process/process.h"

typedef struct {
  char *name;
  int status;
  list_t *waiters;
  sem_t mutex;
} semaphore_t;

typedef struct {
  semaphore_t *table;
  int length;
} semaphore_table_t;

void semaphore_init(semaphore_t *, char *, int);
void semaphore_register(semaphore_table_t *, char *);
void semaphore_table_init(semaphore_table_t *);
semaphore_t *semaphore_find(semaphore_table_t *, char *);
void semaphore_P(semaphore_t *, process_t *, void (*)(void));
void semaphore_V(semaphore_t *, void (*wakeup_proc)(process_t *));

#endif // OS_SIMULATOR_SEMAPHORES_H
