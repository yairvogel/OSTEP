
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  char *addr = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if (addr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < 1 << 14; i++) {
    if (i % 0x30 == 0)
      printf("%d\n", i);

    addr[i] = 'c';
  }
}
