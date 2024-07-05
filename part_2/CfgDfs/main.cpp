#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_map>

using namespace llvm;

namespace
{
    void Dfs(BasicBlock *currentBlock)
    {
        static std::unordered_map<BasicBlock *, bool> visited;
        visited[currentBlock] = true;
        currentBlock->print(outs());
        for (BasicBlock *bb : successors(currentBlock))
        {
            if (!visited[bb])
            {
                Dfs(bb);
            }
        }
    }
    struct CfgDfs : public PassInfoMixin<CfgDfs>
    {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM)
        {
            for (Function &F : M)
            {
                if (!F.isDeclaration())
                {
                    outs() << "----------------------------------------------------------------\n";
                    outs() << "Running DFS for the function " << F.getName() << "\n";
                    BasicBlock &entryBlock = F.getEntryBlock();
                    Dfs(&entryBlock);
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
                    MPM.addPass(CfgDfs());
                    return true;
                }
                return false;
            });
    };

    return {LLVM_PLUGIN_API_VERSION, "CfgDfs", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return getPassPluginInfo();
}