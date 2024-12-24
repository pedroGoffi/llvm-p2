#ifndef GPARSESTMT
#define GPARSESTMT
#include "../ast/stmt.cpp"
#include "./exprParser.cpp"
#include "../lexer/lexer.cpp"
#include "../../header/srcLocation.hpp"
//Stmt* parse_if(Lexer* lexer);
Stmt* parse_stmt(Lexer* lexer);
Stmt* parse_while(Lexer* lexer);
Stmt* parse_for(Lexer* lexer);
Stmt* parse_simple(Lexer* lexer);
ScopeStmt* parse_scope(Lexer* lexer);



ScopeStmt* parse_scope(Lexer* lexer) {
	if (lexer->expect(TK_semicolon)) return nullptr;
	if (!lexer->expect(TK_lcurly)) syntax_error(lexer->getLocation(), "Tried to parse a scope, but didn't found the '{' at the beginning of the scope.\n");
	SrcLocation loc = lexer->getLocation();
	std::vector<Stmt*> nodes;
	while (!lexer->isEof()) {
		if (lexer->getToken().isKind(TK_rcurly)) break;

		Stmt* node = parse_stmt(lexer);
		if (!node) return nullptr;
		nodes.push_back(node);
	}

	if (!lexer->expect(TK_rcurly)) syntax_error(lexer->getLocation(), "Tried to parse a scope, but didn't found the '}' at the end of the scope.\n");
	return new ScopeStmt(loc, nodes);
}

Stmt* parse_for(Lexer* lexer){
	if (!lexer->expectKeyword(for_keyword)) syntax_error(lexer->getLocation(), "Expected keyword 'for' while parsing for-loop.\n");
	SrcLocation loc = lexer->getLocation();
	/*
	* for {}
	* for x:int = 10; x < 10; x++ {}
	*/
	
	Expr* init	=	nullptr;
	if (!lexer->getToken().isKind(TK_lcurly)) init = parse_expr(lexer);
	Expr* cond	=	nullptr;
	if (!lexer->getToken().isKind(TK_lcurly) and lexer->getToken().isKind(TK_semicolon)) {
		lexer->nextToken();
		cond = parse_expr(lexer);
	}
	Expr* inc	=	nullptr;
	if (!lexer->getToken().isKind(TK_lcurly) and lexer->getToken().isKind(TK_semicolon)){
		lexer->nextToken();
		inc = parse_expr(lexer);
	}
	ScopeStmt* scope = nullptr;
	/*
	* for express ;
	* for 0;0;0;
	*/
	if (!lexer->getToken().isKind(TK_lcurly)) {
		if (!init and !cond and !inc) syntax_error(lexer->getLocation(), "Error for loop without body expects initialization, condition and increment expressions to be setted up.\n");
	}
	else scope = parse_scope(lexer);
		
	return new ForStmt(loc, init, cond, inc, scope);
}
Stmt* parse_while(Lexer* lexer) {
	if (!lexer->expectKeyword(while_keyword)) syntax_error(lexer->getLocation(), "Expected keyword 'for' while parsing for-loop.\n");
	SrcLocation loc = lexer->getLocation();
	Expr* cond = nullptr;
	ScopeStmt* scope = nullptr;

	if (!lexer->getToken().isKind(TK_lcurly)) cond = parse_expr(lexer);
	if (lexer->getToken().isKind(TK_lcurly)) scope = parse_scope(lexer);

	return new WhileStmt(loc, cond, scope);
}
Stmt* parse_simple(Lexer* lexer) {
	SrcLocation loc = lexer->getLocation();
	Stmt* stmt = new ExprStmt(loc, parse_expr(lexer));
	lexer->expectSemiColon();
	return stmt;	
}
ComptimeStmt* parse_comptime(Lexer* lexer) {
	SrcLocation loc = lexer->getLocation();
	if (!lexer->expectKeyword(comptime_keyword)) syntax_error(lexer->getLocation(), "Expected comptime keyword while tried to parse comptime statement.\n");
	ScopeStmt* scope = parse_scope(lexer);
	return new ComptimeStmt(loc, scope);
}

Stmt* parse_stmt(Lexer* lexer){
	//SrcLocation loc = lexer->getLocation();
	if(lexer->matchKeyword(for_keyword)){
		return parse_for(lexer);
	}
	else if (lexer->matchKeyword(while_keyword)) {
		return parse_while(lexer);
	}
	else if (lexer->matchKeyword(comptime_keyword)) {
		return parse_comptime(lexer);
	}
	else if(lexer->matchKeyword(return_keyword)){
		lexer->nextToken();
		SrcLocation loc = lexer->getLocation();
		Expr* retExpr = nullptr;
		if (!lexer->getToken().isKind(TK_semicolon)) {
			retExpr = parse_expr(lexer);
		}
		lexer->expectSemiColon();
		return new ReturnStmt(loc, retExpr);
	}
	else {
		return parse_simple(lexer);
	}	
}
#endif/*GPARSESTMT*/