#ifndef SYM_LLVM_COMPILE_HEADER
#define SYM_LLVM_COMPILE_HEADER
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <map>
#include "type.hpp"

class Val {
public:
	llvm::Value* value;
	llvm::Type* type;
	PType* ptype;

	bool is_lvalue;
	bool is_rvalue;
	const char* error;
};

class Sym {
private:
	virtual void __p__(){}
public:
	const char* name;
	Val val;

	Sym() = default;
	Sym(const char* name, Val val);
};
class SymClass : public Sym{
public:
	const char* name;
	std::vector<Sym*> m_fields;
	std::vector<Sym*> m_methods;
	
	SymClass() = default;
	SymClass(const char* name, AggregateType* aggt, llvm::Type* t, std::vector<Sym*> m_fields, std::vector<Sym*> m_methods);

	int get_field_index(const char* name);
	Sym* get_field(const char* name);
	Sym* get_method(const char* name);

};
class SymTable: public Sym {
	std::vector<Sym*> m_table;
	
public:

	Sym* find(const char* name);
	void define(const char* name, Sym* sym);
	static SymTable Create();
	
	void show_all();
};
#endif/*SYM_LLVM_COMPILE_HEADER*/
