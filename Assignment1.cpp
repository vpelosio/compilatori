#include <vector>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace
{
  struct AlgebraicIdentity : PassInfoMixin<AlgebraicIdentity>
  {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      std::vector<Instruction *> instToRemove;

      for (auto BB = F.begin(); BB != F.end(); ++BB)
      {
        for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst)
        {
          auto BinOp = dyn_cast<BinaryOperator>(Inst);

          if (BinOp && BinOp->getOpcode() == Instruction::Add)
          {
            auto firstOperand = dyn_cast<ConstantInt>(BinOp->getOperand(0));
            auto secondOperand = dyn_cast<ConstantInt>(BinOp->getOperand(1));
            int operandToTake = -1;

            if (firstOperand && firstOperand->getValue() == 0)
            {
              operandToTake = 1;
            }
            else if (secondOperand && secondOperand->getValue() == 0)
            {
              operandToTake = 0;
            }

            if (operandToTake != -1)
            {
              BinOp->replaceAllUsesWith(BinOp->getOperand(operandToTake));
              instToRemove.push_back(BinOp);
            }
          }
        }
      }
      for (auto inst : instToRemove)
      {
        inst->eraseFromParent();
      }
      return PreservedAnalyses::all();
    }
    static bool isRequired()
    {
      return true;
    }
  }; // AlgebraicIdentity

  struct StrengthReduction : PassInfoMixin<StrengthReduction>
  {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      std::vector<Instruction *> instToRemove;

      for (auto BB = F.begin(); BB != F.end(); ++BB)
      {
        for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst)
        {
          auto BinOp = dyn_cast<BinaryOperator>(Inst);

          if(!BinOp) 
             break; // skip if it isn't a binary operator

          if (BinOp->getOpcode() == Instruction::Mul)
          {

          }
          else if(BinOp->getOpcode() == Instruction::SDiv || BinOp->getOpcode() == Instruction::UDiv)
          {
            /* Handle signed and unsigned integer division. Floating point division not handled. */
          }
        }
      }
      for (auto inst : instToRemove)
      {
        inst->eraseFromParent();
      }
      return PreservedAnalyses::all();
    }
    static bool isRequired()
    {
      return true;
    }
  }; // StrengthReduction

  struct MultiInstrOpt : PassInfoMixin<MultiInstrOpt>
  {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      return PreservedAnalyses::all();
    }
      
  }; // MultiInstrOpt
}

llvm::PassPluginLibraryInfo assignment1PluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "Assignment1", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "algebraic-identity")
                  {
                    FPM.addPass(AlgebraicIdentity());
                    return true;
                  }
                  else if(Name == "strength-reduction")
                  {
                    FPM.addPass(StrengthReduction());
                  }
                  else if(Name == "multinstr-opt")
                  {
                    FPM.addPass(MultiInstrOpt());
                  }
                  return false;
                });
          }};

}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize TestPass when added to the pass pipeline on the
// command line, i.e. via '-passes=test-pass'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
  return assignment1PluginInfo();
}
