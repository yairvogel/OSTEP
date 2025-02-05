
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int counter = 0;

void* worker(void *data) {
  int l = *(int*)data;
  for (int i = 0; i < l; i++) {
    counter++;
  }
  return NULL;
}
  
int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: threads <value>\n");
    exit(1);
  }

  int loops = atoi(argv[1]);
  pthread_t t1, t2;
  pthread_create(&t1, NULL, worker, &loops);
  pthread_create(&t2, NULL, worker, &loops);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  printf("counter: %d\n", counter);
}
