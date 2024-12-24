#ifndef LLVM_COMPILER_DECLARATIONS
#define LLVM_COMPILER_DECLARATIONS
#include "../../header/expr.hpp"
#include "../../header/stmt.hpp"
#include "../../header/decl.hpp"
#include "../../header/module.hpp"
#include "./utils.cpp"
#include "./scopes.cpp"
#include "./sym.cpp"
Val					compile_expr_field(FieldExpr* f, PContext* ctx);
Val					compile_expr_init(InitExpr* init_expr, PContext* ctx);
Val					compile_expr_int(IntExpr* i, PContext* ctx);
Val					compile_stmt(Stmt* s, PContext* ctx);
Val					compile_scope(ScopeStmt* scope, PContext* ctx);
Val					compile_expr(Expr* e, PContext* ctx);
bool				is_ptype_ptr(PType* ptype);
Val					val_type(PType* ptype, llvm::Type* type);
Val					val_error(const char* fmt, ...);
Val					compile_name_typespec(NameTypeSpec* nts, PContext* ctx);
Val					compile_typespec(TypeSpec* ts, PContext* ctx);
Val					compile_typespec(TypeSpec* ts, PContext* ctx);
std::vector<Val>	compile_proc_params(ProcParams* params, PContext* ctx);
Val					compile_proc_proto(ProcDecl* proc, PContext* ctx);
int					compile_decl_proc(ProcDecl* proc, PContext* ctx);
int					compile_decl(Decl* decl, PContext* ctx);
int					compile_decl_aggregate(AggregateDecl* agg, PContext* ctx);
Val					compile_aggregate_decl_to_type(AggregateDecl* agg, PContext* ctx);
Val					compile_type(PType* ptype, PContext* ctx);
Val					derref_ptype(PType* ptype, PContext* ctx);
bool				is_ptype_ptr(PType* ptype);
Val					compile_expr_binary(BinaryExpr* b, PContext* ctx);
Val					compile_expr_binary_eq(BinaryExpr* b, PContext* ctx);
Val					compile_expr_call(CallExpr* c, PContext* ctx);
Val					compile_expr_string(StringExpr* str, PContext* ctx);
void				compile_module(Module* mod, PContext* ctx, bool enterScope);
Val					compile_stmt_return(ReturnStmt* ret, PContext* ctx);
Val					propagate_aggregate_methods(Val& val, AggregateDecl* agg, PContext* ctx);
#endif/*LLVM_COMPILER_DECLARATIONS*/