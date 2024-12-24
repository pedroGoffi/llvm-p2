#ifndef PIETRA_DECL_HEADER
#define PIETRA_DECL_HEADER
#include "./typespec.hpp"
#include "../header/lexer.hpp"

class Decl {
private:
	virtual void __p__() {}
public:
	const char* name;
};

class ProcParams {
public:
	SrcLocation loc;
	std::vector<FieldTypeSpec*> params;	
	bool isVarArg;
	size_t varArgPos;
	ProcParams(SrcLocation loc, std::vector<FieldTypeSpec*> params, bool isVarArg, size_t varArgPos);
};
class ProcDecl: public Decl {
public:
	SrcLocation loc;	
	ProcParams* params;
	TypeSpec* ret;
	ScopeStmt* scope;
	bool isExtern;	
	ProcDecl(SrcLocation loc, const char* name, ProcParams* params, TypeSpec* ret, ScopeStmt* scope, bool isExtern);
};
class AggregateDecl : public Decl {
public:
	SrcLocation loc;	
	bool isStruct;
	std::vector<FieldTypeSpec*> fields;
	std::vector<Decl*> methods;
	AggregateDecl(SrcLocation loc, const char* name, bool isStruct, std::vector<FieldTypeSpec*> fields, std::vector<Decl*> methods);

};
class UseModuleDecl : public Decl {
public:
	SrcLocation loc;
	std::vector<const char*>	scopes;
	std::vector<const char*>	names;
	std::filesystem::path		path;
	bool						importAll;
	UseModuleDecl(SrcLocation lo, std::filesystem::path fullPath, std::vector<const char*> scope, std::vector<const char*> name, bool importAll);
};
class ConstDecl : public Decl {
public:
	SrcLocation loc;
	TypeSpec* typespec;
	Expr* init;
	ConstDecl(SrcLocation loc, const char* name, TypeSpec* typespec, Expr* init);
};
#endif /*PIETRA_DECL_HEADER*/