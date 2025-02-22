
#ifndef SYNTAX_H
#define SYNTAX_H

typedef char** statement_t;

typedef struct syntaxtree_t syntaxtree_t;
typedef enum nodetype_t nodetype_t;

enum nodetype_t {
  STATEMENT,
  PIPE
};

struct syntaxtree_t {
  nodetype_t nodetype;
  statement_t statement;
  struct syntaxtree_t* left;
  struct syntaxtree_t* right;
};

syntaxtree_t* stparse(char* tokens[]);
int* stexec(syntaxtree_t*, char*);

#endif
