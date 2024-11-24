#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace
{
  struct GlobalVarsPass : public PassInfoMixin<GlobalVarsPass>
  {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM)
    {
      auto globals = M.globals();
      for(auto itr = globals.begin();itr!=globals.end();itr++){
        StringRef varName = itr->getName();
        Type* ty = itr->getType();
        outs()<<"Variable Name: "<<varName<<"\n";
        outs()<<"Variable Type: ";
        ty->print(outs());
        outs()<<"\n";
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
            MPM.addPass(GlobalVarsPass());
            return true;
          }
          return false;
        });
  };

  return {LLVM_PLUGIN_API_VERSION, "GlobalVarsPass", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
  return getPassPluginInfo();
}