#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "syntax.h"

typedef int cid;
typedef int fd;

enum nodetype_t { STATEMENT, PIPE };

struct syntaxnode_t {
  enum nodetype_t nodetype;
};

struct statementnode_t {
  struct syntaxnode_t self;
  statement_t statement;
};

struct pipenode_t {
  struct syntaxnode_t self;
  struct syntaxnode_t *left;
  struct syntaxnode_t *right;
};

struct statementnode_t *new_statement(statement_t);
struct pipenode_t *new_pipe(syntaxtree_t *left, syntaxtree_t *right);
int find(char *const tokens[], char *const term);

syntaxtree_t *stparse(char *tokens[]) {
  int pipeidx = find(tokens, "|");
  if (pipeidx != -1) {
    tokens[pipeidx] = NULL;
    struct syntaxnode_t *left = (struct syntaxnode_t *)new_statement(tokens);
    struct syntaxnode_t *right =
        (struct syntaxnode_t *)stparse(tokens + pipeidx + 1);
    struct pipenode_t *pipe = new_pipe(left, right);
    pipe->right = stparse(tokens + pipeidx + 1);
    return (syntaxtree_t *)pipe;
  }

  return (syntaxtree_t *)new_statement(tokens);
}

mode_t newFileMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

int execcmd(char *const tokens[], fd input, fd output) {
  errno = 0;
  int cid = fork();
  if (cid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (cid == 0) {
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

void stexec_inner(syntaxtree_t *syntax, cid pids[], fd readfd, fd writefd) {
  struct syntaxnode_t *node = syntax;
  int pipefd[2];
  switch (node->nodetype) {
  case STATEMENT:
    pids[0] =
        execcmd(((struct statementnode_t *)syntax)->statement, readfd, writefd);
    break;
  case PIPE:
    errno = 0;
    if (pipe(pipefd) < 0) {
      perror("pipe failed");
      exit(EXIT_FAILURE);
    }

    stexec_inner(((struct pipenode_t *)syntax)->left, pids, readfd, pipefd[1]);
    stexec_inner(((struct pipenode_t *)syntax)->right, ++pids, pipefd[0],
                 writefd);
    break;
  }
}

int *stexec(syntaxtree_t *syntax, char *outfile) {
  int *pids = malloc(sizeof(int) * 10);
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

struct statementnode_t *new_statement(statement_t statement) {
  struct statementnode_t *st = malloc(sizeof(struct statementnode_t));
  st->self.nodetype = STATEMENT;
  st->statement = statement;
  return st;
}

struct pipenode_t *new_pipe(syntaxtree_t *left, syntaxtree_t *right) {
  struct pipenode_t *st = malloc(sizeof(struct pipenode_t));
  st->self.nodetype = PIPE;
  st->left = left;
  st->right = right;
  return st;
}

int find(char *const tokens[], char *const term) {
  for (int i = 0; tokens[i] != NULL; i++) {
    if (strcmp(tokens[i], term) == 0) {
      return i;
    }
  }

  return -1;
}
