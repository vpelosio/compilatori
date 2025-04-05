#include <vector>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace
{
  /**
   * Algebraic Identity
   * Casi gestiti
   * x + 0 = 0 + x = x
   * x * 1 = 1 * x = x
   */
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

          if (BinOp)
          {
            auto firstOperand = dyn_cast<ConstantInt>(BinOp->getOperand(0));
            auto secondOperand = dyn_cast<ConstantInt>(BinOp->getOperand(1));
            int operandToTake = -1;

            if (firstOperand &&
                ((BinOp->getOpcode() == Instruction::Add && firstOperand->getValue().isZero()) ||
                 (BinOp->getOpcode() == Instruction::Mul && firstOperand->getValue().isOne())))
            {
              operandToTake = 1;
            }
            else if (secondOperand &&
                     ((BinOp->getOpcode() == Instruction::Add && secondOperand->getValue().isZero()) ||
                      (BinOp->getOpcode() == Instruction::Mul && secondOperand->getValue().isOne())))
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

  /**
   * Strength reduction (avanzato)
   * Casi gestiti
   * Moltiplicazione per (2^x - 1): 15 * x = x * 15 = (x << 4) - x
   * Moltiplicazione per (2^x + 1): 17 * x = x * 17 = (x << 4) + x
   * Moltiplicazione per 2^x      : 16 * x = x * 16 = x << 4
   * Divisione per 2^x            : x / 8  = x >> 3
   */
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

          if (BinOp)
          {
            int constOpIndex = -1;
            if (dyn_cast<ConstantInt>(BinOp->getOperand(0)))
            {
              constOpIndex = 0;
            }
            else if (dyn_cast<ConstantInt>(BinOp->getOperand(1)))
            {
              constOpIndex = 1;
            }

            if (constOpIndex != -1)
            {
              auto constIntValue = dyn_cast<ConstantInt>(BinOp->getOperand(constOpIndex))->getValue();
              auto varValue = BinOp->getOperand(constOpIndex == 0 ? 1 : 0);
              IRBuilder<> builder(BinOp); // Nuova istruzione prima di BinOp
              bool overflow = false;

              if (BinOp->getOpcode() == Instruction::Mul)
              {
                llvm::APInt one(constIntValue.getBitWidth(), 1);

                if (constIntValue.isPowerOf2())
                {
                  auto shiftValue = builder.getInt32(constIntValue.logBase2());
                  auto shiftInst = builder.CreateShl(BinOp->getOperand(constOpIndex == 0 ? 1 : 0), shiftValue);
                  BinOp->replaceAllUsesWith(shiftInst);
                  instToRemove.push_back(BinOp);
                }
                /* Gestione overflow: 0xFFFFFFFF + 1 su registri a 32 andrebbe in overflow */
                else if (auto sumValue = constIntValue.uadd_ov(one, overflow); !overflow && sumValue.isPowerOf2())
                {
                  auto shiftValue = builder.getInt32(sumValue.logBase2());
                  auto shiftInst = builder.CreateShl(varValue, shiftValue);
                  auto subInst = builder.CreateSub(shiftInst, varValue);
                  BinOp->replaceAllUsesWith(subInst);
                  instToRemove.push_back(BinOp);
                }
                else if (auto subValue = constIntValue.usub_ov(one, overflow); !overflow && subValue.isPowerOf2())
                {
                  auto shiftValue = builder.getInt32(subValue.logBase2());
                  auto rShiftInst = builder.CreateShl(varValue, shiftValue);
                  auto addInst = builder.CreateAdd(rShiftInst, varValue);
                  BinOp->replaceAllUsesWith(addInst);
                  instToRemove.push_back(BinOp);
                }
              }
              else if (BinOp->getOpcode() == Instruction::UDiv) /* Gestiamo solo divisione unsigned per evitare problemi con lo shift a destra di numeri con il segno */
              {
                if (auto secondConstIntOp = dyn_cast<ConstantInt>(BinOp->getOperand(1)); secondConstIntOp && secondConstIntOp->getValue().isPowerOf2())
                {
                  auto shiftValue = builder.getInt32(secondConstIntOp->getValue().logBase2());
                  auto rShiftInst = builder.CreateLShr(BinOp->getOperand(0), shiftValue);
                  BinOp->replaceAllUsesWith(rShiftInst);
                  instToRemove.push_back(BinOp);
                }
              }
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
  }; // StrengthReduction

  struct MultiInstrOpt : PassInfoMixin<MultiInstrOpt>
  {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      std::vector<Instruction *> instToRemove;

      for (auto BB = F.begin(); BB != F.end(); ++BB)
      {
        for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst)
        {
          auto BinOp = dyn_cast<BinaryOperator>(Inst);

          if (BinOp)
          {
            int constOpIndex = -1;
            if (dyn_cast<ConstantInt>(BinOp->getOperand(0)))
            {
              constOpIndex = 0;
            }
            else if (dyn_cast<ConstantInt>(BinOp->getOperand(1)))
            {
              constOpIndex = 1;
            }

            if (constOpIndex != -1)
            {
              for (auto Iter = Inst->user_begin(); Iter != Inst->user_end(); ++Iter)
              {
                User *InstUser = *Iter;

                if (auto SecondOp = dyn_cast<BinaryOperator>(*Iter))
                {
                  if ((BinOp->getOpcode() == Instruction::Add && SecondOp->getOpcode() == Instruction::Sub) || (BinOp->getOpcode() == Instruction::Sub && SecondOp->getOpcode() == Instruction::Add))
                  {
                    int SecondConstOpIndex = -1;
                    if (dyn_cast<ConstantInt>(SecondOp->getOperand(0)))
                    {
                      SecondConstOpIndex = 0;
                    }
                    else if (dyn_cast<ConstantInt>(SecondOp->getOperand(1)))
                    {
                      SecondConstOpIndex = 1;
                    }

                    if (SecondConstOpIndex != -1)
                    {
                      auto constIntFirstOp = dyn_cast<ConstantInt>(BinOp->getOperand(constOpIndex))->getValue();
                      auto constIntSecondOp = dyn_cast<ConstantInt>(SecondOp->getOperand(SecondConstOpIndex))->getValue();

                      if (constIntFirstOp == constIntSecondOp)
                      {
                        SecondOp->replaceAllUsesWith(BinOp->getOperand(!constOpIndex));
                        instToRemove.push_back(SecondOp);
                      }
                    }
                  }
                  else if ((BinOp->getOpcode() == Instruction::Mul && SecondOp->getOpcode() == Instruction::UDiv) && dyn_cast<ConstantInt>(SecondOp->getOperand(1)))
                  {
                    auto constIntMulValue = dyn_cast<ConstantInt>(BinOp->getOperand(constOpIndex))->getValue();
                    auto constIntDivValue = dyn_cast<ConstantInt>(SecondOp->getOperand(1))->getValue();

                    if (constIntMulValue == constIntDivValue)
                    {
                      SecondOp->replaceAllUsesWith(BinOp->getOperand(!constOpIndex));
                      instToRemove.push_back(SecondOp);
                    }
                  }
                }
              }
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
                  else if (Name == "strength-reduction")
                  {
                    FPM.addPass(StrengthReduction());
                    return true;
                  }
                  else if (Name == "multinstr-opt")
                  {
                    FPM.addPass(MultiInstrOpt());
                    return true;
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
