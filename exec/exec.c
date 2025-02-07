#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define UNUSED(x) (void)(x)

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);
  int pid = getpid();
  printf("%d: start\n", pid);
  
  int cid = fork();
  if (cid < 0) {
    fprintf(stderr, "%d: failed to fork\n", pid);
  }
  else if (cid == 0) {
    // child
    sleep(1);
    char* args[3];
    args[0] = "wc";
    args[1] = "exec.c";
    args[2] = NULL;
    execvp("wc", args);
    exit(1);
  }
  else {
    printf("%d: started %d\n", pid, cid);
    wait(NULL);
    printf("%d: ended %d\n", pid, cid);
  }
}
