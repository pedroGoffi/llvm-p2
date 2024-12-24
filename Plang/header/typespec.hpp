#ifndef TYPESPEC_HEADER
#define TYPESPEC_HEADER
#include "./lexer.hpp"
#include "./expr.hpp"
extern class Expr;
class TypeSpec {
protected:
    bool isRef;
    bool isMut;

public:    
    
    virtual void setRef(bool isRef);
    virtual void setMut(bool isMut);
};

class NameTypeSpec : public TypeSpec {
public:
    SrcLocation loc;
    const char* name;
    NameTypeSpec(SrcLocation loc, const char* name);
};


class PtrTypeSpec : public TypeSpec {
public:
    SrcLocation loc;
    TypeSpec* base;
    PtrTypeSpec(SrcLocation loc, TypeSpec* base);
};
class ArrayTypeSpec : public TypeSpec {
public:
    SrcLocation loc;
    Expr* size;
    TypeSpec*   base;
    ArrayTypeSpec(SrcLocation loc, Expr* size, TypeSpec* base);
};
class ProcTypeSpec: public TypeSpec {
public:
    SrcLocation loc;
    std::vector<TypeSpec*> args;
    TypeSpec* ret;
    bool isVararg;
    size_t varargPos;
    ProcTypeSpec(SrcLocation loc, const std::vector<TypeSpec*> args, TypeSpec* ret, bool isVararg, const size_t varargPos);
};

class ConstTypeSpec : public TypeSpec {    
public:
    SrcLocation loc;   
    TypeSpec* base;
    bool isRef;
    ConstTypeSpec(SrcLocation loc, TypeSpec* base, bool isRef);
};
class TemplateTypeSpec : public TypeSpec {
public:
    SrcLocation loc;
    TypeSpec* base;
    std::vector<TypeSpec*> args;
    TemplateTypeSpec(SrcLocation loc, TypeSpec* base, std::vector<TypeSpec*> args);
};
class FieldTypeSpec : public TypeSpec {
public:
    SrcLocation loc;
    const char* name;
    TypeSpec* typespec;
    Expr* defaultInit;
    FieldTypeSpec(SrcLocation loc, const char* name, TypeSpec* typespec, Expr* defaultInit);
};
#endif /*TYPESPEC_HEADER*/