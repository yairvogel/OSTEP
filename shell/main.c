#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define UNUSED(x) (void)(x)

#define BUFLEN 1024

typedef struct {
  char* key;
  char* value;
} KeyValuePair;

KeyValuePair variables[100] = { 0 };

int assignVariable(char* var, char* val, KeyValuePair variables[100]);
char* getVariable(char* const var, KeyValuePair variables[100]);
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
      assignVariable(var, val, variables);
      continue;
    }
    
    char* tokens[512];
    char* tok = strtok(buf, " \n");
    int len;
    for (len = 0; len < 511; len++) {
      if (tok == NULL) break;

      // parameter expansion
      if (tok[0] == '$') {
        char* val = getVariable(tok+1, variables);
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

    int cid = execcmd(tokens, outfile);
    int status;
    if (waitpid(cid, &status, 0) == -1) {
      perror("waitpid falied");
      exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
      lastStatus = WEXITSTATUS(status);
    }
  }

  return 0;
}

mode_t newFileMode =  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

int execcmd(char* tokens[], char* outfile) {
  errno = 0;
  int cid = fork();
  if (cid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }
  else if (cid == 0) {
    if (outfile != NULL) {
      errno = 0;
      int fd = open(outfile, O_WRONLY | O_CREAT, newFileMode);
      if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
      }
      dup2(fd, 1);
      close(fd);
    }

    execvp(*tokens, tokens);
    perror("failed execvp");
    exit(EXIT_FAILURE);
  } else {
    return cid;
  }
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
    if (kvp->key == NULL)
      return NULL;

    if (strncmp(kvp->key, var, BUFLEN) == 0)
      return kvp->value;
  }

  return NULL;
}
