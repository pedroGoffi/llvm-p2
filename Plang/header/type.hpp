#ifndef PIETRATYPES_HEADER
#define PIETRATYPES_HEADER
#include <vector>

class PType {
private:
	virtual void __p__() {}	
	int size;
public:	
	const char* name;
	void setSize(int newSize);
	int& getSize();
};

class VoidType : public PType {
public:
	VoidType();
};
class IntType: public PType {
public:	
	IntType(const char* name, int size);
};
class FloatType : public PType {
public:
	FloatType(const char* name, int size);
};
class AggregateType : public PType {
public:
	const char* name;
	std::vector<PType*> fields;
	AggregateType(const char* name, std::vector<PType*> fields);
};
class PtrType : public PType {
public:
	PType* base;
	PtrType(PType* base);
};
class FieldType : public PType {
public:	
	PType* type;
	FieldType(const char* name, PType* type);
};
class ProcType : public PType {
public:
	std::vector<PType*> params;
	PType* ret;	
	bool is_vararg;
	size_t vararg_pos;
	ProcType(std::vector<PType*> params, PType* ret, bool is_vararg, size_t vararg_pos);
};
void print_type(PType* type);
#endif /*PIETRATYPES_HEADER*/