#ifndef PIETRA_MODULE_HEADER
#define PIETRA_MODULE_HEADER
#include <cstdint>
#include <string>
#include <vector>
#include "./lexer.hpp"
#include "./typespec.hpp"
#include "./srcLocation.hpp"
#include "./stmt.hpp"
#include <filesystem>

class Module {
private:	
	std::filesystem::path	path = {};
	std::filesystem::path	fileName = {};
	std::vector<Decl*>		ast = {};


public:
	Lexer* lexer;
	const char* error;
private:
	std::filesystem::path getParentPath();
	bool parseAst();
public:	
	Module(const char* filePath);
	
	const bool has_error() const;
	
	std::vector<Decl*> getAst();
	std::filesystem::path getPath();
};

#endif /*PIETRA_MODULE_HEADER*/