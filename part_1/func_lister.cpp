#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace
{
  // All LLVM passes must inherit from the CRTP mixin PassInfoMixin
  struct FunctionListerPass : public PassInfoMixin<FunctionListerPass>
  {
    // A pass should have a run method
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM)
    {
      // outs() returns a reference to a raw_fd_ostream for standard output.
      outs() << F.getName() << '\n';
      return PreservedAnalyses::all();
    }
  };

}

PassPluginLibraryInfo getPassPluginInfo()
{
  const auto callback = [](PassBuilder &PB)
  {
    PB.registerPipelineStartEPCallback(
        [&](ModulePassManager &MPM, auto)
        {
          MPM.addPass(createModuleToFunctionPassAdaptor(FunctionListerPass()));
          return true;
        });
  };

  return {LLVM_PLUGIN_API_VERSION, "name", "0.0.1", callback};
};

/* When a plugin is loaded by the driver, it will call this entry point to
obtain information about this plugin and about how to register its passes.
*/
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
  return getPassPluginInfo();
}