#ifndef LLVM_EXPR_COMPILAR
#define LLVM_EXPR_COMPILAR
#include "./declarations.cpp"
#include "./typeCompiler.cpp"
#include <IR/Value.h>

using Ctx = PContext;
Val compile_expr_float(FloatExpr* f, Ctx* ctx){	
	llvm::Type* float_type = ctx->builder->getFloatTy();	
	llvm::APFloat apfloat(f->getFloat());	
	return val_float(llvm::ConstantFP::get(ctx->ctx, apfloat));

}
Val compile_expr_int(IntExpr* i, Ctx* ctx) {
	llvm::Type* int_type = ctx->builder->getInt64Ty();
	llvm::Value* val = llvm::ConstantInt::get(int_type, llvm::APInt(64, i->getI64()));
	return val_int(val);
}
//Val compile_expr_float(IntExpr* i, PContext* ctx){}
Val compile_expr_name(NameExpr* ne, Ctx* ctx) {
	Val* val = ctx->get_ptr(ne->getName());	
	if (!val) return val_error(ctx->errs.add_error("The name %s is not defined in this scope", ne->getName()));

	Val ret = *val;
			
	if (llvm::Function* fn = llvm::dyn_cast<llvm::Function>(val->value)) {
		return val_int(fn);		
	}
		
	if (ctx->expect_load_addr() and !val->type->isPointerTy()) {		
		ret.value = ctx->builder->CreateLoad(val->type, val->value);
		//ret.value = ctx->builder->CreateSExtOrBitCast(value, val->type);
	}
	else {
		ret.type = val->type->getPointerTo();
		ret.ptype = new PtrType(val->ptype);
		ret.value = val->value;		
	}
	
	ret.error = nullptr;
	return ret;
}
Val compile_expr_init(InitExpr* init, Ctx* ctx) {
	if (!init->typespec) {
		return val_error(ctx->errs.add_error("For now we can't initialize a variable without it's type."));
	}

	Val type = compile_typespec(init->typespec, ctx);

	if (type.error) return val_error("error: %s", type.error);	
	assert(type.type);
	assert(type.ptype);

	llvm::Value* allocated_value = ctx->builder->CreateAlloca(type.type, 0, init->name);


	if (init->init) {
		Val initializer = compile_expr(init->init, ctx);
		if (initializer.error) {
			return val_error(ctx->errs.add_error("Failed to compile %s initialier due to: %s", init->name, initializer.error));
		}
		llvm::Value* init_val = initializer.value;
		assert(init_val);
		ctx->builder->CreateStore(init_val, allocated_value);
	}
	Val val = type;
	val.ptype = type.ptype;
	val.value = allocated_value;
	val.is_lvalue = false;
	val.is_rvalue = true;
	val.type = allocated_value->getType();
	ctx->define(init->name, val);
	return val;
}
Val compile_expr_binary_eq(BinaryExpr* b, PContext* ctx){
	ctx->set_load_addr(false);
	Val lhs = compile_expr(b->lhs, ctx);
	if (lhs.error) return val_error("\n%s", lhs.error);

	ctx->set_load_addr(true);
	Val rhs = compile_expr(b->rhs, ctx);
	if (rhs.error) return val_error("\n%s", rhs.error);

	// ptr value
	
	lhs.value = ctx->builder->CreateStore(rhs.value, lhs.value);

	return lhs;
}
Val compile_expr_binary_add(BinaryExpr* b, PContext* ctx) {	
	ctx->set_load_addr(true);
	Val lhs = compile_expr(b->lhs, ctx);
	if (lhs.error) return val_error("%s", lhs.error);

	ctx->set_load_addr(true);
	Val rhs = compile_expr(b->rhs, ctx);
	if (rhs.error) return val_error("%s", rhs.error);

	if (lhs.type != rhs.type) {
		Val err = promote_type(lhs, rhs, ctx);
		//if (err.error) return val_error("%s", err.error);
	}
	//if (lhs.type != rhs.type) {
	//	return val_error("TODO: promote types.\n");
	//}
	

	if (lhs.type->isIntegerTy() and lhs.type == rhs.type) {
		lhs.value = ctx->builder->CreateAdd(rhs.value, lhs.value);
	}

	else {		
		lhs.value->print(llvm::errs());
		printf("\nto:\n");
		rhs.value->print(llvm::errs());
		printf("\n");
		system("pause");
		exit(1);
	}
	return lhs;
}

Val compile_expr_binary(BinaryExpr* b, PContext* ctx){
	if (b->token.isKind(TK_eq)) return compile_expr_binary_eq(b, ctx);
	else if (b->token.isKind(TK_plus)) return compile_expr_binary_add(b, ctx);
	return val_error("Binary expressions are not implemented yet.");
}
static int step_number = 0; // Global step counter

void print_step(const char* step_msg) {
	printf("STEP %d: %s\n", ++step_number, step_msg);
}
Val check_for_struct_methods(AggregateType* cls, const char* field, PContext* ctx) {
	std::string expanded_method = std::string(cls->name) + "_" + std::string(field);
	const char* expanded_method_name = uniqueString.makeIntern(expanded_method.c_str());
	if (Val* method = ctx->get_ptr(expanded_method_name)) {
		return *method;
	}			
	return val_error("Field not found");
}
Val auto_load_struct_ptr(PType* ty, Val* cls_val, PContext* ctx) {
	// **struct
	if (PtrType* ptr = dynamic_cast<PtrType*>(ty)) {
		PType* base = ptr->base;
		if (dynamic_cast<PtrType*>(base)) {
			return val_error("struct double pointer can't auto-load it's address.\n");
		}

		if (AggregateType* agg = dynamic_cast<AggregateType*>(base)) {			
			SymClass* cls = dynamic_cast<SymClass*>(ctx->get_ptr_cls_sym(agg->name));
			assert(cls);
			llvm::Type* cls_type = cls->val.type;

			//cls_val->value = ctx->builder->CreateLoad(cls_type, cls_val->value);
			//cls_val->type = cls_val->value->getType();
			cls_val->ptype = agg;
			printf("AUTO DERREF STRUCT.\n");
			return *cls_val;
		}

		else {
			return val_error("Unexpected error while trying to auto-load struct address.\n");
		}
	}
	return val_type(dynamic_cast<AggregateType*>(ty), nullptr);
}

Val compile_expr_field(FieldExpr* f, PContext* ctx){
	bool load_after = ctx->expect_load_addr();
	ctx->set_load_addr(false);
	Val base = compile_expr(f->base, ctx);
	if (base.error) return val_error("%s", base.error);	
	ctx->set_current_self(base);
	PtrType* ptr = dynamic_cast<PtrType*>(base.ptype);
	AggregateType* agg_t = nullptr;
	
	if (f->auto_derref and dynamic_cast<PtrType*>(ptr->base))
	{
		if (dynamic_cast<AggregateType*>(ptr->base)) {
			printf("[ERROR]: expected pointer to auto-derreference struct with field '%s'", f->field);
		}

		PtrType* basePtr = dynamic_cast<PtrType*>(ptr->base);
		if (!basePtr) {
			return val_error("[ERROR]: failed to auto-derreference with field '%s'\n", f->field);
		}
		Val struct_val = auto_load_struct_ptr(basePtr, &base, ctx);							
		agg_t = dynamic_cast<AggregateType*>(struct_val.ptype);
		
	}
	else {
		agg_t = dynamic_cast<AggregateType*>(ptr->base);
	}
		
	if (!agg_t)		return val_error("Could not find struct in field access [CODE PTYPE].\n");	
	SymClass* cls	= dynamic_cast<SymClass*>(ctx->get_ptr_cls_sym(agg_t->name));
	assert(cls);

	int field_index = cls->get_field_index(f->field);
	if (field_index < 0) {
		Val method = check_for_struct_methods(agg_t, f->field, ctx);
		if (method.error) {
			return val_error("Field '%s' not found in struct %s.\n", f->field, agg_t->name);
		}		
		return method;
		
	}
		
	Sym* field = cls->m_fields[field_index];
	Val field_val = field->val;	
	field_val.value = ctx->builder->CreateStructGEP(cls->val.type, base.value, field_index);
	ctx->set_load_addr(load_after);
	if(ctx->expect_load_addr()){				
		field_val.value = ctx->builder->CreateLoad(field_val.type, field_val.value);				
	}
	else {
		// get za ptr		
		field_val.ptype = new PtrType(field_val.ptype);
		field_val.type = field_val.type->getPointerTo();
	}	
	
	return field_val;
}
std::vector<llvm::Value*> fix_args_for_struct_method_call(llvm::Function* func, std::vector<llvm::Value*> old_args, PContext* ctx) {	
	return old_args;

	const char* func_name = uniqueString.makeIntern(func->getName().str().c_str());
	if (Val* fn_val = ctx->get_ptr(func_name)) {
		ProcType* proc = dynamic_cast<ProcType*>(fn_val->ptype);		
	
		if (proc->params.size() == 0) return old_args;
		PtrType* as_ptr  = dynamic_cast<PtrType*>(proc->params.at(0));
		if (!as_ptr) return old_args;

		AggregateType* agg = dynamic_cast<AggregateType*>(as_ptr->base);
		if (!agg) return old_args;
				
		// TODO: check if func is method	
		
		std::vector<llvm::Value*> args_with_self = { ctx->get_current_self().value };
		for (llvm::Value* arg : old_args) args_with_self.push_back(arg);
		return args_with_self;
	}

	return old_args;
}
Val compile_expr_call(CallExpr* c, PContext* ctx) {
	Val base = compile_expr(c->base, ctx);
	if (base.error) return val_error("%s", base.error);
	assert(base.value);
	std::vector<llvm::Value*> args;
	for (Expr* arg : c->args) {
		Val arg_val = compile_expr(arg, ctx);
		if (arg_val.error) return val_error("%s", arg_val.error);

		assert(arg_val.value);
		args.push_back(arg_val.value);
	}

	llvm::Function* func = llvm::dyn_cast<llvm::Function> (base.value);
	if (!func) {		
		return val_error("The function is undefined while trying to call.\n");		
	}
	
	args = fix_args_for_struct_method_call(func, args, ctx);
	
		
	llvm::FunctionCallee callee = llvm::FunctionCallee(func);	
	base.value = ctx->builder->CreateCall(callee, args);
	return base;
}
Val compile_expr_string(StringExpr* str, PContext* ctx) {
	return val_str(ctx->builder->CreateGlobalStringPtr(str->getString()));
}
Val compile_expr_unary(UnaryExpr* u, PContext* ctx) {
	switch (u->token.kind) {
	default: {
		return val_error("Unary type indefined for compiling.\n");
	}

	case TK_and: {
		ctx->set_load_addr(false);
		return compile_expr(u->expr, ctx);
	}
	case TK_mul: { // * expr -> derreference
		Val base = compile_expr(u->expr, ctx);
		if (base.error)
			return val_error("%s", base.error);

		if (!dynamic_cast<PtrType*>(base.ptype)) {
			return val_error("Trying to derrefernce a non pointer type.\n");
		}
		
		base.value = ctx->builder->CreateLoad(base.type, base.value);
		return base;
	}
	case TK_minus: { // - expr
		Val base = compile_expr(u->expr, ctx);
		if (base.error)
			return val_error("%s", base.error);
		if (base.type->isIntegerTy()) {
			base.value = ctx->builder->CreateNeg(base.value);
		}
		else {
			base.value = ctx->builder->CreateFNeg(base.value);
		}
		return base;
	}
	}
}

Val compile_expr(Expr* e, PContext* ctx){	
	if (IntExpr*	int_expr	= dcast<IntExpr>(e))	return compile_expr_int(int_expr, ctx);
	if (FloatExpr*	f_expr		= dcast<FloatExpr>(e))	return compile_expr_float(f_expr, ctx);
	if (StringExpr*	str_expr	= dcast<StringExpr>(e))	return compile_expr_string(str_expr, ctx);
	if (NameExpr*	name_expr	= dcast<NameExpr>(e))	return compile_expr_name(name_expr, ctx);
	if (InitExpr*	init_expr	= dcast<InitExpr>(e))	return compile_expr_init(init_expr, ctx);
	if (FieldExpr*	field_expr	= dcast<FieldExpr>(e))	return compile_expr_field(field_expr, ctx);
	if (BinaryExpr*	binary_expr	= dcast<BinaryExpr>(e))	return compile_expr_binary(binary_expr, ctx);
	if (CallExpr*	call_expr	= dcast<CallExpr>(e))	return compile_expr_call(call_expr, ctx);
	if (UnaryExpr*	unary_expr	= dcast<UnaryExpr>(e))	return compile_expr_unary(unary_expr, ctx);
	
	else {
		print_expr(e, 5);
		system("pause");
		ctx->errs.add_error("Can't compile this expression yet.");
		return val_error("Can't compile this expression yet.");
	}
}
#endif/*LLVM_EXPR_COMPILAR*/