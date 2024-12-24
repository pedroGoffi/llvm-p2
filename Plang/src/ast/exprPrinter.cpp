#ifndef EXPRPRINTER
#define EXPRPRINTER
#include "../../header/expr.hpp"
#include "./printerCommon.cpp"
void print_expr(Expr* e, int i = 0) {
    ident(i);	
    if (!e) {
        printf("e == nullptr.\n");
        return;
    }
    
    
    
    if (IntExpr* intExpr = dynamic_cast<IntExpr*>(e)) {
        printf("(Integer %li)", intExpr->getI64());
    }

    else if (FloatExpr* floatExpr = dynamic_cast<FloatExpr*>(e)) {
        printf("(Integer %lf)", floatExpr->getFloat());
    }

    else if (StringExpr* stringExpr = dynamic_cast<StringExpr*>(e)) {
        printf("(String %s)", stringExpr->getString());
    }
    else if (NameExpr* nameExpr = dynamic_cast<NameExpr*>(e)) {
        printf("(Name %s)", nameExpr->getName());
    }
    else if (UnaryExpr* unaryExpr = dynamic_cast<UnaryExpr*>(e)) {
        printf("(Unary %s\n", unaryExpr->token.name);
        print_expr(unaryExpr->expr, i + 1);
        printf(")\n");
    }

    else if (BinaryExpr* binaryExpr = dynamic_cast<BinaryExpr*>(e)) {
        printf("(Binary %s\n", binaryExpr->token.name);
        print_expr(binaryExpr->lhs, i + 1);
        printf("\n");
        print_expr(binaryExpr->rhs, i + 1);
        printf(")\n");
    }

    else if (TernaryExpr* ternaryExpr = dynamic_cast<TernaryExpr*>(e)) {
        printf("(Ternary [IF;THEN;ELSE]\n");
        print_expr(ternaryExpr->condExpr, i + 1);        
        printf("\n");
        print_expr(ternaryExpr->thenExpr, i + 1);
        printf("\n");
        print_expr(ternaryExpr->elseExpr, i + 1);
        printf(")\n");
    }
    else if(InitExpr* initExpr = dynamic_cast<InitExpr*>(e)){
        printf("(INIT %s\n", initExpr->name);
        if (initExpr->typespec) {
            ident(i + 1);
            printf("TypeSpec = [\n");
            print_typespec(initExpr->typespec, i + 2);
            printf("\n");
            ident(i + 1);
            printf("]\n");
            
        }

        if (initExpr->init) {
            ident(i + 1);
            printf("Initialization = [\n");
            print_expr(initExpr->init, i + 2);
            printf("\n");
            ident(i + 1);
            printf("]\n");
        }        
        ident(i);
        printf(")\n");
    }
    else if (ModifyExpr* modifyExpr = dynamic_cast<ModifyExpr*>(e)) {
        printf("(MODIFY %s\n", modifyExpr->isInc? "++": "--");

        print_expr(modifyExpr->expr, i + 1);
        printf("\n");
        ident(i);
        printf(")\n");
    }
    else if(ArrayAccessExpr* arrayAccessExpr = dynamic_cast<ArrayAccessExpr*>(e)){
        printf("(ARR ACCESS\n");
        print_expr(arrayAccessExpr->base, i + 1);
        printf("\n");
        print_expr(arrayAccessExpr->index, i + 1);
        printf("\n");
        ident(i);
        printf(")\n");
    }
    else if (FieldExpr* fieldExpr = dynamic_cast<FieldExpr*>(e)) {
        printf("(FIELD %s\n", fieldExpr->field);
        print_expr(fieldExpr->base, i + 1);
        printf("\n");
        ident(i);
        printf(")\n");
    }
    else if (CallExpr* callExpr = dynamic_cast<CallExpr*>(e)) {
        printf("(CALL\n");

        print_expr(callExpr->base, i + 1);
        printf("\n");
        ident(i + 1);
        printf("With Args = [\n");
        for (Expr*& arg : callExpr->args) {
            print_expr(arg, i + 2);
            printf("\n");
        }
        ident(i + 1);
        printf("]\n");
        ident(i);
        printf(")\n");
    }
	else {
		printf("Expression not handled in print_expr.\n");
        system("pause");
		exit(1);
	}
}
#endif /*EXPRPRINTER*/