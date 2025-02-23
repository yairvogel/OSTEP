#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// depends on <stdio.h>, so kept in a separate block to avoid include sorting
#include <readline/history.h>
#include <readline/readline.h>

#include "syntax.h"
#include "vartable.h"

#define UNUSED(x) (void)(x)

#define BUFLEN 1024

int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  char *buf = NULL;
  int lastStatus = 0;
  void *vartable = vartable_create(100);
  using_history();
  while (1) {
    const char *prompt;
    if (lastStatus) {
      prompt = "! ";
    } else {
      prompt = "> ";
    }

    if (buf != NULL) {
      free(buf);
      buf = NULL;
    }

    char *buf = readline(prompt);
    if (buf == NULL) {
      return 0;
    }

    char var[BUFLEN];
    char val[BUFLEN];
    int l = sscanf(buf, "%s = %s", var, val);
    if (l == 2) {
      add_history(buf);
      vartable_set(vartable, var, val);
      continue;
    }

    char *tokens[512];
    char *tok = strtok(buf, " ");
    int len;
    for (len = 0; len < 511; len++) {
      if (tok == NULL)
        break;

      // parameter expansion
      if (tok[0] == '$') {
        char *val = vartable_get(vartable, tok + 1);
        if (val == NULL) {
          *tok = '\0';
        } else {
          tok = val;
        }
      }

      tokens[len] = tok;
      tok = strtok(NULL, " ");
    }

    tokens[len] = NULL;
    if (len == 0) {
      lastStatus = 0;
      continue;
    }

    if (len == 1 && (strcmp(tokens[0], "exit") == 0)) {
      exit(0);
    }

    if (len == 1 && (strcmp(tokens[0], "dump") == 0)) {
      vartable_dump(vartable);
      continue;
    }

    char *outfile = NULL;
    if (len > 2 && strcmp(tokens[len - 2], ">") == 0) {
      outfile = tokens[len - 1];
      tokens[len - 2] = NULL;
    }

    add_history(buf);

    syntaxtree_t *syntax = stparse(tokens);
    int *cids = stexec(syntax, outfile);

    int status = 0;
    for (int *cid = cids; *cid != -1; cid++) {
      int p_st;
      if (waitpid(*cid, &p_st, 0) == -1) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
      }

      if (WIFEXITED(p_st)) {
        status |= WEXITSTATUS(p_st);
      }
    }

    lastStatus = status;
  }

  vartable_destroy(vartable);
  clear_history();
  return 0;
}
