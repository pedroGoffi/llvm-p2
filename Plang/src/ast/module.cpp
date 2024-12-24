#ifndef MODULE_AST
#define MODULE_AST
#include "../../header/module.hpp"
#include <filesystem>


namespace fs = std::filesystem;



const bool Module::has_error() const {
	return this->error != nullptr;
}

Module::Module(const char* filePath){
	this->error = nullptr;
	this->path = fs::absolute(filePath);
	if (!this->path.has_root_directory()) {} // error
	this->fileName = this->path.filename();
	if(!this->fileName.has_filename()){} // error
	int len = 0;
	const char* codeString = read_file(this->path, &len);
	if (!codeString) {
		this->error = strf("Failed to read the file: '%s'\n", filePath);
		return;
	}
	const char* codeStringEnd = codeString + len;
	if (*codeStringEnd != '\0') {
		this->error = "Failed to calculate the end of the file: '%s'\n";
		return;
	}
	this->lexer = new Lexer(codeString, codeStringEnd);

	if (!this->parseAst()) 
		fatal("Error while parsing the ast.\n");
}
bool Module::parseAst() {
	this->lexer->setContextPath(this->path);
	while (!this->lexer->isEof()) {
		Decl* decl = parse_decl(this->lexer);
		if (!decl) return false;
		if(0){}
		else {
			this->ast.push_back(decl);
		}
	}
	return true;
}
std::vector<Decl*> Module::getAst() { return this->ast; }
fs::path Module::getParentPath() { return this->path.parent_path();  }
fs::path Module::getPath() { return this->path; }
#endif /*MODULE_AST*/