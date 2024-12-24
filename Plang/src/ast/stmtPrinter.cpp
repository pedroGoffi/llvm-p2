#ifndef STMTPRINTER
#define STMTPRINTER
#include "./exprPrinter.cpp"
#include "../../header/stmt.hpp"
#include "./printerCommon.cpp"

void print_stmt(Stmt* stmt, int i = 0) {
    ident(i);
    
    if (ExprStmt* exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        printf("(STMT EXPR\n");
        print_expr(exprStmt->expr, i + 1);
        printf(")\n");
    }
    else if (ReturnStmt* ret = dynamic_cast<ReturnStmt*>(stmt)) {
        printf("(STMT RETURN\n");
        print_expr(ret->expr, i + 1);
        printf(")\n");
    }
    else if (ScopeStmt* scopeStmt = dynamic_cast<ScopeStmt*>(stmt)) {
        printf("(Scope\n");
        for (Stmt*& node : scopeStmt->nodes) {
            print_stmt(node, i + 1);
        }
        ident(i);
        printf(")\n");
    }
    //else if (IfStmt* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
    //    
    //}
    //else if (SwitchStmt* switchStmt = dynamic_cast<SwitchStmt*>(stmt)) {
    //    
    //}
    else if (ForStmt* forStmt = dynamic_cast<ForStmt*>(stmt)) {
        printf("(STMT FOR\n");
        if (forStmt->init) {
            print_expr(forStmt->init, i + 1);            
            printf("\n");
        }
        if (forStmt->cond) {
            print_expr(forStmt->cond, i + 1);
            printf("\n");
        }
        if (forStmt->inc) {
            print_expr(forStmt->inc, i + 1);
            printf("\n");
        }
        if (forStmt->scope) {
            print_stmt(forStmt->scope, i + 1);
            printf("\n");
        }

        
    }
    else if (WhileStmt* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        printf("(WHILE\n");
        if (whileStmt->cond) {
            print_expr(whileStmt->cond, i + 1);
            printf("\n");
        }

        if (whileStmt->scope) {
            print_stmt(whileStmt->scope, i + 1);
            printf("\n");
        }
        ident(i);
        printf(")\n");
    }
    else if (ComptimeStmt* comptimeStmt = dynamic_cast<ComptimeStmt*>(stmt)) {
        printf("(COMPTIME STMT:\n");
        print_stmt(comptimeStmt->scope, i + 1);
        ident(i);
        printf(")\n");
       
    }
    //else if (ReturnStmt* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
    //    
    //}
    else {
        printf("Statement not handled in print_stmt.\n");
        system("pause");
        exit(1);
    }
}
#endif /*STMTPRINTER*/