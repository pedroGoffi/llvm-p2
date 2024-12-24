#ifndef PIETRA_TYPES
#define PIETRA_TYPES
#include "../../header/type.hpp"
#include "../lexer/lexer.cpp"
void PType::setSize(int newSize) { this->size = newSize; }
int& PType::getSize() { return this->size; }

VoidType::VoidType() {
	this->name = uniqueString.makeIntern("void");
	this->setSize(0);
}
IntType::IntType(const char* name, int size){
	
	this->name = uniqueString.makeIntern(name);
	this->setSize(size);
}
FloatType::FloatType(const char* name, int size){
	this->name = uniqueString.makeIntern(name);
	this->setSize(size);
}
AggregateType::AggregateType(const char* name, std::vector<PType*> fields) {
	this->name = uniqueString.makeIntern(name);
	this->fields = fields;
	if (this->fields.size() > 0) {
		this->setSize(this->fields.size() * this->fields.at(0)->getSize());
	}
	else {
		this->setSize(0);
	}
}
PtrType::PtrType(PType* base) {
	this->setSize(sizeof(void*));
	this->base = base;
}

FieldType::FieldType(const char* name, PType* type) {
	
	this->name = name;
	this->type = type;
}
ProcType::ProcType(std::vector<PType*> params, PType* ret, bool is_vararg, size_t vararg_pos) {
	this->params = params;
	this->ret = ret;
	this->is_vararg = is_vararg;
	this->vararg_pos = vararg_pos;
}
void print_type(PType* type) {
	if (!type) {
		printf("TYPE == nullptr\n");
	}
	else if (auto vt = dynamic_cast<VoidType*>(type)) {
		printf("VoidType\n");
	}
	else if (auto it = dynamic_cast<IntType*>(type)) {
		printf("IntType\n");
	}
	else if (auto ft = dynamic_cast<FloatType*>(type)) {
		printf("FloatType\n");
	}
	else if (auto at = dynamic_cast<AggregateType*>(type)) {
		printf("AggregateType\n");
	}
	else if (auto pt = dynamic_cast<PtrType*>(type)) {
		printf("PtrType\n");
		if (pt->base) {
			print_type(pt->base);  // Recursive call for PtrType
		}
	}
	else if (auto ft = dynamic_cast<FieldType*>(type)) {
		printf("FieldType\n");
	}
	else if (auto proc = dynamic_cast<ProcType*>(type)) {
		printf("Procedure [\n");
		for (PType* param : proc->params) {
			printf("\n"); 
			print_type(param);
		}		
		printf("]\n");
	}
	else {
		printf("Unknown type\n");
	}
}

#endif/*PIETRA_TYPES*/