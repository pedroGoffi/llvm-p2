#ifndef PIETRA_EXPR_HEADER
#define PIETRA_EXPR_HEADER
#include <cstdint>
#include <string>
#include <vector>
#include "./lexer.hpp"
#include "./typespec.hpp"
#include "./srcLocation.hpp"
#include "./stmt.hpp"

class Expr {
public:
    virtual ~Expr() = default;
};


class ValueExpr: public Expr {
private:
    void init();
public:
    enum {
        VAL_none,
        VAL_string,
        VAL_float,
        VAL_integer
    } kind;

    const char* string = 0;
    double      f64 = 0;
    long        i64 = 0;
    SrcLocation loc;
public:
    ValueExpr() = default;
    ValueExpr(const char* string, SrcLocation pos);
    ValueExpr(double f64, SrcLocation pos);
    ValueExpr(long i64, SrcLocation pos);    

    virtual const char*& getString() { assert(0 and "[ValueExpr ERROR]: not for getString");  exit(1); }
    virtual double&      getFloat() { assert(0 and "[ValueExpr ERROR]: not for getFloat");  exit(1); }
    virtual long&        getI64() { assert(0 and "[ValueExpr ERROR]: not for getI64");  exit(1); }
    virtual const char*& getName() { assert(0 and "[ValueExpr ERROR]: not for getName");  exit(1); }
};

class IntExpr : public ValueExpr {
public:    
    IntExpr(long i64, SrcLocation loc);
    
    long& getI64() override;
};

class FloatExpr : public ValueExpr {
public:    
    FloatExpr(double f64, SrcLocation loc);

    double& getFloat() override;
};
class InitExpr: public Expr {
public:
    SrcLocation loc;
    const char* name;
    TypeSpec* typespec;
    Expr* init;
    InitExpr(SrcLocation loc, const char* name, TypeSpec* typespec, Expr* init);
};
class StringExpr : public ValueExpr {
public:
    StringExpr(const char* str, SrcLocation loc);
    const char*& getString() override;
};
class NameExpr: public ValueExpr {
public:
    NameExpr(SrcLocation loc, const char* name);
    const char*& getName() override;
};
class CallExpr: public Expr {
public:
    SrcLocation loc;
    Expr* base;
    std::vector<Expr*> args;
    CallExpr(SrcLocation loc, Expr* base, std::vector<Expr*> args);
};
class FieldExpr : public Expr {
public:
    SrcLocation loc;
    Expr* base;
    const char* field;
    bool auto_derref;
    FieldExpr(SrcLocation loc, Expr* base, const char* field, bool auto_derref);
};

class ArrayAccessExpr : public Expr {
public:
    SrcLocation loc;
    Expr* base;
    Expr* index;
    ArrayAccessExpr(SrcLocation loc, Expr* base, Expr* index);

};
class ModifyExpr : public Expr {
public:
    SrcLocation loc;
    Expr* expr;
    bool isInc;
    ModifyExpr(SrcLocation loc, Expr* expr, bool isInc);

};
class UnaryExpr : public Expr {
public:
    SrcLocation loc;
    Token token;    
    Expr* expr;
    UnaryExpr(SrcLocation loc, Token token, Expr* expr);
};

class BinaryExpr : public Expr {
public:
    SrcLocation loc;
    Token token;
    Expr* lhs;
    Expr* rhs;

    BinaryExpr(const SrcLocation loc, const Token token, Expr* lhs, Expr* rhs);
};

class TernaryExpr: public Expr {
public:
    SrcLocation loc;
    Expr* condExpr;
    Expr* thenExpr;
    Expr* elseExpr;

    TernaryExpr(const SrcLocation loc, Expr* condExpr, Expr* thenExpr, Expr* elseExpr);
};

#endif /*PIETRA_EXPR_HEADER*/