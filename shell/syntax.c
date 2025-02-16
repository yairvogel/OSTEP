#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "syntax.h"

int find (char* tokens[], const char* term) {
  for (int i = 0; tokens[i] != NULL ; i++) {
    if (strcmp(tokens[i], term) == 0) {
      return i;
    }
  }

  return -1;
}

syntaxtree_t* statement(statement_t statement) {
  syntaxtree_t* st = malloc(sizeof(syntaxtree_t));
  st->nodetype = STATEMENT;
  st->statement = statement;
  return st;
}

syntaxtree_t* pipe_st(syntaxtree_t* arg1) {
  syntaxtree_t* st = malloc(sizeof(syntaxtree_t));
  st->nodetype = PIPE;
  st->left = arg1;
  return st;
}

syntaxtree_t* stparse(char* tokens[]) {
  int pipeidx = find(tokens, "|");
  if (pipeidx != -1) {
    tokens[pipeidx] = NULL;
    syntaxtree_t* left = statement(tokens);
    syntaxtree_t* pipe = pipe_st(left);
    pipe->right = stparse(tokens + pipeidx + 1);
    return pipe;
  }

  return statement(tokens);
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


int* stexec(syntaxtree_t* syntax, char* outfile) {
  // assume syntax is non null, with node of type statement
  statement_t statement = syntax->statement;
  int* cids = malloc(2*sizeof(int));
  cids[0] = execcmd(statement, outfile);
  cids[1] = -1;
  return cids;
}
