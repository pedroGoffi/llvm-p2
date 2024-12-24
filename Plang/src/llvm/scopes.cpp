#ifndef LLVM_SCOPES
#define LLVM_SCOPES
#include <map>
#include "./Sym.cpp"
#include "../../header/scope.hpp"
#include "./typeCompiler.cpp"


bool ModuleManager::add_path(std::filesystem::path path) {
	if (!this->find_path(path)) {
		this->paths.push_back(path);
		return true;
	}
	return false;
}
bool ModuleManager::find_path(std::filesystem::path path){
	for (auto p : this->paths) {
		if (p == path) return true;
	}
	return false;
}

PContext::PContext(){
	this->builder = new llvm::IRBuilder<>(this->ctx);
	this->mod = new llvm::Module("Pietra_lang", this->ctx);
	this->current_function = nullptr;	
}

void PContext::define(const char* name, Val val) {
	this->scope.define_named_sym_in_last_scope(name, val);	
}
void PContext::context_variables_rewind(){
	this->defined_return = false;
	this->set_load_addr(true);
	this->set_current_self(val_null());
}
void PContext::set_load_addr(bool state) {
	this->m_expect_load_addr = state;
}
void PContext::set_current_self(Val val) { 
	val.error = nullptr;
	this->m_current_self = val; 
}
Val PContext::get_current_self() { return this->m_current_self; }
SymTable* PContext::global_scope() { return this->scope.get_table_at(0); }
Scope& PContext::get_mut_scope() { return this->scope; }
const bool PContext::expect_load_addr() {
	return this->m_expect_load_addr;
}
Val* PContext::get_ptr(const char* name) {
	if (Sym * s = this->scope.get_sym_by_name(name)) return &s->val;
	return nullptr;
}

Val* PContext::get_ptr_cls(const char* name) {
	Sym* s = this->scope.get_cls(name);
	if (s) return &s->val;
	return nullptr;
}
Sym* PContext::get_ptr_cls_sym(const char* name) {
	Sym* s = this->scope.get_cls(name);
	if (s) return s;
	return nullptr;
}
void PContext::define_cls(const char* name, Val val, llvm::StructType* type, PContext* ctx){
	AggregateType* agg = dynamic_cast<AggregateType*>(val.ptype);
	if (!agg) {
		printf("[ERROR]: Trying to define a non struct as a class.\n");
		return;
	}	
	
	this->scope.define_cls(name, val, type, ctx);
}

int PContext::enter_scope() {	
	this->scope.enter_scope(); 
	return this->scope.get_size() - 1;
}
int PContext::leave_scope() { 		
	this->scope.leave_scope(); 
	return this->scope.get_size();
}

llvm::Function* PContext::get_current_function() { return this->current_function; }

void PContext::set_current_function(llvm::Function* function) { this->current_function = function; }
void PContext::drop_current_function() { this->current_function = nullptr; }
void PContext::show_all() {
	this->scope.show_all();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//class Sym {
//private:
//	virtual void __p__() {}
//public:
//	const char* name;
//	Val val;
//
//};
//void PContext::define(const char*, Val val) {
//	this->scope.
//}
Scope::Scope() {	
	this->m_scopes.push_back(SymTable::Create());
	this->m_classes.push_back(SymTable::Create());
}
Sym* Scope::get_sym_by_name(const char* name) {
	SymTable& table = this->m_scopes.back();
	return table.find(name);
}
void Scope::define_named_sym_in_last_scope(const char* name, Val val){
	if (this->get_sym_by_name(name)) {
		printf("[ERROR]: the name %s is already declared in this scope.\n", name);
		return;
	}
	SymTable& table = this->m_scopes.back();
	Sym* sym = new Sym(name, val);
	table.define(name, sym);	
}
void Scope::define_cls(const char* name, Val val, llvm::StructType* type, PContext* ctx) {	
	if (this->get_cls(name)) {
		printf("[ERROR]: the class %s is already declared in this scope.\n", name);
		return;
	}
	SymTable& table = this->m_classes.back();
	AggregateType* agg_t = dynamic_cast<AggregateType*>(val.ptype);
	if (!agg_t) {
		printf("[ERROR]: define_cls expects aggregate type as ptype.\n");
		return;
	}
	SymClass* cls = new SymClass(name, agg_t, val.type, {}, {});
	
	for (PType* field_ptype : agg_t->fields) {
		FieldType* field_t = dynamic_cast<FieldType*>(field_ptype);
		if (!field_t) {
			printf("[ERROR]: Failed to mount the struct %s\n", name);
			return;
		}
		Val ftype = compile_type(field_t->type, ctx);
		if (ftype.error) {
			printf("[ERROR]: Failed to mount the struct %s due to: %s\n", name, ftype.error);
			return;
		}
		Sym* field_sym = new Sym(field_t->name, ftype);
		cls->m_fields.push_back(field_sym);
	}
	
	cls->name = name;
	cls->val.type = type;
	table.define(name, cls);
}

SymClass* Scope::get_cls(const char* name){
	SymTable& table = this->m_classes.back();
	return dynamic_cast<SymClass*>(table.find(name));
	
}
void Scope::enter_scope() {	
	// first scope -> don't has n-1 scope
	if(this->m_scopes.size() > 0){ 
		// not first scope
		SymTable last_scope = this->m_scopes.back();
		this->m_scopes.push_back(SymTable::Create());
		this->m_scopes.back() = last_scope; // copy from n - 1 scope
	}
	// not first scope -> has n-1 scope
	else {
		// first scope
		this->m_scopes.push_back(SymTable::Create());
	}

	// first scope -> don't has n-1 scope
	if (this->m_classes.size() > 0) {
		// not first scope
		SymTable last_scope = this->m_classes.back();
		this->m_classes.push_back(SymTable::Create());
		this->m_classes.back() = last_scope; // copy from n - 1 scope
	}
	// not first scope -> has n-1 scope
	else {
		// first scope
		this->m_classes.push_back(SymTable::Create());
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
int Scope::get_size() { return this->m_scopes.size(); }
SymTable* Scope::get_table_at(size_t index) {
	if (index > this->m_scopes.size()){
		printf("[ERROR]: vector out of size at get_table_at.\n");
		return nullptr;
	}
	return &this->m_scopes.at(index);
}
SymTable* Scope::get_last_scope() { 
	if (this->m_scopes.size() == 0) {
		return nullptr;
	}
	return &this->m_scopes.back(); 
}
void Scope::show_all() {
	if (this->m_scopes.size() == 0) {
		printf("SCOPE SIZE = 0\n");
		return;
	}

	SymTable& table = this->m_scopes.back();
	
	table.show_all();
}
void Scope::leave_scope(){
	if (this->m_scopes.size() == 0) {
		printf("[ERROR]: scope reach end, tried to leave.\n");
		exit(1);
	}	
	this->m_scopes.pop_back();

	if (this->m_classes.size() == 0) {
		printf("[ERROR]: scope reach end, tried to leave.\n");
		exit(1);
	}
	this->m_classes.pop_back();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
const char* Errs::add_error(const char* fmt, ...) { 
	va_list args;
	va_start(args, fmt);
	size_t n = 1 + vsnprintf(NULL, 0, fmt, args);
	va_end(args);
	char* str = (char*) xmalloc(n);
	va_start(args, fmt);
	vsnprintf(str, n, fmt, args);
	va_end(args);
	this->errs.push_back(str);	
	return str;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
const char* Errs::consume_error() {
	if (this->errs.size() > 0) {
		const char* last_err = this->errs.back();
		this->errs.pop_back();
		return last_err;
	}
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
const bool Errs::has_error() const { return this->errs.size() > 0; }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#endif/*LLVM_SCOPES*/