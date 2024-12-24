#ifndef LLVM_COMPILER
#define LLVM_COMPILER
#include <llvm/Support/FileSystem.h>
#include <iostream>
#include <chrono>
#include "./declarations.cpp"
#include "./declCompiler.cpp"
#include "./typeCompiler.cpp"
const char* cmd_llc_compilation_table[] = {
    /*TO_NONE   0 */ "llc -filetype=obj %s -o %s",
    /*TO_WASM   1 */ "llc -march=wasm32 -filetype=obj %s -o %s",
    /*TO_BIN    2 */ "llc -filetype=obj %s -o %s",
};

const char* get_llc_command(int id) {
    if (id < 0 or id > sizeof(cmd_llc_compilation_table)) return nullptr;

    return cmd_llc_compilation_table[id];
}
struct Compiler_stats {    
    enum {
        DEBUG_NONE,
        DEBUG_NEC,
        DEBUG_ALL,
        DEBUG_DEV
    } debugLvl;
    
    enum {
        TO_NONE,
        TO_WASM,
        TO_BIN
    } IR_out_kind;

    std::vector<const char*> errs;
    void addErr(const char* err) {
        this->errs.push_back(err);
    }

    TemporaryFile tmpIR;
    TemporaryFile tmpMiddle;

    std::chrono::steady_clock::time_point t_start;
    std::chrono::steady_clock::time_point t_end;

    void begin_clock() {
        this->t_start = this->fetch_clock();
    }
    void end_clock() {
        this->t_end = this->fetch_clock();
    }
    double clock_duration() {
        return std::chrono::duration<double, std::milli>(this->t_end - this->t_start).count();
    }
    void trace_errors() {
        printf("[ERRORS]:\n");
        for (const char*& err : this->errs) {
            printf("\t[ERROR]: %s\n", err);
        }
    }
private: std::chrono::steady_clock::time_point fetch_clock() {
        return std::chrono::high_resolution_clock::now();
    }
           
} cstats;


inline int CMD(const char* fmt, ...) {    
    va_list args;
    va_start(args, fmt);
    size_t n = 1 + vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* command = (char*)xmalloc(n);
    va_start(args, fmt);
    vsnprintf(command, n, fmt, args);
    va_end(args);    

    if(cstats.debugLvl <= Compiler_stats::DEBUG_ALL){
        printf("[CMD]: %s\n", command);
    }
    cstats.begin_clock();
    int result = system(command);
    cstats.end_clock();
    
    if (cstats.debugLvl <= Compiler_stats::DEBUG_DEV) {
        printf("\t[*]: Command took %ll", cstats.clock_duration());
    }
    free(command);
    return result;
}

int compile_module_to_LLVM_IR(llvm::Module* mod, const char* IR_filename){
    assert(mod);
    // Verify the module
    if (verifyModule(*mod, &llvm::errs())) {
        cstats.addErr(strf("Module verification failed!\n"));
        return EXIT_FAILURE;
    }

    // Save the module to a .bc file
    std::error_code EC;
    
    cstats.tmpIR = TemporaryFile::create(IR_filename);
    llvm::raw_fd_ostream OutFile(IR_filename, EC, llvm::sys::fs::OF_None);
    if (EC) {
        cstats.addErr(strf("Error opening file: %s\n", EC.message().c_str()));
        return EXIT_FAILURE;
    }
    mod->print(OutFile, nullptr);    
    return EXIT_SUCCESS;
}
int prepare_compilation(llvm::Module* mod, const char* out_fp) {
    if (compile_module_to_LLVM_IR(mod, out_fp) == EXIT_FAILURE) {
        cstats.addErr(strf("failed to compile to llvm/IR."));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int compile_to_target() {
    const char* llc_cmd = get_llc_command(cstats.IR_out_kind);
    int exit_code = EXIT_FAILURE;

    switch (cstats.IR_out_kind) {
    case Compiler_stats::TO_WASM: {
        exit_code = CMD(llc_cmd,
            cstats.tmpIR.getName(),
            strf("%s%s", cstats.tmpIR.getName(), ".wasm"));
        break;
    }

    // Default -> binary
    case Compiler_stats::TO_BIN:
    case Compiler_stats::TO_NONE:
    default:
        cstats.tmpMiddle = TemporaryFile::create(strf("%s.TMP.o", cstats.tmpIR.getName()));
        CMD(llc_cmd, cstats.tmpIR.getName(), cstats.tmpMiddle.getName());
        // Link .o to .exe using `clang`
        exit_code = CMD("clang -o pietra_out.exe %s", cstats.tmpMiddle.getName());
        break;
    }

    return exit_code;

}
int compile(llvm::Module* mod, const char* out_fp = "a.out") {
    if (prepare_compilation(mod, out_fp) == EXIT_FAILURE) return EXIT_FAILURE;    
    // Convert .bc to .o using `llvm-as` and `llc` commands
    if (!cstats.tmpIR.isSet) {
        cstats.addErr(strf("INTERNAL ERROR: Couldn't find the temporary file for llvm IR"));
        return EXIT_FAILURE;
    }
    return compile_to_target();
}

#endif /*LLVM_COMPILER*/