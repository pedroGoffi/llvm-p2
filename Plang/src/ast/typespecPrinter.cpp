#ifndef TYPESPECPRINTER
#define TYPESPECPRINTER
#include "../../header/typespec.hpp"
#include "./exprPrinter.cpp"
#include "./printerCommon.cpp"

void print_typespec(TypeSpec* ts, int i = 0){
	ident(i);

    if (NameTypeSpec* nameType = dynamic_cast<NameTypeSpec*>(ts)) {
        printf("(TYPESPEC NAME %s)\n", nameType->name);
    }
    else if (PtrTypeSpec* ptrType = dynamic_cast<PtrTypeSpec*>(ts)) {
        printf("(TYPESPEC POINTER\n");
        print_typespec(ptrType->base, i + 1);
        printf(")\n");
    }
    else if (FieldTypeSpec* fieldTypeSpec = dynamic_cast<FieldTypeSpec*>(ts)) {
        printf("(TYPESPEC FIELD %s\n", fieldTypeSpec->name);
        print_typespec(fieldTypeSpec->typespec, i + 1);
        printf("\n");
        if (fieldTypeSpec->defaultInit) {
            ident(i + 1);
            printf("Default Init = [\n");
            print_expr(fieldTypeSpec->defaultInit, i + 2);
            ident(i + 1);
            printf("]\n");
        }

    }
    else if (ArrayTypeSpec* arrayType = dynamic_cast<ArrayTypeSpec*>(ts)) {
        printf("(TYPESPEC ARRAY\n");
        if(arrayType->size){
            print_expr(arrayType->size, i + 1);
        }
        else {
            ident(i + 1);
            printf("Undeterminated size\n");
        }

        print_typespec(arrayType->base, i + 1);
        printf(")\n");
        
    }
    //else if (ProcTypeSpec* procType = dynamic_cast<ProcTypeSpec*>(ts)) {
    //    std::cout << "TypeSpec é do tipo ProcTypeSpec" << std::endl;
    //    // Use procType aqui
    //}
    //else if (ConstTypeSpec* constType = dynamic_cast<ConstTypeSpec*>(ts)) {
    //    std::cout << "TypeSpec é do tipo ConstTypeSpec" << std::endl;
    //    // Use constType aqui
    //}
    else if (TemplateTypeSpec* templateType = dynamic_cast<TemplateTypeSpec*>(ts)) {
        printf("(TYPESPEC TEMPLATE\n");
        print_typespec(templateType->base, i + 1);
        for (TypeSpec*& ts : templateType->args) {
            ident(i + 1);
            printf("With T = [\n");
            print_typespec(ts, i + 2);
            ident(i + 1);
            printf("]\n");
            
        }
        printf(")\n");     
    }
    else {
        printf("typespec not handled in print_typespec .\n");
        system("pause");
        exit(1);
    }
}

#endif /*TYPESPECPRINTER*/