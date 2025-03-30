// write a program `detab` which replaces tabs in the input with the proper
// number of blanks to space to the next tab stop. Assume a fixed set of tab
// stops, say every `n` positions

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLEN 1000

int readline(char buf[MAXLEN]);
int detab(char dst[MAXLEN], char src[MAXLEN], int spaces);

// stream char by char, using only
int main(int argc, char *argv[]) {
  int spaces = 2;
  if (argc == 2) {
    spaces = atoi(argv[1]);
  }

  char src[MAXLEN];
  char dst[MAXLEN];
  int l;
  while (readline(src) != 0) {
    detab(dst, src, spaces);
    printf("%s", dst);
  }

  return 0;
}

int readline(char buf[MAXLEN]) {
  char c;
  int i;
  for (i = 0; i < MAXLEN - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
    buf[i] = c;

  if (c == '\n') {
    buf[i] = c;
    ++i;
  }

  buf[i] = '\0';

  return i;
}

int detab(char dst[MAXLEN], char src[MAXLEN], int spaces) {
  int j = 0;
  for (int i = 0; i < MAXLEN; i++) {
    char c = src[i];
    if (c == '\0')
      break;
    if (c == '\t') {
      for (int k = 0; k < spaces; k++)
        dst[j++] = ' ';
    } else {
      dst[j++] = c;
    }
  }

  dst[j] = '\0';
  return j;
}
