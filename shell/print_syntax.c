#include "syntax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_syntax(syntaxtree_t*, int);

void print_statement(statement_t st, int depth) {
  for (int i = 0; i < depth; i++) {
    write(1, " ", 1);
  }

  printf("statement:");
  for (int i = 0; st[i] != NULL; i++) {
    printf(" %s", st[i]);
  }

  printf("\n");
}

void print_pipe(syntaxtree_t* syntax, int depth) {
  print_syntax(syntax->left, depth+4);

  for (int i = 0; i < depth; i++) {
    write(1, " ", 2);
  }
  printf("||\n");

  print_syntax(syntax->right, depth+4);
}

void print_syntax(syntaxtree_t* syntax, int depth) {
  switch (syntax->nodetype) {
    case STATEMENT: print_statement(syntax->statement, depth); break;
    case PIPE: print_pipe(syntax, depth);
  }
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  char buf[1024];
  while(1) {
    char* read = fgets(buf, 1023, stdin);
    if (read == NULL) exit(EXIT_FAILURE);
    char* tokens[512];
    char* tok = strtok(buf, " \n");
    int len;
    for (len = 0; len < 511; len++) {
      if (tok == NULL) break;
      tokens[len] = tok;
      tok = strtok(NULL, " \n");
    }
    tokens[len] = NULL;

    syntaxtree_t* syntax = stparse(tokens);
    print_syntax(syntax, 0);
  }
}

