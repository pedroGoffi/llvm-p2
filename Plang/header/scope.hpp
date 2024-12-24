#ifndef LLVM_SCOPES_HEADER
#define LLVM_SCOPES_HEADER
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/TypedPointerType.h"
#include "llvm/ADT/AddressRanges.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <llvm/Support/FileSystem.h>
#include <map>
#include "./Sym.hpp"
class PContext;

/*
* scope division
* FUNCTION
* FUNCTION PARAMS
* FUNCTION BODY
* 
* RESET AT FUNCTION and then compile the next function to avoid parameter name colision
*/
class Scope {
public:
	std::vector<SymTable> m_scopes;
	std::vector<SymTable> m_classes;
public:
	Scope();
	void define_named_sym_in_last_scope(const char* name, Val val);
	Sym* get_sym_by_name(const char* name);

	void define_cls(const char* name, Val val, llvm::StructType* type, PContext* ctx);
	SymClass* get_cls(const char*);
	void enter_scope();
	void leave_scope();
	void show_all();
	SymTable* get_last_scope();
	SymTable* get_table_at(size_t index);
	int get_size();
};

class Errs {
public:
	std::vector<const char*> errs;
	const char* add_error(const char* fmt, ...);
	const char* consume_error();
	const bool has_error() const;
};
class ModuleManager {
public:	
	std::vector<std::filesystem::path> paths;

	bool add_path(std::filesystem::path path);
	bool find_path(std::filesystem::path path);
};
class PContext {
private:
	bool	m_expect_load_addr;
	Val		m_current_self = val_error("Self is not defined yet");
public:
	bool				defined_return;
	llvm::LLVMContext	ctx;
	llvm::IRBuilder<>*	builder;
	llvm::Module*		mod;
	Scope scope;
	Errs errs;
	llvm::Function* current_function;
	ModuleManager	modMan;
public:
	PContext();

	void context_variables_rewind();
	void set_current_self(Val val);
	Val get_current_self();

	SymTable* global_scope();
	Scope& get_mut_scope();
	const bool expect_load_addr();
	Val* get_ptr(const char* name);
	void define(const char* name, Val val);

	Sym* get_ptr_cls_sym(const char* name);
	Val* get_ptr_cls(const char* name);
	void define_cls(const char* name, Val val, llvm::StructType* type, PContext* ctx);
	void set_load_addr(bool state);
	int enter_scope();
	int leave_scope();
	llvm::Function* get_current_function();
	void set_current_function(llvm::Function* function);
	void drop_current_function();

	void show_all();
};
#endif/*LLVM_SCOPES_HEADER*/