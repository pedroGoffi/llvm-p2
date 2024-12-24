#ifndef GPARSEDECL
#define GPARSEDECL
#include "../../header/lexer.hpp" // for Lexer
#include "../../header/module.hpp"
#include "../ast/decl.cpp"
#include "../../header/decl.hpp"

Decl* parse_decl(Lexer* lexer);
ProcParams* parse_fn_params(Lexer* lexer);
FieldTypeSpec* parse_fn_param(Lexer* lexer);
Decl* parse_aggregate(Lexer* lexer);
Decl* parse_use_module(Lexer* lexer);
void parse_module_arg(Lexer* lexer, std::vector<const char*>& vecDst, bool& importAll);
std::filesystem::path transform_use_scope_to_filepath(Lexer* lexer, std::vector<const char*> scopes);

void parse_new_module(std::filesystem::path filePath);

static bool is_module_args(Lexer* lexer) {
	/*
	* use module::*
	* use module::{*}
	* use module::{printf, ...}
	* use module::{...}
	* use module::printf	
	*/
}
void parse_module_arg(Lexer* lexer, std::vector<const char*>& vecDst, bool& importAll)
{
	if (lexer->expect(TK_mul)) {
		importAll = true;
	}
	else {
		const char* name = lexer->getToken().name;
		assert(name);
		lexer->nextToken();
		vecDst.push_back(name);
	}
}
void parse_use_module_args(Lexer* lexer, std::vector<const char*>& vecDst, bool& importAll){
	if(!lexer->expect(TK_lcurly)){
		parse_module_arg(lexer, vecDst, importAll);
		return;		
	}
	if (lexer->getToken().isKind(TK_name)) {
		do {
			if (vecDst.size() > 0) {
				if (!lexer->expect(TK_comma)) syntax_error(lexer->getLocation(), "Expected ',' between names in use module.\n");
			}
			

			parse_module_arg(lexer, vecDst, importAll);
			
		
		} while (!lexer->isEof() and !lexer->getToken().isKind(TK_rcurly) and (
			lexer->getToken().isKind(TK_comma) or 
			lexer->getToken().isKind(TK_name)
		));
	}
	if (!lexer->expect(TK_rcurly)) syntax_error(lexer->getLocation(), "Expected '}' after use module arguments.\n");	
}
std::filesystem::path transform_use_scope_to_filepath(Lexer* lexer, std::vector<const char*> scopes){	
	std::filesystem::path parentContextPath = lexer->contextPath.parent_path();
	for (const char* scope : scopes) {		
		parentContextPath += "\\";
		parentContextPath += scope;		
	}
	parentContextPath += PIETRA_EXTENSION;
	
	return parentContextPath;
}
Decl* parse_use_module(Lexer* lexer) {
	SrcLocation loc = lexer->getLocation();
	if (!lexer->expectKeyword(use_keyword)) syntax_error(lexer->getLocation(), "Expected keyword 'use' in use module.\n");

	std::vector<const char*> scopes;
	do {						
		assert(lexer->getToken().isKind(TK_name));

		const char* scope = lexer->getToken().name;
		lexer->nextToken();
		scopes.push_back(scope);
		if (lexer->getToken().isKind(TK_scoperes)) lexer->nextToken();

	} while (
		!lexer->isEof()
		and (	lexer->getToken().isKind(TK_name)
			or	lexer->getToken().isKind(TK_scoperes)));

	std::filesystem::path fullPath = transform_use_scope_to_filepath(lexer, scopes);
	std::vector<const char*> names;
	bool importAll = false;
	parse_use_module_args(lexer, names, importAll);
	lexer->expectSemiColon();
	

	return new UseModuleDecl(loc, fullPath, scopes, names, importAll);
}
FieldTypeSpec* parse_fn_param(Lexer* lexer){
	SrcLocation loc = lexer->getLocation();
	if (!lexer->getToken().isKind(TK_name)) syntax_error(lexer->getLocation(), "Expected name in field name. <NAME> : <TYPESPEC>\n");
	const char* name = lexer->getToken().name;
	lexer->nextToken();

	if (!lexer->expect(TK_colon)) syntax_error(lexer->getLocation(), "Expeted ':' after field name name. <NAME> : <TYPESPEC>\n");

	TypeSpec* typespec = parse_typespec(lexer);
	Expr* defaultInit = nullptr;
	if (lexer->expect(TK_eq)) {
		defaultInit = parse_expr(lexer);
	}

	return new FieldTypeSpec(loc, name, typespec, defaultInit);
}
ProcParams* parse_fn_params(Lexer* lexer){
	SrcLocation loc = lexer->getLocation();
	bool isVarArg = false;
	int varArgPos = -1;
	int thisPos = 0;
	if (!lexer->expect(TK_lparen)) syntax_error(lexer->getLocation(), "Expected '(' after function name, for it's arguments.\n");
	std::vector<FieldTypeSpec*> params;
	while (!lexer->isEof()) {
		if (lexer->getToken().isKind(TK_rparen)) break;
		if (params.size() != 0) {
			if (!lexer->expect(TK_comma)) syntax_error(lexer->getLocation(), "Expected ',' between function parameters.\n");
		}

		if (lexer->expect(TK_tripledot)) {
			isVarArg = true;
			varArgPos = thisPos;
			break;
		}
		else {
			FieldTypeSpec* param = parse_fn_param(lexer);
			if (!param) return nullptr;
			params.push_back(param);
			thisPos++;
		}
	}

	if (!lexer->expect(TK_rparen)) {
		if (isVarArg) syntax_error(lexer->getLocation(), "Expected variadic argument to be the last argument.\n");		
		syntax_error(lexer->getLocation(), "Expected ')' after function arguments.\n");
	}
	
	
	return new ProcParams(loc, params, isVarArg, varArgPos);
}
std::pair<std::vector<FieldTypeSpec*>, std::vector<Decl*>> parse_aggregate_fields(Lexer* lexer){
	if (!lexer->expect(TK_lcurly)) syntax_error(lexer->getLocation(), "Expected '{' do declare struct fields.\n");
	// same syntax
	std::pair<std::vector<FieldTypeSpec*>, std::vector<Decl*>> intrisics;

	while (!lexer->isEof()) {
		if (lexer->getToken().isKind(TK_rcurly)) break;		
		/*
		* var / fn 
		*/ 
		if (lexer->matchKeyword(fn_keyword)) {
			Decl* func = parse_fn(lexer);
			if (!func) {
				printf("[ERROR]: mah que bah qq tu ta fazendo.\n");
				return {};
			}

			// NOTE: Optionaly define Self as struct the current type
			intrisics.second.push_back(func);
		}
		else {
			FieldTypeSpec* field = parse_fn_param(lexer);
			if (!field) return {};
			intrisics.first.push_back(field);
		}
	}
	if (!lexer->getToken().isKind(TK_rcurly)) syntax_error(lexer->getLocation(), "Expected '}' after struct body.\n");
	lexer->nextToken();
	return intrisics;
}
Decl* parse_aggregate(Lexer* lexer){
	if (	!lexer->matchKeyword(struct_keyword)
		and !lexer->matchKeyword(union_keyword)) syntax_error(lexer->getLocation(), "Expected 'struct' or 'union' while tried to parse aggregate.\n");
	

	bool isStruct = lexer->matchKeyword(struct_keyword);
	lexer->nextToken();
	if (!lexer->getToken().isKind(TK_name)) syntax_error(lexer->getLocation(), "Expected token in struct name definition.\n");

	const char* name = lexer->getToken().name;
	SrcLocation loc = lexer->getLocation();
	lexer->nextToken();
	
	std::pair<std::vector<FieldTypeSpec*>, std::vector<Decl*>> fields = parse_aggregate_fields(lexer);
	
	
	return new AggregateDecl(loc, name, isStruct, fields.first, fields.second);
}
Decl* parse_fn(Lexer* lexer) {
	if (!lexer->expectKeyword(fn_keyword)) syntax_error(lexer->getLocation(), "Expected 'fn' on function definition.\n");
	SrcLocation loc = lexer->getLocation();
	if(!lexer->getToken().isKind(TK_name)) syntax_error(lexer->getLocation(), "Expected name in function name definition.\n");
	const char* name = lexer->getToken().name;
	lexer->nextToken();
	ProcParams* params = parse_fn_params(lexer);

	TypeSpec* ret = nullptr;
	if (lexer->expect(TK_colon)) ret = parse_typespec(lexer);

	bool isExtern = lexer->expectKeyword(extern_keyword);
	ScopeStmt* scope = parse_scope(lexer);
	
	return new ProcDecl(loc, name, params, ret, scope, isExtern);
}
Decl* parse_decl(Lexer* lexer){	
	if(lexer->matchKeyword(fn_keyword)){
		return parse_fn(lexer);
	}
	else if (lexer->matchKeyword(struct_keyword) or lexer->matchKeyword(union_keyword)) {
		return parse_aggregate(lexer);
	}
	else if(lexer->matchKeyword(use_keyword)){
		return parse_use_module(lexer);
	}
	else {
		printf("Can't parse this declaration.\n");
		system("pause");
		exit(1);
	}
}
void parse_new_module(std::filesystem::path filePath) {
	Module newModule = Module(filePath.string().c_str());
	fatal("UwU ok \n");
}
#endif /*GPARSEDECL*/