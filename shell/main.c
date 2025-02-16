#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "syntax.h"

#define UNUSED(x) (void)(x)

#define BUFLEN 1024

typedef struct {
  char* key;
  char* value;
} kvpair_t;

kvpair_t variables[100] = { 0 };

int setvar(char* var, char* val, kvpair_t variables[100]);
char* getvar(char* const var, kvpair_t variables[100]);
__pid_t execcmd(char* tokens[], char* outfile);

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  char buf[BUFLEN];
  int lastStatus = 0;
  while (1) {
    if (lastStatus) {
      write(1, "! ", 2);
    } else {
      write(1, "> ", 2);
    }

    char* read = fgets(buf, BUFLEN-1, stdin);
    if (read == NULL) {
      return 0;
    }

    char var[BUFLEN];
    char val[BUFLEN];
    int l = sscanf(buf, "%s = %s\n", var, val);
    if (l == 2) {
      setvar(var, val, variables);
      continue;
    }
    
    char* tokens[512];
    char* tok = strtok(buf, " \n");
    int len;
    for (len = 0; len < 511; len++) {
      if (tok == NULL) break;

      // parameter expansion
      if (tok[0] == '$') {
        char* val = getvar(tok+1, variables);
        if (val == NULL) {
          *tok = '\0';
        } else {
          tok = val;
        }
      }

      tokens[len] = tok;
      tok = strtok(NULL, " \n");
    }

    tokens[len] = NULL;
    if (len == 0) {
      lastStatus = 0;
      continue;
    }

    if (len == 1 && (strcmp(tokens[0], "exit") == 0)) {
      exit(0);
    }
    
    char* outfile = NULL;
    if (len > 2 && strcmp(tokens[len-2], ">") == 0) {
      outfile = tokens[len-1];
      tokens[len-2] = NULL;
    }

    syntaxtree_t* syntax = stparse(tokens);
    int* cids = stexec(syntax, outfile);

    int status;
    for (int* cid = cids; *cid != -1; cid++) {
      int p_st;
      if (waitpid(*cid, &p_st, 0) == -1) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
      }

      if (WIFEXITED(p_st)) {
        status |= WEXITSTATUS(p_st);
      }
    }

    lastStatus = WEXITSTATUS(status);
  }

  return 0;
}

int setvar(char* var, char* val, kvpair_t variables[100]) {
  for (int i = 0; i < 100; i++) {
    kvpair_t* kvp = variables + i;
    if (kvp->key == NULL) {
      kvp->key = malloc(sizeof(char) * (strnlen(var, BUFLEN) + 1));
      strcpy(kvp->key, var);

      kvp->value = malloc(sizeof(char) * (strnlen(val, BUFLEN) + 1));
      strcpy(kvp->value, val);
      return 0;
    } else if (strncmp(kvp->key, var, BUFLEN) == 0) {
      free(kvp->value);
      kvp->value = malloc(sizeof(char) * (strnlen(val, BUFLEN) + 1));
      strcpy(kvp->value, val);
      return 0;
    }
  }

  return 1;
}

char* getvar(char* var, kvpair_t variables[100]) {
  for (int i = 0; i < 100; i++) {
    kvpair_t* kvp = variables + i;
    if (kvp->key == NULL)
      return NULL;

    if (strncmp(kvp->key, var, BUFLEN) == 0)
      return kvp->value;
  }

  return NULL;
}
