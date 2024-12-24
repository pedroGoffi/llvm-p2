#ifndef DECL_PRINTER
#define DECL_PRINTER
#include "./printerCommon.cpp"
#include "../../header/decl.hpp"

void print_decl(Decl* decl, int i = 0) {
	ident(i);

	if(ProcDecl* procDecl = dynamic_cast<ProcDecl*>(decl)){
		printf("(PROCEDURE %s\n", procDecl->name);
		
		if (procDecl->ret) {
			ident(i + 1);
			printf("RETURNS = [\n");
			print_typespec(procDecl->ret, i + 2);
			ident(i + 1);
			printf("]\n");
		}
		for (FieldTypeSpec*& param : procDecl->params->params) {
			print_typespec(param, i + 1);
			printf("\n");
		}
		
		if (procDecl->scope) print_stmt(procDecl->scope, i + 1);
		ident(i);
		printf(")\n");
	}
	else if (AggregateDecl* aggregateDecl = dynamic_cast<AggregateDecl*>(decl) ){
		printf("(AGGREGATE %s as %s\n", aggregateDecl->name, aggregateDecl->isStruct ? "struct": "union");
		for(FieldTypeSpec* field: aggregateDecl->fields){
			print_typespec(field, i + 1);
		
		}
		
		ident(i);
		printf(")\n");
	}
	else if (UseModuleDecl* useModuleDecl = dynamic_cast<UseModuleDecl*>(decl)) {
		printf("(USE MODULE %s)\n", useModuleDecl->name);
	}
	else {
		printf("Decl not handled in print_decl.\n");
		system("pause");
		exit(1);
	}
}
#endif /*DECL_PRINTER*/