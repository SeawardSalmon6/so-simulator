#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../helpers/constants.h"
#include "semaphores.h"

void semaphore_init(semaphore_t *sem, char *name, const int status) {
  sem->name = duplicate_str(name);
  sem->status = status;
  sem->waiters = list_init();
  sem_init(&sem->mutex, 0, 1);
}

void semaphore_register(semaphore_table_t *sem_table, char *sem_name) {
  semaphore_t *sem, *table;
  int i;

  for (i = 0; i < sem_table->length; i++) {
    if (strcmp(sem_table->table[i].name, sem_name) == 0) {
      return;
    }
  }

  table = (semaphore_t *)realloc(sem_table->table, sizeof(semaphore_t) * (sem_table->length + 1));

  if (!table) {
    printf("\n--> Error: Not enough memory to allocate semaphore table!");
    exit(EXIT_FAILURE);
  }

  sem_table->table = table;
  sem_table->length = sem_table->length + 1;
  sem = &sem_table->table[sem_table->length - 1];

  semaphore_init(sem, sem_name, 1);
}

void semaphore_table_init(semaphore_table_t *sem_table) {
  semaphore_t *table = (semaphore_t *)malloc(0);

  if (!table) {
    printf("\n--> Error: Not enough memory to allocate semaphore table!");
    exit(EXIT_FAILURE);
  }

  sem_table->table = table;
  sem_table->length = 0;
}

semaphore_t *semaphore_find(semaphore_table_t *sem_table, char *sem_name) {
  semaphore_t *tmp = &sem_table->table[0];

  while (tmp < &sem_table->table[sem_table->length]) {
    if (strcmp(sem_name, tmp->name) == 0) {
      return tmp;
    }
  }

  return NULL;
}
