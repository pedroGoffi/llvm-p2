#define DEV_TEST true

#include "Plang/src/ast/type.cpp"
#include "Plang/src/parser/stmtParser.cpp"
#include "Plang/src/ast/stmtPrinter.cpp"
#include "Plang/src/ast/typespecPrinter.cpp"
#include "Plang/src/ast/typespec.cpp"
#include "Plang/src/ast/decl.cpp"
#include "Plang/src/parser/typespecParser.cpp"
#include "Plang/src/parser/declParser.cpp"
#include "Plang/src/ast/declPrinter.cpp"
#include "Plang/src/ast/module.cpp"
#include <iostream>
#include "Plang/src/llvm/compilerLLVM.cpp"

#define  DEFINE_FLAG(__FLAG__, __STR__) const char* __FLAG__##_FLAG = uniqueString.makeIntern(__STR__)
#define DEFINE_FLAG2(__F, __F_short, __F_full)				\
	DEFINE_FLAG(__F, __F_short);							\
	DEFINE_FLAG(__F##_FULL, __F_full)

DEFINE_FLAG2(OUT, "-o", "--output");
DEFINE_FLAG2(TARGET, "-t", "--target");
DEFINE_FLAG2(HELP, "-h", "--help");


const char* allowed_target_arch_str[] = {
	uniqueString.makeIntern("bin"),
	uniqueString.makeIntern("wasm")
};
bool check_allowed_target_arch_str(const char* str) {
	for (const char* arch : allowed_target_arch_str) {
		if (str == arch) return true;
	}
	return false;
}
struct {
	const char* executable			= nullptr;
	const char* out_p				= "a.out";
	const char* in_p				= nullptr;	
	const char* target_arch_str		= "bin";
	int debug_lvl					= Compiler_stats::DEBUG_NEC;

} cmd_flags;

void show_help() {	
	printf("USAGE: %s [options] file...\n", cmd_flags.executable);
	printf("OPTIONS :\n");
	printf("\t[-o / --output] : Specifies the output file for the compilation process\n");
	printf("\t[-t / --target] : Indicates the target architecture for compilation.\n");
	printf("\t[-h / --help] : Displays help information for using the command\n");	
}
const char* shift(int& argc, char** &argv){
	const char* flag = uniqueString.makeIntern(*argv);
	argc--;
	argv++;
	return flag;
}

int parse_cmd_flag(const char* flag, int& argc, char**& argv){
	if(flag == OUT_FLAG or flag == OUT_FULL_FLAG){
		if (argc == 0) {
			printf("[ERROR]: the flag -o expects filepath.\n");
			return EXIT_FAILURE;
		}
		cmd_flags.in_p = shift(argc, argv);
	}
	else if (flag == HELP_FLAG or flag == HELP_FULL_FLAG) {
		show_help();
		return EXIT_FAILURE; // to stop further steps on compilation
	}
	else if (flag == TARGET_FLAG or flag == TARGET_FULL_FLAG) {
		if (argc == 0) {
			printf("[ERROR]: the flag -t expects arch target to compilation.\n");
			return EXIT_FAILURE;
		}
		
		cmd_flags.target_arch_str = shift(argc, argv);
		if (!check_allowed_target_arch_str(cmd_flags.target_arch_str)) {
			printf("[ERROR]: the target %s is not allowed.\n", cmd_flags.target_arch_str);
			return EXIT_FAILURE;
		}
	}
	else {
		printf("[ERROR]: the flag %s is not identified.\n", flag);
		return EXIT_FAILURE;
	}
}
int audit_flags() {
	if (!cmd_flags.in_p) {
		printf("[ERROR]: no input files.");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
int parse_cmd_flags(int& argc, char**& argv){
	cmd_flags.executable = shift(argc, argv);
	while (argc) {
		const char* flag = shift(argc, argv);
		if (parse_cmd_flag(flag, argc, argv) == EXIT_FAILURE) break;
	}
	if(audit_flags() == EXIT_FAILURE) return EXIT_FAILURE;
	// parsed all argv then argc == 0
	return argc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char** argv){		
	parse_cmd_flags(argc, argv);
	printf("cmd_flags.in_p = %s\n", cmd_flags.in_p);
	return 1;
	const char* exampleFilePath = "../../../Plang/examples/example.pi";
	initInternKeywords();
	Module mod = Module(exampleFilePath);

	PContext ctx;
	//init_builtin(mod.lexer, &ctx);
	
	compile_module(&mod, &ctx);			
	if(ctx.errs.has_error()){
		while (ctx.errs.has_error()) {
			const char* err = ctx.errs.consume_error();
			printf("[ERROR]: %s\n", err);
		}

		system("pause");
		exit(1);
	}

	
	ctx.mod->print(llvm::errs(), nullptr);
	printf("[SUCESS]: Pietra executable is mounted.\n");
	
	printf("[NOTE]: Starting compilation to executable...\n");
	cstats.IR_out_kind = Compiler_stats::TO_WASM;
	if (compile(ctx.mod) == EXIT_FAILURE) {
		cstats.trace_errors();
		printf("[ERROR]: Failed to compile to executable.\n");
		system("pause");
		return EXIT_FAILURE;
	}
	
			
	printf("[SUCESS]: Pietra executable is mounted.\n");
	printf("[DEV]: Running executable.\n");
	system("pietra_out.exe");
	system("pause");
	
	return EXIT_SUCCESS;
	
}
