#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  int* p = malloc(sizeof(int));
  assert(p != NULL);
  *p = 0;
  __pid_t pid = getpid();
  printf("%d pointer value is %p\n", pid, p);
  
  while (1)
  {
    sleep(1);
    *p = *p  + 1;
    printf("%d value is %d\n", getpid(), *p);
  }
}
