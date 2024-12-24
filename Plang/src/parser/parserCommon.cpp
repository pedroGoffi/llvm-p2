#ifndef PARSERCOMMON
#define PARSERCOMMON
#include "../../header/expr.hpp"
#include "../../header/stmt.hpp"
#include "../../header/decl.hpp"
#include "../../header/srcLocation.hpp"

Expr* parse_new(Lexer* lexer, SrcLocation loc);
Expr* parse_init(Lexer* lexer, SrcLocation loc, const char* name);
Expr* parse_operand(Lexer* lexer);
Expr* parse_base(Lexer* lexer);
Expr* parse_unary(Lexer* lexer);
Expr* parse_mul(Lexer* lexer);
Expr* parse_add(Lexer* lexer);
Expr* parse_cmp(Lexer* lexer);
Expr* parse_and(Lexer* lexer);
Expr* parse_or(Lexer* lexer);
Expr* parse_ternary(Lexer* lexer);
Expr* parse_paren(Lexer* lexer);
Expr* parse_expr(Lexer* lexer);
std::vector<Expr*> parse_call_args(Lexer* lexer);


Stmt* parse_stmt(Lexer* lexer);
Stmt* parse_while(Lexer* lexer);
Stmt* parse_for(Lexer* lexer);
Stmt* parse_simple(Lexer* lexer);
ComptimeStmt* parse_comptime(Lexer* lexer);
ScopeStmt* parse_scope(Lexer* lexer);


Decl* parse_decl(Lexer* lexer);
ProcParams* parse_fn_params(Lexer* lexer);
FieldTypeSpec* parse_fn_param(Lexer* lexer);
Decl* parse_aggregate(Lexer* lexer);
std::pair<std::vector<FieldTypeSpec*>, std::vector<Decl*>> parse_aggregate_fields(Lexer* lexer);

TypeSpec* parse_operandspec(Lexer* lexer);
TypeSpec* parse_basespec(Lexer* lexer);
TypeSpec* parse_namespec(Lexer* lexer);
TypeSpec* parse_ptrspec(Lexer* lexer);
TypeSpec* parse_arrayspec(Lexer* lexer);
std::vector<TypeSpec*> parse_typespec_list(Lexer* lexer, TokenKind tokenKindDelim);
//TypeSpec* parse_procspec(Lexer* lexer);
TypeSpec* parse_constspec(Lexer* lexer);
TypeSpec* parse_typespec(Lexer* lexer);
TypeSpec* parse_templatespec(SrcLocation opLoc, TypeSpec* operand, Lexer* lexer);
Decl* parse_fn(Lexer* lexer);
#endif /*PARSERCOMMON*/