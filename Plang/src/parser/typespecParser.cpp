#ifndef GPARSETYPESPEC
#define GPARSETYPESPEC
#include "../../header/lexer.hpp" // for Lexer
#include "../ast/typespec.cpp"
#include "./parserCommon.cpp"

TypeSpec* parse_operandspec(Lexer* lexer);
TypeSpec* parse_basespec(Lexer* lexer);
TypeSpec* parse_namespec(Lexer* lexer);
TypeSpec* parse_ptrspec(Lexer* lexer);
TypeSpec* parse_arrayspec(Lexer* lexer);
std::vector<TypeSpec*> parse_typespec_list(Lexer* lexer, TokenKind tokenKindDelim);
//TypeSpec* parse_procspec(Lexer* lexer);
TypeSpec* parse_constspec(Lexer* lexer);
TypeSpec* parse_typespec(Lexer* lexer);
TypeSpec* parse_templatespec(SrcLocation opLoc, TypeSpec* operand, Lexer* lexer);

TypeSpec* parse_namespec(Lexer* lexer){		
	if (!lexer->getToken().isKind(TK_name)) syntax_error(lexer->getLocation(), "Expected name while parsing typespec.\n");
	SrcLocation loc = lexer->getLocation();
	const char* name = lexer->getToken().name;
	lexer->nextToken();
	return new NameTypeSpec(loc, name);
}
TypeSpec* parse_ptrspec(Lexer* lexer) {
	if (!lexer->getToken().isKind(TK_mul)) syntax_error(lexer->getLocation(), "Expected * while parsing pointer typespec.\n");
	SrcLocation loc = lexer->getLocation();
	lexer->nextToken();
	return new PtrTypeSpec(loc, parse_typespec(lexer));
}
TypeSpec* parse_arrayspec(Lexer* lexer) {
	if (!lexer->getToken().isKind(TK_lbracket)) syntax_error(lexer->getLocation(), "Expected [ while parsing array typespec.\n");
	SrcLocation loc = lexer->getLocation();
	lexer->nextToken();
	Expr* size = nullptr;
	if (!lexer->getToken().isKind(TK_rbracket)) size = parse_expr(lexer);

	if (!lexer->expect(TK_rbracket)) syntax_error(lexer->getLocation(), "Expected ] while parsing array typespec.\n");
	return new ArrayTypeSpec(loc, size, parse_typespec(lexer));	
}
std::vector<TypeSpec*> parse_typespec_list(Lexer* lexer, TokenKind tokenKindDelim){	
	std::vector<TypeSpec*> list;
	do {
		if (list.size() != 0) {
			if (!lexer->expect(TK_comma)) syntax_error(lexer->getLocation(), "Expected ',' while parsing typespec arguments.\n");
		}
		TypeSpec* arg = parse_typespec(lexer);
		if (!arg) return {};

		list.push_back(arg);

	} while (!lexer->isEof() and lexer->getToken().isKind(TK_comma) and !lexer->getToken().isKind(tokenKindDelim));
	return list;
}
TypeSpec* parse_templatespec(SrcLocation opLoc, TypeSpec* operand, Lexer* lexer) {
	if (!lexer->checkTemplate()) syntax_error(lexer->getLocation(), "expected template while parsing.\n");
	lexer->nextToken();

	std::vector<TypeSpec*> args; 
	if (!lexer->getToken().isKind(TK_greater)) {
		lexer->parsingTemplateContext();
		args = parse_typespec_list(lexer, TK_less);
		lexer->endOfparsingTemplateContext();
	}
	if (!lexer->getToken().isKind(TK_greater)) syntax_error(lexer->getLocation(), "parser tried to parse as a template a non-template typespec.\n");
	lexer->nextToken();
	return new TemplateTypeSpec(opLoc, operand, args);
}
TypeSpec* parse_operandspec(Lexer* lexer) {	
	return parse_namespec(lexer);	
}
TypeSpec* parse_basespec(Lexer* lexer){		
	if (lexer->getToken().isKind(TK_lbracket)) {
		return parse_arrayspec(lexer);
	}
	if (lexer->getToken().isKind(TK_mul)) {
		return parse_ptrspec(lexer);
	}
	bool isRef = lexer->expect(TK_and);
	TypeSpec* operand = parse_operandspec(lexer);
	if (lexer->checkTemplate()) {
		if (isRef) syntax_error(lexer->getLocation(), "Can't use reference '&' in template typespec.\n");
		
		SrcLocation loc = lexer->getLocation();
		return parse_templatespec(loc, operand, lexer);
	}
	operand->setRef(isRef);	
	return operand;	
}

TypeSpec* parse_constspec(Lexer* lexer) {
	if (!lexer->matchKeyword(const_keyword)) syntax_error(lexer->getLocation(), "Expected 'const' keyword.\n");
	bool isRef = lexer->expect(TK_and);
	
	return new ConstTypeSpec(lexer->getLocation(), parse_typespec(lexer), isRef);
}
TypeSpec* parse_typespec(Lexer* lexer){	
	if (lexer->matchKeyword(const_keyword)) {
		return parse_constspec(lexer);
	}
	else {
		return parse_basespec(lexer);
	}
}

#endif /*GPARSETYPESPEC*/