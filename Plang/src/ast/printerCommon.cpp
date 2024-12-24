#include <cstdio>
#ifndef PRINTER_COMMON
#define PRINTER_COMMON

#define IDENT_FORCE 4
void print_expr(Expr* e, int i);
void print_typespec(TypeSpec* ts, int i);
void print_stmt(Stmt* stmt, int i);

static void fident(FILE* file, int ident){
	for (int i = 0; i < ident * IDENT_FORCE; i++) fprintf(file, " ");
}
static void ident(int ident) {
	fident(stdout, ident);
}

#endif /*PRINTER_COMMON*/