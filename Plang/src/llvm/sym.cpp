#ifndef SYM_LLVM_COMPILE
#define SYM_LLVM_COMPILE
#include <map>
#include "../../header/sym.hpp"
#include "../common.cpp"
Val val_full(llvm::Value* value, llvm::Type* type, PType* ptype, bool is_lvalue, bool is_rvalue, const char* error);
Val val_type(PType* ptype, llvm::Type* type);
Val val_error(const char* fmt, ...);
Val val_int(llvm::Value* val);

SymClass::SymClass(const char* name, AggregateType* aggt, llvm::Type* t, std::vector<Sym*> m_fields = {}, std::vector<Sym*> m_methods = {}){
	this->name = name;
	this->m_fields = m_fields;
	this->m_methods = m_methods;
	this->val = val_type(aggt, t);
}
Sym* SymTable::find(const char* name){
	for (Sym* s : this->m_table) {
		assert(s->name);
		if (s->name == name) return s;
	}	
	return nullptr;
}
void SymTable::define(const char* name, Sym* sym) {
	if (this->find(name)) {
		printf("[ERROR]: the name %s is already defined in this scope.\n", name);
		return;
	}
	sym->name = name;
	this->m_table.push_back(sym);
}

void SymTable::show_all() {
	printf("SYM TABLE SHOW ALL.\n");
	for (Sym* s : this->m_table) {		
		printf("DEFINED %s\n", s->name);
	}
}

SymTable SymTable::Create() {	
	return SymTable();	
}

Sym::Sym(const char* name, Val val) {
	this->name = name;
	this->val = val;
}

int find_index_of_sym_vec(std::vector<Sym*> vec, const char* name) {
	for (int i = 0; i < vec.size(); i++) {
		Sym* s = vec.at(i);
		if (s->name == name) return i;
	}
	return -1;
}
int SymClass::get_field_index(const char* name) {	
	return find_index_of_sym_vec(this->m_fields, name);
}
Sym* SymClass::get_field(const char* name) { 
	int idx = find_index_of_sym_vec(this->m_fields, name);
	if (idx < 0) return nullptr;
	return this->m_fields.at(idx);
}
Sym* SymClass::get_method(const char* name) { 
	int idx = find_index_of_sym_vec(this->m_methods, name);
	if (idx < 0) return nullptr;
	return this->m_methods.at(idx);
}

Val val_full(llvm::Value* value, llvm::Type* type, PType* ptype, bool is_lvalue = false, bool is_rvalue = false, const char* error = 0) {
	return { value, type, ptype, is_lvalue, is_rvalue, error };
}
Val val_type(PType* ptype, llvm::Type* type) {
	return val_full(nullptr, type, ptype);
}
Val val_error(const char* fmt, ...) {
	BUILD_STRING_FROM_VARARG(fmt, error);	
	return val_full(0, 0, 0, false, false, error? error: "<error is corrupted>");
}
Val val_int(llvm::Value* val) {
	return val_full(val, val->getType(), new IntType("i64", 64), true);
}
Val val_float(llvm::Value* val) {
	return val_full(val, val->getType(), new FloatType("f64", 64), true);
}
Val val_str(llvm::Value* str) {
	return val_full(str, str->getType(), new PtrType(new IntType("i8", 8)), true, false, nullptr);
}
Val val_null() {
	return val_full(nullptr, nullptr, nullptr, false, false, nullptr);
}
#endif/*SYM_LLVM_COMPILE*/

