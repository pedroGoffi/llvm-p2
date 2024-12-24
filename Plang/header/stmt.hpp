#ifndef PIETRA_STMT_HEADER
#define PIETRA_STMT_HEADER
#include "./expr.hpp"
#include "./lexer.hpp"

class Stmt;
class ExprStmt;
class IfStmt;
class SwitchStmt;
class ForStmt;
class WhileStmt;
class ReturnStmt;

class Stmt {
private:	
	virtual void __p__() {} // to make it polymorphic
};

class ExprStmt : public Stmt {
public:	
	SrcLocation loc;
	Expr* expr;
	ExprStmt(SrcLocation loc, Expr* expr);
};

class ScopeStmt : public Stmt {
public:
	SrcLocation loc;
	std::vector<Stmt*> nodes;
	ScopeStmt(SrcLocation loc, std::vector<Stmt*> nodes);
};
class IfStmt : public Stmt {
public:
	SrcLocation loc;
	Expr* cond;
	ScopeStmt* scope;
	std::vector<std::pair<Expr*, ScopeStmt*>> elifs;
	ScopeStmt* elseScope;
	IfStmt(SrcLocation loc, Expr* cond, ScopeStmt* scope, std::vector<std::pair<Expr*, ScopeStmt*>> elifs, ScopeStmt* elseScope);
};
class SwitchStmt : public Stmt {};
class ForStmt : public Stmt {
public:
	SrcLocation loc;
	Expr* init; 
	Expr* cond;
	Expr* inc;
	ScopeStmt* scope;

	ForStmt(SrcLocation loc, Expr* init, Expr* cond, Expr* inc, ScopeStmt* scope);
};
class WhileStmt : public Stmt {
public:
	SrcLocation loc;
	Expr* cond;
	ScopeStmt* scope;
	WhileStmt(SrcLocation loc, Expr* cond, ScopeStmt* scope);
};
class ComptimeStmt: public Stmt {
public:
	SrcLocation loc;
	ScopeStmt* scope;
	ComptimeStmt(SrcLocation loc, ScopeStmt* scope);

};
class ReturnStmt : public Stmt {
public:
	SrcLocation loc;
	Expr* expr;
	ReturnStmt(SrcLocation loc, Expr* expr);

};

#endif /*PIETRA_STMT_HEADER*/