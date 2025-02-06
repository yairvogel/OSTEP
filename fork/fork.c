
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int _argc, char* _argv[]) {
  printf("%d: Hello from main!\n", getpid());

  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "Error calling fork\n");
    exit(1);
  } else if (pid == 0) {
    printf("%d: hello from child\n", getpid());
  } else {
    printf("%d: child process id is %d\n", getpid(), pid);
  }
}
