#ifndef LLVM_DECL_COMPILE
#define LLVM_DECL_COMPILE
#include "./declarations.cpp"
#include "./stmtCompiler.cpp"

std::vector<Val> compile_proc_params(ProcParams* params, PContext* ctx){	
	std::vector<Val> params_val;
	for(FieldTypeSpec* field: params->params){
		Val param_val;		
		Val type = compile_typespec(field->typespec, ctx);		
		if (type.error) {
			ctx->errs.add_error(type.error);
			return {};
		}
		assert(type.type);
		// TODO: resolve typespec param_val.ptype = field->typespec;
		param_val = type; // copy the type trace in Val type
		// allocate the parameter and set param_val.value to the stack allocated variable				
		params_val.push_back(param_val);		
	}
	return params_val;
}

Val compile_proc_proto(ProcDecl* proc, PContext* ctx) {	 	
	std::vector<Val> params = compile_proc_params(proc->params, ctx); // scope += 1
	if (params.size() != proc->params->params.size()) {
		return val_error("[ERROR]: failed to compile function %s\n", proc->name);		
	}
	std::vector<llvm::Type*> params_type;
	std::vector<PType*> params_ptype;
	for (Val& val : params) {
		if (val.error) {			
			return val_error("got error: %s.", val.error);
		}
		if (!val.type) {
			return val_error("val.type == nullptr.");			
		}
		params_type.push_back(val.type);
		params_ptype.push_back(val.ptype);
	}

	Val ret = compile_typespec(proc->ret, ctx);
	if (ret.error) return val_error("%s", ret.error);
	
	llvm::FunctionType* func_type = llvm::FunctionType::get(ret.type, params_type, proc->params->isVarArg);	
	llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, proc->name, ctx->mod);
	if (!func) {
		return val_error("Function func == nullptr.");		
	}
	
	
	Val func_val = {
		func,
		func->getReturnType(),
		new ProcType(params_ptype, ret.ptype, proc->params->isVarArg, proc->params->varArgPos),
		false,
		false,
		nullptr
	};	
	
	

	func_val.error = nullptr;
	return func_val;
}

void compile_proc_argument_stack_allocation(ProcParams* params, PContext* ctx) {	
	llvm::Function* func = ctx->get_current_function();	
	if (!func) {
		ctx->errs.add_error("Can't compile the stack allocation for parameters.");
		return;
	}
	llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx->ctx, "entry", func);
	ctx->builder->SetInsertPoint(entry);

	unsigned arg_pos = 0;
	for (llvm::Argument& arg : func->args()) {		
		FieldTypeSpec*	param = params->params.at(arg_pos);		
		arg.setName(param->name);
		arg_pos++;

		Val val = compile_typespec(param->typespec, ctx);
		if (val.error) {
			ctx->errs.add_error("Can't initialize arguments %s.", param->name);
			return;
		}
		
		if(val.type->isPointerTy()){
			val.value = &arg;
		}
		else {
			val.value = ctx->builder->CreateAlloca(arg.getType(), 0, param->name);
			ctx->builder->CreateStore(&arg, val.value);			
		}
		ctx->define(param->name, val);
		
		
	}
}
void compile_default_return_value(llvm::Function* func, Val func_val, PContext* ctx){
	if (!ctx->defined_return) {
		ProcType* proc = dynamic_cast<ProcType*>(func_val.ptype);
		assert(proc);
		PType* ret_val = proc->ret;
		if (dynamic_cast<VoidType*>(ret_val)) {
			ctx->builder->CreateRetVoid();
		}

		else {			
			print_type(func_val.ptype);
			ctx->builder->CreateRet(llvm::Constant::getNullValue(func->getReturnType()));
		}
		
	}
}
int compile_decl_proc(ProcDecl* proc, PContext* ctx){	
	Val func_val = compile_proc_proto(proc, ctx);	

	if (func_val.error) {
		printf("Failed to compile procedure prototype %s.\n", proc->name);
		assert(func_val.error);
		printf("ERROR: %s\n", func_val.error);
		return EXIT_FAILURE;
	}
	ctx->define(proc->name, func_val);
	llvm::Function* func = llvm::dyn_cast<llvm::Function>(func_val.value);
	assert(func);	
	if (!func) {
		ctx->errs.add_error("Failed to compile function %s.", proc->name);
		return EXIT_FAILURE;
	}
	
	if (proc->isExtern) {			
		return EXIT_SUCCESS;
	}
	
	ctx->set_current_function(func);
	ctx->enter_scope();	
	compile_proc_argument_stack_allocation(proc->params, ctx);	
	compile_scope(proc->scope, ctx);							
	compile_default_return_value(func, func_val, ctx);	
	ctx->leave_scope();
	ctx->drop_current_function();
	llvm::verifyFunction(*llvm::dyn_cast<llvm::Function>(func_val.value));
	return EXIT_SUCCESS;
}
std::vector<FieldTypeSpec*> proc_params_with_self(std::vector<FieldTypeSpec*>& old_params, AggregateDecl* agg, PContext* ctx){
	SymClass* cls = dynamic_cast<SymClass*>(ctx->get_ptr_cls_sym(agg->name));
	assert(cls);

	PtrTypeSpec* ptr_to_cls = new PtrTypeSpec({}, new NameTypeSpec({}, agg->name));
	FieldTypeSpec* self_ts = new FieldTypeSpec({}, "self", ptr_to_cls, nullptr);

	std::vector<FieldTypeSpec*> params = { self_ts };
	for (FieldTypeSpec* param : old_params) params.push_back(param);
	return params;
}
Val propagate_aggregate_methods(Val& val, AggregateDecl* agg, PContext* ctx) {	
	llvm::StructType* struct_type = llvm::dyn_cast<llvm::StructType>(val.type);
	assert(struct_type);

		
	for (Decl* decl : agg->methods) {
		// STRUCT_method
		{
			// STRUCT_method
			std::string method_name = (std::string(agg->name) + "_" + std::string(decl->name));			
			decl->name = uniqueString.makeIntern(method_name.c_str());
		}
		{
			// first arguement being self
			if (ProcDecl* proc = dynamic_cast<ProcDecl*>(decl)) {
				proc->params->params = proc_params_with_self(proc->params->params, agg, ctx);				
			}
		}
		if (compile_decl(decl, ctx) < 0) {
			assert(decl->name);
			return val_error("Failed to mount methods for struct %s.%s\n", agg->name, decl->name);
		}
	}
	
	return val;
}

int compile_decl_aggregate(AggregateDecl* agg, PContext* ctx){
	printf("[WARN]: the struct feature is not fully implemented yet, proceed with caution.\n");
	Val val = compile_aggregate_decl_to_type(agg, ctx);
	if (val.error) {
		printf("[ERROR]: %s\n", val.error);
		return EXIT_FAILURE;
	}
	

	llvm::StructType* strukt = llvm::dyn_cast<llvm::StructType>(val.type);
	if (!strukt) {
		printf("Failed to mount struct %s\n", agg->name);
		return EXIT_FAILURE;	
	}
	assert(agg);
	
	ctx->define_cls(
		agg->name,
		val,
		strukt,
		ctx);

	
	Val self_val = val;
	self_val.ptype = new PtrType(self_val.ptype);
	self_val.type = self_val.type->getPointerTo();
		
	ctx->set_current_self(self_val);
	propagate_aggregate_methods(val, agg, ctx);
	ctx->set_current_self(val_null());
	return EXIT_SUCCESS;
}
int compile_decl_use(UseModuleDecl* use, PContext* ctx) {	
	if (!ctx->modMan.add_path(use->path)) return EXIT_SUCCESS;
	Module new_module = Module(use->path.string().c_str());
	if (new_module.error) {
		printf("[ERROR]: %s\n", new_module.error);
		system("pause");
		return EXIT_FAILURE;
	}
		
	compile_module(&new_module, ctx, false);
	
	return EXIT_SUCCESS;
}
int compile_decl(Decl* decl, PContext* ctx) {	
	if (ProcDecl*		proc	= dcast<ProcDecl>(decl))		return compile_decl_proc(proc, ctx);
	if (AggregateDecl*	agg		= dcast<AggregateDecl>(decl))	return compile_decl_aggregate(agg, ctx);
	if (UseModuleDecl*	use		= dcast<UseModuleDecl>(decl))	return compile_decl_use(use, ctx);

	else {
		ctx->errs.add_error("Can't compile declaration %s.", decl->name);
		return EXIT_FAILURE;
	}
}

void compile_module(Module* mod, PContext* ctx, bool enterScope = true){	
	assert(mod and ctx);	
	int last_scope_size;
	if(enterScope) last_scope_size = ctx->enter_scope(); //  scope global
		
	for (Decl* decl : mod->getAst()) {
		ctx->context_variables_rewind();				

		if (compile_decl(decl, ctx) != EXIT_SUCCESS) 
			break;
	}
	
	if (enterScope) {
		int current_scope_size = ctx->leave_scope();
		if (0 and last_scope_size != current_scope_size) {
			ctx->errs.add_error("[DEV-ERROR]: the current scope size doesn't match the last scope size (%i: %i).\n", last_scope_size, current_scope_size);
		}
	}


	
}
#endif/*LLVM_DECL_COMPILE*/