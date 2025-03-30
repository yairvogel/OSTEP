
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACES 2
#define MAXLEN 1000

/* turns space sequences of length n to a tab */
void entab(char str[MAXLEN], int n) {
  int r = 0, w = 0;
  int spaces_seen = 0;
  while (1) {
    char c = str[r++];
    if (c == ' ') {
      if (++spaces_seen == n) {
        str[w++] = '\t';
        spaces_seen = 0;
      }
    } else {
      for (int i = 0; i < spaces_seen; i++) {
        str[w++] = ' ';
      }
      spaces_seen = 0;

      if (c == '\0') {
        str[w++] = '\0';
        break;
      } else {
        str[w++] = c;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int spaces = SPACES;
  if (argc == 2) {
    spaces = atoi(argv[1]);
  }

  char buf[MAXLEN];
  while (1) {
    char *line = fgets(buf, MAXLEN, stdin);
    if (line == NULL) {
      break;
    }

    entab(line, spaces);
    printf("%s", line);
  }
}
