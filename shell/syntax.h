
#ifndef SYNTAX_H
#define SYNTAX_H

typedef char** statement_t;
typedef void syntaxtree_t;

syntaxtree_t* stparse(char* tokens[]);
int* stexec(syntaxtree_t*, char*);

#endif
