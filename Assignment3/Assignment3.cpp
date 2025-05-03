#include <unordered_set>
#include <algorithm>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Dominators.h"

using namespace llvm;

namespace
{
  struct LICM : PassInfoMixin<LICM>
  {
    PreservedAnalyses run(Function &f, FunctionAnalysisManager &fam)
    {
      auto &li = fam.getResult<LoopAnalysis>(f);
      auto &dt = fam.getResult<DominatorTreeAnalysis>(f);
      
      if(!li.empty()) // At least one loop exists
      {
        for(auto *loop: li)
        {
          if(loop->isLoopSimplifyForm()) // The loop is in normal form
          {
            runLICM(loop, &li, &dt);
          }
        }
      }
      
      return PreservedAnalyses::all();
    }

    bool dominateAllLoopExits(llvm::DominatorTree *dt, llvm::Loop *loop, llvm::BasicBlock *bb)
    {
      llvm::SmallVector<llvm::BasicBlock*> exitBasicBlocks;
      loop->getExitBlocks(exitBasicBlocks);

      return std::all_of(exitBasicBlocks.begin(), exitBasicBlocks.end(), [dt, bb](llvm::BasicBlock *exitBB){ return dt->dominates(bb, exitBB);});
    }

    bool isVarDeadAtLoopExit(llvm::Instruction *instr, llvm::Loop *loop)
    {
      /* Variable dead at loop exit means there are no uses outside the loop */
      /* Eg: Declared inside the loop and no longer accessible */

      for(auto *user: instr->users())
      {
        auto *userInstr = dyn_cast<llvm::Instruction>(user);
        if(userInstr && !loop->contains(userInstr))
          return false;
      }
      return true;
    }

    bool dominateAllUses(llvm::DominatorTree *dt, llvm::Loop *loop, llvm::Instruction *instr)
    {
      for(auto *user: instr->users())
      {
        auto *userInstr = dyn_cast<llvm::Instruction>(user);
        if(userInstr && loop->contains(userInstr) && !dt->dominates(instr->getParent(), userInstr->getParent()))
        {
          /* The user instruction is in the loop but is not dominated by the instruction candidated to hoisting */
          return false;
        }
      }
      return true;
    }

    bool isSafeToHoist(llvm::Loop *loop, llvm::DominatorTree *dt, llvm::BasicBlock *bb, llvm::Instruction *instr)
    {
      /* Unique definition property is granted by SSA so we didn't check */

      if((dominateAllLoopExits(dt, loop, bb) || isVarDeadAtLoopExit(instr, loop)) && dominateAllUses(dt, loop, instr))
        return true;

      return false;
    }

    void runLICM(llvm::Loop *loop, llvm::LoopInfo *loopInfo, llvm::DominatorTree *dt)
    {      
      auto *preHeader = loop->getLoopPreheader();

      if(!preHeader)
      {
        /* Extra check that should be always true since we execute only on loops in normal form */
        return;
      }

      loopInvInstrSet.clear();
      std::vector<llvm::BasicBlock*> loopPreOrderBBVec;
      std::vector<llvm::Instruction*> instrToMove;
      getBBPreorderVec(loop, dt, dt->getRootNode(), &loopPreOrderBBVec);

      for(auto bb: loopPreOrderBBVec)
      {
        if(loopInfo->getLoopFor(bb) == loop)
        {
          /* The basic block is contained inside the loop*/
          /* Since we got basic blocks dominated by the loop header in pre-order, 
             we can have the exit block that is dominated by the loop header but isn't inside the loop
          */
          for(auto &instr: *bb)
          {
            if(isInstructionLoopInvariant(&instr, loop))
            {
              loopInvInstrSet.insert(&instr);
              
              if(isSafeToHoist(loop, dt, bb, &instr))
              {
                instrToMove.push_back(&instr);
              }
            }
          }
        }
      }

      for(auto *instr: instrToMove)
      {
        if(isDepHoisted(instr, loop))
        {
          instr->moveBefore(preHeader->getTerminator());
        }
      }
    }

    /* Pre-order visit of the dominator tree of the loop's basic blocks */
    void getBBPreorderVec(llvm::Loop *loop, llvm::DominatorTree *dt, llvm::DomTreeNode *dtn, std::vector<llvm::BasicBlock*> *loopPreOrderBBVec)
    {
      auto bb = dtn->getBlock();
      if(dt->dominates(loop->getHeader(), bb))
        loopPreOrderBBVec->push_back(bb);
      
      for(auto child: dtn->children())
        getBBPreorderVec(loop, dt, child, loopPreOrderBBVec);
    }

    bool isInstructionLoopInvariant(llvm::Instruction *instr, llvm::Loop *loop)
    {
      if(instr->mayHaveSideEffects() || instr->isVolatile() || isa<PHINode>(instr))
        return false;

      return std::all_of(instr->operands().begin(), instr->operands().end(), [this, loop](llvm::Value *v) { return isOperandLoopInvariant(v, loop);});
    }

    bool isOperandLoopInvariant(llvm::Value *operand, llvm::Loop *loop)
    {
      auto instr = dyn_cast<llvm::Instruction>(operand);

      if(isa<llvm::Constant>(operand) || isa<llvm::Argument>(operand) || (instr && (!loop->contains(instr) || loopInvInstrSet.count(instr))))
      {
        return true;
      }

      return false;
    }
    bool isDepHoisted(llvm::Instruction *instr, llvm::Loop *loop)
    {
      return std::all_of(instr->operands().begin(), instr->operands().end(), [loop](llvm::Value* v){ 
        llvm::Instruction* opInstr = dyn_cast<llvm::Instruction>(v);
        
        if(opInstr && loop->contains(opInstr) && opInstr->getParent() != loop->getLoopPreheader())
          return false;

        return true;
      });
    }

    static bool isRequired()
    {
      return true;
    }

private:
    std::unordered_set<llvm::Instruction*> loopInvInstrSet;
  };
}

llvm::PassPluginLibraryInfo assignment3PluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "Assignment3", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "mylicm")
                  {
                    FPM.addPass(LICM());
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
  return assignment3PluginInfo();
}
