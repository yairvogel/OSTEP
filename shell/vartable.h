
#ifndef VARTABLE
#define VARTABLE

void* vartable_create(int cap);
void  vartable_destroy(void* table);
int   vartable_set(void* table, char* const var, char* const val);
char* vartable_get(void* table, char* const var);

void  vartable_dump(void* table);

#endif
