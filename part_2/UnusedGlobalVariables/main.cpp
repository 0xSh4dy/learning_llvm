#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace
{
  struct UnusedGlobalVarsPass : public PassInfoMixin<UnusedGlobalVarsPass>
  {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM)
    {
      auto globalVars = M.globals();
      for(GlobalVariable &gvar: globalVars){
        if(gvar.use_empty()){
            outs()<<"Unused global variable: "<<gvar.getName()<<"\n";
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
            MPM.addPass(UnusedGlobalVarsPass());
            return true;
          }
          return false;
        });
  };

  return {LLVM_PLUGIN_API_VERSION, "UnusedGlobalVarsPass", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
  return getPassPluginInfo();
}