#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "syntax.h"

typedef int cid;
typedef int fd;

syntaxtree_t* new_statement(statement_t);
syntaxtree_t* new_pipe(syntaxtree_t* left, syntaxtree_t* right);
int find (char* const tokens[], char* const term);

syntaxtree_t* stparse(char* tokens[]) {
  int pipeidx = find(tokens, "|");
  if (pipeidx != -1) {
    tokens[pipeidx] = NULL;
    syntaxtree_t* left = new_statement(tokens);
    syntaxtree_t* right = stparse(tokens + pipeidx + 1);
    syntaxtree_t* pipe = new_pipe(left, right);
    pipe->right = stparse(tokens + pipeidx + 1);
    return pipe;
  }

  return new_statement(tokens);
}

mode_t newFileMode =  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

int execcmd(char *const tokens[], fd input, fd output) {
  errno = 0;
  int cid = fork();
  if (cid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }
  else if (cid == 0) {
    if (output >= 0) {
      dup2(output, 1);
      close(output);
    }
    if (input >= 0) {
      dup2(input, 0);
      close(input);
    }

    execvp(*tokens, tokens);
    perror("failed execvp");
    exit(EXIT_FAILURE);
  } else {
    return cid;
  }
}

void stexec_inner(syntaxtree_t* syntax, cid pids[], fd readfd, fd writefd) {
  int pipefd[2];
  switch (syntax->nodetype) {
    case STATEMENT:
      pids[0] = execcmd(syntax->statement, readfd, writefd);
      break;
    case PIPE:
      errno = 0;
      if (pipe(pipefd) < 0) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
      }

      stexec_inner(syntax->left, pids, readfd, pipefd[1]);
      stexec_inner(syntax->right, ++pids, pipefd[0], writefd);
      break;
  }
}

int* stexec(syntaxtree_t* syntax, char* outfile) {
  int* pids = malloc(sizeof(int) * 10);
  memset(pids, -1, sizeof(int) * 10);

  int fd = -1;
  if (outfile != NULL) {
    errno = 0;
    fd = open(outfile, O_WRONLY | O_CREAT, newFileMode);
    if (fd < 0) {
      perror("open failed");
      exit(EXIT_FAILURE);
    }
  }

  stexec_inner(syntax, pids, -1, fd);
  return pids;
}

syntaxtree_t* new_statement(statement_t statement) {
  syntaxtree_t* st = malloc(sizeof(syntaxtree_t));
  st->nodetype = STATEMENT;
  st->statement = statement;
  return st;
}

syntaxtree_t* new_pipe(syntaxtree_t* left, syntaxtree_t* right) {
  syntaxtree_t* st = malloc(sizeof(syntaxtree_t));
  st->nodetype = PIPE;
  st->left = left;
  st->right = right;
  return st;
}

int find(char* const tokens[], char* const term) {
  for (int i = 0; tokens[i] != NULL ; i++) {
    if (strcmp(tokens[i], term) == 0) {
      return i;
    }
  }

  return -1;
}
