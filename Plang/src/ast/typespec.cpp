#ifndef TYPESPEC_AST
#define TYPESPEC_AST
#include "../../header/typespec.hpp"

void TypeSpec::setRef(bool isRef) {
	this->isRef = isRef;
}
void TypeSpec::setMut(bool isMut) {
	this->isMut = isMut;
}

NameTypeSpec::NameTypeSpec(SrcLocation loc, const char* name) {
	this->loc = loc;
	this->name = uniqueString.makeIntern(name);
}

PtrTypeSpec::PtrTypeSpec(SrcLocation loc, TypeSpec* base) {
	this->loc = loc;
	this->base = base;
}
ArrayTypeSpec::ArrayTypeSpec(SrcLocation loc, Expr* size, TypeSpec* base) {
	this->loc = loc;
	this->size = size;
	this->base = base;
}
ProcTypeSpec::ProcTypeSpec(SrcLocation loc, const std::vector<TypeSpec*> args, TypeSpec* ret, bool isVararg = false, const size_t varargPos = 0) {
	this->loc = loc;
	this->args = args;
	this->ret = ret;
	this->isVararg = isVararg;
	this->varargPos = varargPos;
}

ConstTypeSpec::ConstTypeSpec(SrcLocation loc, TypeSpec* base, bool isRef) {
	this->loc = loc;
	this->base = base;
	this->isRef = isRef;
}
TemplateTypeSpec::TemplateTypeSpec(SrcLocation loc, TypeSpec* base, std::vector<TypeSpec*> args) {
	this->loc = loc;
	this->base = base;
	this->args = args;
}
FieldTypeSpec::FieldTypeSpec(SrcLocation loc, const char* name, TypeSpec* typespec, Expr* defaultInit) {
	this->loc = loc;
	this->name = uniqueString.makeIntern(name);
	this->typespec = typespec;
	this->defaultInit = defaultInit;
}
#endif /*TYPESPEC_AST*/