#ifndef GPARSEEXPR
#define GPARSEEXPR
#include "../../header/expr.hpp"
#include "../ast/expr.cpp"
#include "../common.cpp"
#include "../ast/exprPrinter.cpp"
#include "../../header/lexer.hpp"
#include "./parserCommon.cpp"

Expr* parse_new(Lexer* lexer, SrcLocation loc);
Expr* parse_init(Lexer* lexer, SrcLocation loc, const char* name);
Expr* parse_operand(Lexer* lexer);
Expr* parse_base(Lexer* lexer);
Expr* parse_unary(Lexer* lexer);
Expr* parse_mul(Lexer* lexer);
Expr* parse_add(Lexer* lexer);
Expr* parse_cmp(Lexer* lexer);
Expr* parse_and(Lexer* lexer);
Expr* parse_or(Lexer* lexer);
Expr* parse_ternary(Lexer* lexer);
Expr* parse_paren(Lexer* lexer);
Expr* parse_expr(Lexer* lexer);
std::vector<Expr*> parse_call_args(Lexer* lexer);


Expr* parse_new(Lexer* lexer, SrcLocation loc) {
	printf("Unimplemented NEW expr.\n");
	exit(1);
}
Expr* parse_init(Lexer* lexer, SrcLocation loc, const char* name) {
	// NOTE: name is already parsed at this point, we shall expect ':' (typespec ('=' expr), ':=' expr)
	/*
	* if not typespec then expr must be setted
	* if not expr then typespec must be setter
	* otherwise error
	*/
	if (!lexer->expect(TK_colon)) syntax_error(loc, "Expected ':' in variable declaration.\n");
	TypeSpec* typespec = nullptr;
	Expr* init = nullptr;
	
	if (!lexer->getToken().isKind(TK_eq)) {		
		typespec = parse_typespec(lexer);
	}

	if (lexer->getToken().isKind(TK_eq)) {
		lexer->nextToken();
		init = parse_expr(lexer);
	}

	if (!typespec and !init) syntax_error(loc, "Variable must have typespec or initialization declared.\n");
	
	return new InitExpr(loc, name, typespec, init);
}
std::vector<Expr*> parse_call_args(Lexer* lexer){
	if (!lexer->expect(TK_lparen)) syntax_error(lexer->getLocation(), "Expected '(' while parsing call args.\n");
	std::vector<Expr*> args;

	while (!lexer->isEof()) {
		if (lexer->getToken().isKind(TK_rparen)) break;

		if (args.size() > 0) {
			
			if (!lexer->expect(TK_comma)) syntax_error(lexer->getLocation(), "Expected ',' between function call arguments.\n");
		
		}

		Expr* arg = parse_expr(lexer);
		
		
		if (!arg) return {};

		args.push_back(arg);		
	}

	if (!lexer->expect(TK_rparen)) syntax_error(lexer->getLocation(), "Expected ')' got EOF.\n");
	return args;

}

Expr* parse_operand(Lexer* lexer){
	SrcLocation loc = lexer->getLocation();
	if (lexer->token.isKind(TK_int)) {
		long val = lexer->token.i64;
		lexer->nextToken();
		return new IntExpr(val, loc);
	}
	else if (lexer->token.isKind(TK_float)) {
		double f64 = lexer->token.f64;
		lexer->nextToken();
		return new FloatExpr(f64, loc);
	}
	else if (lexer->token.isKind(TK_dqstring)) {
		const char* str = lexer->token.string;
		lexer->nextToken();
		return new StringExpr(str, loc);
	}
	else if (lexer->token.isKind(TK_lparen)) {
		return parse_paren(lexer);
	}
	else if (lexer->token.name == new_keyword) {
		printf("NEW EXPR.\n");
		system("pause");
		exit(1);
	}
	else if (lexer->getToken().isKind(TK_name)) {
		const char* name = lexer->getToken().name;
		SrcLocation loc = lexer->getLocation();
		lexer->nextToken();
		if(lexer->getToken().isKind(TK_colon)){
			return parse_init(lexer, loc, name);
		}

		return new NameExpr(loc, name);
	}

	
	fatal_error(loc, "Unexpected token %s in expression.\n", lexer->token.name);
	return nullptr;
}

Expr* parse_base(Lexer* lexer) {
	Expr* expr = parse_operand(lexer);
	while (
		lexer->token.isKind(TK_lparen)		or
		lexer->token.isKind(TK_lbracket)	or
		lexer->token.isKind(TK_dot)			or
		lexer->token.isKind(TK_plusplus)	or
		lexer->token.isKind(TK_minusminus)	or
		lexer->token.isKind(TK_arrow))
	{
		
		SrcLocation loc = lexer->getLocation();
		if(lexer->token.isKind(TK_lparen)){
			std::vector<Expr*> args = parse_call_args(lexer);			
			expr = new CallExpr(loc, expr, args);
		}
		else if (lexer->token.isKind(TK_dot)) {
			lexer->nextToken();
			if (!lexer->getToken().isKind(TK_name)) syntax_error(lexer->getLocation(), "Expected token NAME after field access '.'.\n");
			const char* field = lexer->getToken().name;
			lexer->nextToken();
			expr = new FieldExpr(loc, expr, field, false);
		}
		else if (lexer->token.isKind(TK_arrow)) {
			lexer->nextToken();
			if (!lexer->getToken().isKind(TK_name)) syntax_error(lexer->getLocation(), "Expected token NAME after field access '.'.\n");
			const char* field = lexer->getToken().name;
			lexer->nextToken();
			expr = new FieldExpr(loc, expr, field, true);
		}
		else if (lexer->token.isKind(TK_lbracket)) {
			lexer->nextToken();

			if (lexer->getToken().isKind(TK_rbracket)) syntax_error(lexer->getLocation(), "Expected index to access the array.\n");
			Expr* index = parse_expr(lexer);
			if (!lexer->expect(TK_rbracket)) syntax_error(lexer->getLocation(), "Expected ']' after array access.\n");
			expr = new ArrayAccessExpr(loc, expr, index);
		}
		else if (lexer->token.isKind(TK_plusplus) or lexer->token.isKind(TK_minusminus)) {
			bool isInc = lexer->token.isKind(TK_plusplus);
			lexer->nextToken();

			expr = new ModifyExpr(loc, expr, isInc);
		}
		
		else {
			printf("Unimplemented parse_base\n");
			system("pause");
			exit(1);
		}
	}
	return expr;
}
static bool isUnary(Lexer* lexer) {
	return
		lexer->token.isKind(TK_plusplus) ||
		lexer->token.isKind(TK_minusminus) ||
		lexer->token.isKind(TK_mul) ||
		lexer->token.isKind(TK_and) ||
		lexer->token.isKind(TK_minus) ||
		lexer->token.isKind(TK_not) ||
		lexer->token.isKind(TK_plus) ||
		lexer->token.isKind(TK_minus);
}
Expr* parse_unary(Lexer* lexer){
	if (isUnary(lexer)) {
		SrcLocation loc = lexer->getLocation();
		Token token = lexer->getToken();
		lexer->nextToken();

		if (token.isKind(TK_plusplus) or token.isKind(TK_minusminus)) {
			// TODO: modify expr 
			exit(1);			
		}
		else {
			return new UnaryExpr(loc, token, parse_unary(lexer));
		}
	}

	return parse_base(lexer);
}
static bool isExprMul(Lexer* lexer) {
	return lexer->token.isKind(TK_mul)
		or lexer->token.isKind(TK_div)
		or lexer->token.isKind(TK_mod)
		or lexer->token.isKind(TK_and)
		or lexer->token.isKind(TK_shl)
		or lexer->token.isKind(TK_shr);
}
Expr* parse_mul(Lexer* lexer) {
	Expr* expr = parse_unary(lexer);
	while (isExprMul(lexer)) {
		SrcLocation loc = lexer->getLocation();
		Token op = lexer->getToken();
		lexer->nextToken();
		expr = new BinaryExpr(loc, op, expr, parse_unary(lexer));
	}
	return expr;
}
static bool isExprAdd(Lexer* lexer){			
	return lexer->token.isKind(TK_plus)
		or lexer->token.isKind(TK_minus)
		or lexer->token.isKind(TK_xor)
		or lexer->token.isKind(TK_or);
	
}
Expr* parse_add(Lexer* lexer) {
	Expr* expr = parse_mul(lexer);
	while(isExprAdd(lexer)){
		SrcLocation loc = lexer->getLocation();
		Token op = lexer->getToken();
		lexer->nextToken();
		expr = new BinaryExpr(loc, op, expr, parse_mul(lexer));
	}
	return expr;
}
static bool isExprCmp(Lexer* lexer) {
	return lexer->token.isKind(TK_eq)
		or lexer->token.isKind(TK_noteq)
		or lexer->token.isKind(TK_less)
		or lexer->token.isKind(TK_greater)
		or lexer->token.isKind(TK_lesseq)
		or lexer->token.isKind(TK_greatereq);
}
Expr* parse_cmp(Lexer* lexer) {
	Expr* expr = parse_add(lexer);
	while (isExprCmp(lexer)) {
		SrcLocation loc = lexer->getLocation();
		Token op = lexer->getToken();
		lexer->nextToken();
		expr = new BinaryExpr(loc, op, expr, parse_add(lexer));
	}
	return expr;

}
Expr* parse_and(Lexer* lexer) {
	Expr* expr = parse_cmp(lexer);
	while (lexer->token.isKind(TK_andand)) {				
		SrcLocation loc = lexer->getLocation();
		Token token = lexer->getToken();
		lexer->nextToken();
		expr = new BinaryExpr(loc, token, expr, parse_cmp(lexer));
	}
	return expr;
}
Expr* parse_or(Lexer* lexer) {
	Expr* expr = parse_and(lexer);	
	
	while (lexer->token.isKind(TK_oror)) {		
		SrcLocation loc = lexer->getLocation();
		Token token = lexer->getToken();
		lexer->nextToken();
		expr = new BinaryExpr(loc, token, expr, parse_and(lexer));
	}
	return expr;
}

Expr* parse_ternary(Lexer* lexer) {
	SrcLocation loc = lexer->getLocation();
	Expr* expr = parse_or(lexer);
	if (lexer->expect(TK_question)) {
		Expr* thenExpr = parse_ternary(lexer);
		if (!lexer->expect(TK_colon)) fatal_error(lexer->getLocation(), "Expected ':' in ternary expression.\n");
		Expr* elseExpr = parse_ternary(lexer);
		expr = new TernaryExpr(loc, expr, thenExpr, elseExpr);
	}
	return expr;
}
Expr* parse_paren(Lexer* lexer) {
	if (!lexer->expect(TK_lparen)) fatal_error(lexer->getLocation(), "Expected ')'.\n");
	
	Expr* expr = parse_expr(lexer);
	if (!lexer->expect(TK_rparen)) fatal_error(lexer->getLocation(), "Expected '('.\n");

	return expr;
}



Expr* parse_expr(Lexer* lexer) {	
	return parse_ternary(lexer);
}


#endif /*GPARSEEXPR*/