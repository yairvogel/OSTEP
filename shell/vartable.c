
#include "vartable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct kvpair_t kvpair_t;
typedef struct vartable vartable_t;

#define BUFLEN 1024

struct kvpair_t {
  char* key;
  char* value;
};

struct vartable {
  ushort cap;
  ushort len;
  kvpair_t* arr;
};


void* vartable_create(int cap) {
  kvpair_t* arr = malloc(sizeof(kvpair_t) * cap);
  vartable_t* t = malloc(sizeof(vartable_t));
  t->cap = cap;
  t->len = 0;
  t->arr = arr;
  return t;
}

void vartable_destroy(void* table)
{
  vartable_t* vartable = table;
  free(vartable->arr);
  free(vartable);
}

int   vartable_set(void* table, char* const var, char* const val) {
  vartable_t* vartable = table;
  if (vartable->len >= vartable->cap) {
    return 1;
  }

  kvpair_t* arr = vartable->arr;
  for (uint i = 0; i < vartable->len; i++) {
    kvpair_t* kvp = &arr[i];
    if (kvp->key == NULL) {
    } else if (strncmp(kvp->key, var, BUFLEN) == 0) {
      free(kvp->value);
      kvp->value = malloc(sizeof(char) * (strnlen(val, BUFLEN) + 1));
      strcpy(kvp->value, val);
      return 0;
    }
  }

  kvpair_t* kvp = &arr[vartable->len];
  kvp->key = malloc(sizeof(char) * (strnlen(var, BUFLEN) + 1));
  strcpy(kvp->key, var);

  kvp->value = malloc(sizeof(char) * (strnlen(val, BUFLEN) + 1));
  strcpy(kvp->value, val);

  vartable->len++;
  return 0;
}

char* vartable_get(void* table, char* const var) {
  vartable_t* vartable = table;
  for (int i = 0; i < vartable->len; i++) {
    kvpair_t kvp = vartable->arr[i];

    if (strncmp(kvp.key, var, BUFLEN) == 0) {
      return kvp.value;
    }
  }

  return NULL;
}

void vartable_dump(void* table) {
  vartable_t* vartable = table;
  for (uint i = 0; i < vartable->len; i++) {
    kvpair_t kvp = vartable->arr[i];
    printf("%s=%s\n", kvp.key, kvp.value);
  }
}
