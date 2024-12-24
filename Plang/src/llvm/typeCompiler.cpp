#ifndef LLVM_TYPE_COMPILER
#define LLVM_TYPE_COMPILER
#include "./declarations.cpp"
#include "./utils.cpp"
// will return ptype and llvm::Type into Val
Val val_type_int(llvm::IntegerType* it, PType* ptype) {
	return val_type(ptype, it);

}
Val compile_name_typespec(NameTypeSpec* nts, PContext* ctx){
	if (nts->name == i64_keyword)	return val_type(new IntType(nts->name, 64), ctx->builder->getInt64Ty());
	if (nts->name == i32_keyword)	return val_type(new IntType(nts->name, 32), ctx->builder->getInt32Ty());
	if (nts->name == i16_keyword)	return val_type(new IntType(nts->name, 16), ctx->builder->getInt16Ty());
	if (nts->name == i8_keyword)	return val_type(new IntType(nts->name, 8),	ctx->builder->getInt8Ty());
	if (nts->name == char_keyword)	return val_type(new IntType(nts->name, 8), ctx->builder->getInt8Ty());
	if (nts->name == void_keyword)	return val_type(new VoidType(), ctx->builder->getVoidTy());

	if (nts->name == f32_keyword)	return val_type(new FloatType("f32", 32), ctx->builder->getFloatTy());
	if (nts->name == f64_keyword)	return val_type(new FloatType("f64", 64), ctx->builder->getDoubleTy());

	// check for Self
	if(nts->name == Self_keyword){
		Val self = ctx->get_current_self();				
		if (!self.type or !self.ptype) return val_error("Self is not defined.\n");
		return self;
		
	}
	if (Sym* cls_sym = ctx->get_ptr_cls_sym(nts->name)) {
		SymClass* cls = dynamic_cast<SymClass*>(cls_sym);;
		if (!cls) {
			return val_error("Couldn't load the class %s", nts->name);
		}					
		return cls->val;
	}
	return val_error("The type %s is not defined in this scope.", nts->name);
}

Val compile_typespec(TypeSpec* ts, PContext* ctx){
	

	if (auto named_typespec = dcast<NameTypeSpec>(ts)) return compile_name_typespec(named_typespec, ctx);
	if (auto ptr_typespec = dcast<PtrTypeSpec>(ts)) { 
		Val pointee = compile_typespec(ptr_typespec->base, ctx);

		if (pointee.error) {
			printf("ok\n");
			return val_error(ctx->errs.add_error("type error: %s", pointee.error));
		}
		// pointer is equal pointee
		Val pointer = pointee;
		// but we only change:
		
		pointer.type = pointee.type->getPointerTo();
		pointer.ptype = new PtrType(pointer.ptype);
		pointer.error = nullptr;
		//pointer.ptype->name = uniqueString.makeIntern(pointee.ptype->name);
		return pointer;
		
	}
	else {
		return val_error("Unexpected typespec in compile_typespec.\n");
	}
}
Val compile_aggregate_decl_to_type(AggregateDecl* agg, PContext* ctx) {
	AggregateType* agg_type = new AggregateType(agg->name, {});
	std::vector<llvm::Type*> llvm_fields = std::vector<llvm::Type*>{};	

	for (FieldTypeSpec* field : agg->fields) {
		Val ty = compile_typespec(field->typespec, ctx);
		if (ty.error) return val_error(ty.error);
		assert(ty.ptype and ty.type);

		ty.ptype = new FieldType(field->name, ty.ptype);

		
		agg_type->fields.push_back(ty.ptype);
		llvm_fields.push_back(ty.type);			
	}
	llvm::StructType* struct_type = llvm::StructType::create(ctx->ctx, agg->name);
	
	struct_type->setBody(llvm_fields, false);
	struct_type->setName(agg->name);
	
	return val_type(agg_type, struct_type);
}
bool is_ptype_ptr(PType* ptype) {
	return dynamic_cast<PtrType*>(ptype) != nullptr;
}

Val compile_type(PType* ptype, PContext* ctx) {		
	if (!ptype) return val_error("ptype == nullptr.");

	if (VoidType* var = dynamic_cast<VoidType*>(ptype)) { 
		return val_type(var, ctx->builder->getVoidTy());
	}

	if (IntType* var = dynamic_cast<IntType*>(ptype)) {
		return val_type(var, ctx->builder->getInt64Ty());
	}

	if (FloatType* var = dynamic_cast<FloatType*>(ptype)) {
		return val_type(var, ctx->builder->getDoubleTy());
	}

	if (AggregateType* var = dynamic_cast<AggregateType*>(ptype)) {
		SymClass* cls = dynamic_cast<SymClass*>(ctx->get_ptr_cls_sym(var->name));
		if (!cls) {
			printf("ERROR GET CLASS.\n");
			return val_error("Bad logic.\n");
		}

		printf("IS AGGREGATE! %s!!!!!!\n", cls->name);
		return val_type(var, cls->val.type);
	}

	if (PtrType* var = dynamic_cast<PtrType*>(ptype)) {
		Val base = compile_type(var->base, ctx);
		if (base.error) return val_error("%s", base.error);
		assert(base.type);

		return val_type(var, base.type->getPointerTo());

	}

	if (FieldType* var = dynamic_cast<FieldType*>(ptype)) { 
		printf("GOT FIELD:\n");
		system("pause");
		exit(1);
	}
	
	printf("tyname %s\n", ptype->name);
	
	return val_error("undefined in deffef ptype");
	
}
Val derref_ptype(PType* ptype, PContext* ctx) {
	if (!is_ptype_ptr(ptype)) {
		return val_error("can't derref a non pointer.\n");
	}
	PtrType* base = dynamic_cast<PtrType*>(ptype);
	assert(base);
	return compile_type(base, ctx);
}
Val promote_to_integer(Val& lhs, Val& rhs, PContext* ctx) {
	int lhs_size = lhs.type->getIntegerBitWidth();
	int rhs_size = lhs.type->getIntegerBitWidth();
	// casts promoter -> promoteTo
	Val* promoteTo = &lhs;
	Val* promoter = &rhs;
	if (lhs_size > rhs_size) {
		// rhs -> lhs
		promoter = &rhs;
		promoteTo = &lhs;
	}
	else {
		// lhs -> rhs
		promoter = &lhs;
		promoteTo = &rhs;
	}

		
	//promoter->value = ctx->builder->CreateBitCast(promoter->value, promoteTo->type);
	return *promoter;
}
Val promote_type(Val& lhs, Val& rhs, PContext* ctx){
	if (lhs.type == rhs.type) return lhs;

	if (lhs.type->isIntegerTy() and rhs.type->isIntegerTy()) {
		return promote_to_integer(lhs, rhs, ctx);
	}

	printf("@@@@@@@@ CAST ERROR [");
	lhs.type->print(llvm::outs());
	printf(", ");
	rhs.type->print(llvm::outs());
	printf("]\n");

	return val_error("Can't promote the lhs type with the rhs.\n");
}
#endif/*LLVM_TYPE_COMPILER*/