#ifndef EXPR_AST
#define EXPR_AST
#include "../../header/expr.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ValueExpr::init(){
	this->i64 = 0;
	this->string = 0;
	this->f64 = 0;
	this->kind = ValueExpr::VAL_none;
}
ValueExpr::ValueExpr(const char* string, SrcLocation pos){
	this->init();
	this->kind = ValueExpr::VAL_string;
	this->string = string;
	this->loc = loc;
}
ValueExpr::ValueExpr(double f64, SrcLocation pos){
	this->init();
	this->kind = ValueExpr::VAL_float;
	this->f64 = f64;
	this->loc = loc;
}
ValueExpr::ValueExpr(long i64, SrcLocation pos) {
	this->init();
	this->kind = ValueExpr::VAL_integer;	
	this->i64 = i64;
	this->loc = loc;
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntExpr::IntExpr(long i64, SrcLocation loc) : ValueExpr(i64, loc) {}
long& IntExpr::getI64() {
	return this->i64;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

FloatExpr::FloatExpr(double f64, SrcLocation loc) : ValueExpr(f64, loc) {}
double& FloatExpr::getFloat() { return this->f64;  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
InitExpr::InitExpr(SrcLocation loc, const char* name, TypeSpec* typespec, Expr* init) {
	this->loc = loc;
	this->name = name;
	this->typespec = typespec;
	this->init = init;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
StringExpr::StringExpr(const char* str, SrcLocation loc) : ValueExpr(str, loc) {}
const char*& StringExpr::getString() { return this->string; }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
NameExpr::NameExpr(SrcLocation loc, const char* name) : ValueExpr(name, loc){}
const char*& NameExpr::getName() { return this->string;  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CallExpr::CallExpr(SrcLocation loc, Expr* base, std::vector<Expr*> args) {
	this->loc = loc;
	this->base = base;
	this->args = args;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FieldExpr::FieldExpr(SrcLocation loc, Expr* base, const char* field, bool auto_derref) {
	this->loc = loc;
	this->base = base;
	this->field = field;
	this->auto_derref = auto_derref;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
ArrayAccessExpr::ArrayAccessExpr(SrcLocation loc, Expr* base, Expr* index) {
	this->loc = loc;
	this->base = base;
	this->index = index;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModifyExpr::ModifyExpr(SrcLocation loc, Expr* expr, bool isInc) {
	this->loc = loc;
	this->expr = expr;
	this->isInc = isInc;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
UnaryExpr::UnaryExpr(SrcLocation loc, Token token, Expr* expr){
	this->loc = loc;
	this->token = token;
	this->expr = expr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BinaryExpr::BinaryExpr(const SrcLocation loc, const Token token, Expr* lhs, Expr* rhs) {
	this->loc = loc;
	this->token = token;
	this->lhs = lhs;
	this->rhs = rhs;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
TernaryExpr::TernaryExpr(const SrcLocation loc, Expr* condExpr, Expr* thenExpr, Expr* elseExpr) {
	this->loc = loc;
	this->condExpr = condExpr;
	this->thenExpr = thenExpr;
	this->elseExpr = elseExpr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /*EXPR_AST*/