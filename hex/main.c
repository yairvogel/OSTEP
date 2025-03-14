
#include <stdio.h>
#include <stdlib.h>


void p(unsigned long n) { printf("%lx, %lb\n", n, n); }

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	int *ints = (int*)malloc(sizeof(int) * 1024);
	int *ptr = &ints[512 + 256];
	p(ptr - ints);

	long *longs = (long*)malloc(sizeof(long) * 1024);
	long *lptr = &longs[512 + 256];
	p(lptr - longs);
}
