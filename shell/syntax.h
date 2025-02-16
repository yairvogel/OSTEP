
#ifndef SYNTAX_H
#define SYNTAX_H

typedef char** statement_t;

typedef enum {
  STATEMENT,
  PIPE
} nodetype_t;

typedef struct syntaxtree_t {
  nodetype_t nodetype;
  statement_t statement;
  struct syntaxtree_t* left;
  struct syntaxtree_t* right;
} syntaxtree_t;

syntaxtree_t* stparse(char* tokens[]);
int* stexec(syntaxtree_t*, char*);

#endif
