#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

char *duplicate_str(char *str) {
  if (!str) {
    return NULL;
  }

  const size_t length = strlen(str);
  char *tmp = (char *)malloc((length + 1) * sizeof(char));
  if (!tmp) {
    return NULL;
  }

  memcpy(tmp, str, length);
  tmp[length] = 0;
  return tmp;
}
