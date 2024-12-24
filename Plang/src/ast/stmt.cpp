#ifndef STMT_AST
#define STMT_AST
#include "../../header/stmt.hpp"

ExprStmt::ExprStmt(SrcLocation loc, Expr* expr) {
	this->loc = loc;
	this->expr = expr;
}
ScopeStmt::ScopeStmt(SrcLocation loc, std::vector<Stmt*> nodes) {
	this->loc = loc;
	this->nodes = nodes;
}
IfStmt::IfStmt(SrcLocation loc, Expr* cond, ScopeStmt* scope, std::vector<std::pair<Expr*, ScopeStmt*>> elifs, ScopeStmt* elseScope) {
	this->loc = loc;
	this->cond = cond;
	this->scope = scope;
	this->elifs = elifs;
	this->elseScope = elseScope;
}
ForStmt::ForStmt(SrcLocation loc, Expr* init, Expr* cond, Expr* inc, ScopeStmt* scope) {
	this->loc = loc;
	this->init = init;
	this->inc = inc;
	this->cond = cond;
	this->scope = scope;
}
WhileStmt::WhileStmt(SrcLocation loc, Expr* cond, ScopeStmt* scope) {
	this->loc = loc;
	this->cond = cond;
	this->scope = scope;
}
ComptimeStmt::ComptimeStmt(SrcLocation loc, ScopeStmt* scope) {
	this->loc = loc;
	this->scope = scope;
}
ReturnStmt::ReturnStmt(SrcLocation loc, Expr* expr) {
	this->loc = loc;
	this->expr = expr;
}
#endif /*STMT_AST*/