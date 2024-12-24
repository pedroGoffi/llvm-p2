#ifndef LLVM_STMT_COMPILER
#define LLVM_STMT_COMPILER
#include "./declarations.cpp"
#include "./exprCompiler.cpp"

Val compile_stmt_return(ReturnStmt* ret, PContext* ctx){
	llvm::Function* func = ctx->get_current_function();
	if (!func) return val_error("Current function is not set in compile_stmt_return.\n");
	Val val;
	if(ret->expr){
		val = compile_expr(ret->expr, ctx);
		if (val.error) return val_error("%s", val.error);
		val.value = ctx->builder->CreateRet(val.value);
		
	}
	else {
		if (!func->getReturnType()->isVoidTy()) 
			return val_error("The current function %s expects non void return type.\n", func->getName().str().c_str());

		val.value = ctx->builder->CreateRetVoid();		
		val.error = nullptr;
	}
	ctx->defined_return = true;
	return val;
}
Val compile_stmt(Stmt* s, PContext* ctx){
	assert(s);
	ctx->context_variables_rewind();
	if (auto expr = dcast<ExprStmt>(s))		return compile_expr(expr->expr, ctx);
	if (auto ret = dcast<ReturnStmt>(s))	return compile_stmt_return(ret, ctx);
	else {
		print_stmt(s, 5);
		return val_error("Can't compile this statement yet.");
	}
}
Val compile_scope(ScopeStmt* scope, PContext* ctx) {
	ctx->enter_scope();
	Val val = val_int(ctx->builder->getInt32(0));
	for (Stmt* node : scope->nodes) {
		val = compile_stmt(node, ctx);
		if (val.error) {
			return val_error(ctx->errs.add_error("Unexpected error: %s", val.error));
		}
	}
	printf("------------\n");
	ctx->leave_scope();
	return val;
}
#endif/*LLVM_STMT_COMPILER*/