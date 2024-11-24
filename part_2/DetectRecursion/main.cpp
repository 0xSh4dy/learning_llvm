#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace
{
  struct DetectRecursion : public PassInfoMixin<DetectRecursion>
  {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM)
    {
      for (Function &F : M)
      {
        bool recursionDetected = false;

        for (BasicBlock &BB : F)
        {
          for (Instruction &instr : BB)
          {
            if (instr.getOpcode() == Instruction::Call)
            {
              CallInst *callInstr = dyn_cast<CallInst>(&instr);
              if (callInstr)
              {
                Function *calledFunction = callInstr->getCalledFunction();
                if (calledFunction && calledFunction->getName() == F.getName())
                {
                  outs() << "Recursion detected: " << calledFunction->getName() << "\n";
                  recursionDetected = true;
                  break;
                }
              }
            }
          }
          if (recursionDetected)
            break;
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
            MPM.addPass(DetectRecursion());
            return true;
          }
          return false;
        });
  };

  return {LLVM_PLUGIN_API_VERSION, "DetectRecursion", LLVM_VERSION_STRING, callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
  return getPassPluginInfo();
}