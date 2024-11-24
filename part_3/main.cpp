#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/raw_ostream.h>

struct Instruction {
    std::string name;
    int64_t val1;
    int64_t val2;

    Instruction(const std::string &name, int64_t val1, int64_t val2) 
        : name(name), val1(val1), val2(val2) {}
};

void fatal_error(const std::string &message) {
    std::cerr << message << std::endl;
    std::exit(1);
}

std::vector<std::unique_ptr<Instruction>> GetInstructions(const std::string &file_name) {
    std::ifstream ifile(file_name);
    std::string instruction_line;
    std::vector<std::unique_ptr<Instruction>> instructions;

    if (!ifile.is_open()) {
        fatal_error("Failed to open file: " + file_name);
    }

    while (std::getline(ifile, instruction_line)) {
        std::istringstream stream(instruction_line);
        std::string instruction_type;
        int64_t val1, val2;
        char comma;

        if (stream >> instruction_type >> val1 >> comma >> val2) {
            instructions.push_back(std::make_unique<Instruction>(instruction_type, val1, val2));
        } else {
            fatal_error("Invalid instruction format: " + instruction_line);
        }
    }
    return instructions;
}

void AddFunctionsToIR(llvm::LLVMContext &ctx, llvm::Module *module, const std::string &function_name) {
    auto int64_type = llvm::Type::getInt64Ty(ctx);
    std::vector<llvm::Type *> params(2, int64_type);
    llvm::IRBuilder<> ir_builder(ctx);

    llvm::FunctionType *function_type = llvm::FunctionType::get(int64_type, params, false);
    llvm::Function *func = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, function_name, module);

    llvm::BasicBlock *basic_block = llvm::BasicBlock::Create(ctx, "entry", func);
    ir_builder.SetInsertPoint(basic_block);

    auto args = func->args();
    auto arg_iter = args.begin();
    llvm::Argument *arg1 = arg_iter++;
    llvm::Argument *arg2 = arg_iter;

    llvm::Value *result = nullptr;

    if (function_name == "add") {
        result = ir_builder.CreateAdd(arg1, arg2);
    } else if (function_name == "sub") {
        result = ir_builder.CreateSub(arg1, arg2);
    } else if (function_name == "mul") {
        result = ir_builder.CreateMul(arg1, arg2);
    } else if (function_name == "xor") {
        result = ir_builder.CreateXor(arg1, arg2);
    } else {
        fatal_error("Invalid function name: " + function_name);
    }

    ir_builder.CreateRet(result);
 }

llvm::orc::ExecutorAddr GetExecutorAddr(llvm::orc::LLJIT &jit, const std::string &function_name) {
    auto sym = jit.lookup(function_name).get();
    if (!sym) {
        fatal_error("Function not found in JIT: " + function_name);
    }
    return sym;
}

int main() {
    llvm::LLVMContext ctx;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    auto module = std::make_unique<llvm::Module>("neko_module", ctx);

    AddFunctionsToIR(ctx, module.get(), "add");
    AddFunctionsToIR(ctx, module.get(), "sub");
    AddFunctionsToIR(ctx, module.get(), "mul");
    AddFunctionsToIR(ctx, module.get(), "xor");

    auto jit_builder = llvm::orc::LLJITBuilder();
    auto jit = jit_builder.create();
    if (!jit) {
        fatal_error("Failed to create JIT: " + llvm::toString(jit.takeError()));
    }

    if (auto err = jit->get()->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::make_unique<llvm::LLVMContext>()))) {
        fatal_error("Failed to add IR module for JIT compilation: " + llvm::toString(std::move(err)));
    }

    auto instructions = GetInstructions("code.txt");
    std::unordered_map<std::string, llvm::orc::ExecutorAddr> fn_symbols;

    for (const auto &instruction : instructions) {
        if (fn_symbols.find(instruction->name) == fn_symbols.end()) {
            fn_symbols[instruction->name] = GetExecutorAddr(*jit->get(), instruction->name);
        }

        auto *fn = reinterpret_cast<int64_t (*)(int64_t, int64_t)>(fn_symbols[instruction->name].getValue());
        int64_t value = fn(instruction->val1, instruction->val2);
        std::cout << value << std::endl;
    }

    return 0;
}
