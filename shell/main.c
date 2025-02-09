#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define UNUSED(x) (void)(x)

#define BUFLEN 1024

typedef struct {
  char* key;
  char* value;
} KeyValuePair;

KeyValuePair variables[100] = { 0 };

int assignVariable(char* var, char* val, KeyValuePair variables[100]);
char* getVariable(char* var, KeyValuePair variables[100]);

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
      assignVariable(var, val, variables);
      continue;
    }
    
    char* tokens[512];
    char* tok = strtok(buf, " \n");
    int len;
    for (len = 0; len < 511; len++) {
      if (tok == NULL) break;
      if (tok[0] == '$') {
        char* val = getVariable(tok+1, variables);
        if (val == NULL) {
          *tok = '\0';
        } else {
          tok = val;
        }
      }

      *(tokens + len) = tok;
      tok = strtok(NULL, " \n");
    }

    *(tokens + len + 1) = NULL;
    
    if (len == 0) {
      lastStatus = 0;
      continue;
    }

    // tokens holds NULL terminated array of char* 
    int cid = fork();
    if (cid < 0) {
      fprintf(stderr, "failed forking executable\n");
    }

    else if (cid == 0) {
      errno = 0;
      execvp(*tokens, tokens);
      perror("failed execvp");
      exit(EXIT_FAILURE);
    } else {
      int status;
      if (waitpid(cid, &status, 0) == -1) {
        perror("waitpid falied");
        exit(EXIT_FAILURE);
      }
      
      if (WIFEXITED(status)) {
        lastStatus = WEXITSTATUS(status);
      }
    }
  }

  return 0;
}

int assignVariable(char* var, char* val, KeyValuePair variables[100]) {
  for (int i = 0; i < 100; i++) {
    KeyValuePair* kvp = variables + i;
    if (kvp->key == NULL) {
      kvp->key = malloc(sizeof(char) * strnlen(var, BUFLEN) + 1);
      strcpy(kvp->key, var);

      kvp->value = malloc(sizeof(char) * strnlen(val, BUFLEN) + 1);
      strcpy(kvp->value, val);
      return 0;
    } else if (strncmp(kvp->key, var, BUFLEN) == 0) {
      free(kvp->value);
      kvp->value = malloc(sizeof(char) * strnlen(val, BUFLEN) + 1);
      strcpy(kvp->value, val);
      return 0;
    }
  }

  return 1;
}

char* getVariable(char* var, KeyValuePair variables[100]) {
  for (int i = 0; i < 100; i++) {
    KeyValuePair* kvp = variables + i;
    if (kvp->key == NULL) {
      return NULL;
    }

    if (strncmp(kvp->key, var, BUFLEN) == 0) {
      return kvp->value;
    }
  }

  return NULL;
}
