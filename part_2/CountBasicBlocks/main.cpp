#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace
{
    struct CountBasicBlocks : public PassInfoMixin<CountBasicBlocks>
    {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM)
        {
            for (Function &F : M)
            {
                if (!F.isDeclaration())
                {
                    int nBlocks = 0;
                    outs() << "----------------------------------------------------------------------\n";
                    outs() << "Counting and printing basic blocks in the function " << F.getName() << "\n";
                    for (BasicBlock &BB : F)
                    {
                        BB.print(outs());
                        outs() << "\n";
                        nBlocks++;
                    }
                    outs() << "Number of basic blocks: " << nBlocks << "\n";
                }
            }
            return PreservedAnalyses::all();
        }

        static bool isRequired()
        {
            return true;
        }
    };

}

PassPluginLibraryInfo getPassPluginInfo()
{
    const auto callback = [](PassBuilder &PB)
    {
        PB.registerPipelineParsingCallback(
            [&](StringRef name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>)
            {
                if (name == "run-pass")
                {
                    MPM.addPass(CountBasicBlocks());
                    return true;
                }
                return false;
            });
    };

    return {LLVM_PLUGIN_API_VERSION, "CountBasicBlocks", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return getPassPluginInfo();
}