#include <unordered_set>
#include <algorithm>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

namespace
{
  /* Implementation inspired by  https://llvm.org/doxygen/LoopFuse_8cpp_source.html */
  class AddRecLoopReplacer : public SCEVRewriteVisitor<AddRecLoopReplacer> {
  public:
    AddRecLoopReplacer(ScalarEvolution &SE, const Loop &OldL, const Loop &NewL)
        : SCEVRewriteVisitor(SE), OldL(OldL), NewL(NewL) {}
 
    const SCEV *visitAddRecExpr(const SCEVAddRecExpr *Expr) {
      const Loop *ExprL = Expr->getLoop();
      SmallVector<const SCEV *, 2> Operands;
      if (ExprL == &OldL) {
        append_range(Operands, Expr->operands());
        return SE.getAddRecExpr(Operands, &NewL, Expr->getNoWrapFlags());
      }
      return nullptr;
    }
 
  private:
    const Loop &OldL, &NewL;
  };

  struct InductionInfo
  {
    PHINode *phiNode;
    const SCEV *startValue; 
    const SCEV *stepValue;  
  };

  struct LoopFusion : PassInfoMixin<LoopFusion>
  {
    PreservedAnalyses run(Function &f, FunctionAnalysisManager &fam)
    {
      auto &li = fam.getResult<LoopAnalysis>(f);
      auto &dt = fam.getResult<DominatorTreeAnalysis>(f);
      auto &pdt = fam.getResult<PostDominatorTreeAnalysis>(f);
      auto &se = fam.getResult<ScalarEvolutionAnalysis>(f);
      auto &di = fam.getResult<DependenceAnalysis>(f);
      std::vector<llvm::Loop*> loopsVec;
      
      if(!li.empty()) // At least one loop exists
      {
        for(auto *loop: li)
        {
          loopsVec.push_back(loop);
        }
      }
      if(loopsVec.size()%2 != 0) 
      {
        errs() << "\nError, loops are not multiple of 2\n";
      }
      outs() << "\n***FUNCTION " << f.getName() << "***\n";
      
      reverse(loopsVec.begin(), loopsVec.end());
      for(int i=0;i<loopsVec.size();i+=2)
      {
        outs() << "\nCHECKING\n";
        outs() << *loopsVec[i];
        outs() << "AND\n";
        outs() << *loopsVec[i+1];
        outs() << "\n";

        if (!loopsVec[i]->isLoopSimplifyForm() || !loopsVec[i+1]->isLoopSimplifyForm())
        {
            errs() << "One of the loop is not simplified\n";
            continue;
        }
        errs() << "Passed loop simplify check\n";

        if(!areLoopsAdjacent(loopsVec[i], loopsVec[i+1]))
        {
          outs() << "Loops adjacency check NOT PASSED\n";
          continue;
        }
        outs() << "Loops adjacency check PASSED\n";


        if(!areControlFlowEquivalent(loopsVec[i], loopsVec[i+1], dt, pdt))
        {
          outs() << "Control flow equivalent check NOT PASSED\n";
          continue;
        }
        outs() << "Control flow equivalent check PASSED\n";

        if(!tripCountEquivalent(se, loopsVec[i], loopsVec[i+1]))
        {
          outs() << "Trip count equivalent check NOT PASSED\n";
          continue;
        }
        outs() << "Trip count equivalent check PASSED\n";

        if(!dependencesAllowFusion(loopsVec[i], loopsVec[i+1], di, se, dt))
        {
          outs() << "Dependency check NOT PASSED\n";
          continue;
        }
        outs() << "Dependency check PASSED\n";

        outs() << "ALL CHECKS PASSED\n";

        if (!fuseLoops(loopsVec[i], loopsVec[i+1], li, dt, se, di))
        {
          outs() << "ERROR DURING LOOP FUSION, SKIPPING...\n";
          continue;
        }
        outs() << "***LOOP FUSION COMPLETED***\n";
        
        outs() << "\n";
      }
      return PreservedAnalyses::all();
    }

    bool fuseLoops(Loop *l1, Loop *l2, LoopInfo &li, DominatorTree &dt, ScalarEvolution &se, DependenceInfo &di)
    {
      /*if(l1->isGuarded() && l2->isGuarded())
      {
         // exit of l1 guard will point to the exit bb of l2 guard
         auto *l1GuardBB = getLoopEntryBB(l1);
         auto *brInstr = dyn_cast<BranchInstr>(l1GuardBB->getTerminator());
         auto *l1ExitBB = l1->getSuccessor(0);
      }*/
      return false;
    }

    bool getLoopInductionInfo(Loop *loop, ScalarEvolution &se, InductionInfo &inductionInfo)
    {
      auto *inductionVar = loop->getInductionVariable(se);
      if (!inductionVar)
        return false;

      inductionInfo.phiNode = inductionVar;

      auto *phiNodeSCEV = se.getSCEV(inductionVar);

      if (auto *AddRec = dyn_cast<SCEVAddRecExpr>(phiNodeSCEV))
      {
        inductionInfo.startValue = AddRec->getStart();
        inductionInfo.stepValue = AddRec->getStepRecurrence(se);
        return true;
      }

      return false;
    }

    /**
     * for (int i = 0; i < 10; i++) 
     * {
     *  // Loop 1 body 
     * }
     * for (int j = 10; j < 30; j += 2) 
     * {
     *  // Loop 2 body
     * }
     *
     * newJ = startL2 + (stepL2 / stepL1) * (i - startL1)
     * newJ = 10 + (2 / 1) * (i - 0) = 10 + 2i
     */
    Value *transformLoopIndVar(IRBuilder<> &builder, Value *i, const SCEVConstant *start1, const SCEVConstant *step1, const SCEVConstant *start2, const SCEVConstant *step2)
    {
      const APInt &start1Val = start1->getAPInt();
      const APInt &step1Val = step1->getAPInt();
      const APInt &start2Val = start2->getAPInt();
      const APInt &step2Val = step2->getAPInt();

      uint64_t ratio = (step2Val.getZExtValue() / step1Val.getZExtValue());

      Value *Offset = builder.getInt64(start2Val.getZExtValue());
      Value *Start1 = builder.getInt64(start1Val.getZExtValue());
      Value *Ratio = builder.getInt64(ratio);

      // Ensure the input induction variable 'i' is also 64-bit for consistent arithmetic.
      // This is done to avoid potential issues with mixed bit-width arithmetic.
      Value *i64 = i;
      if (i->getType()->isIntegerTy())
      {
        unsigned bitw = cast<IntegerType>(i->getType())->getBitWidth();
        if (bitw < 64)
          i64 = builder.CreateZExt(i, builder.getInt64Ty(), "i.cast");
      }

      Value *diff = builder.CreateSub(i64, Start1, "diff");
      Value *scaled = builder.CreateMul(Ratio, diff, "scaled");
      Value *newJ = builder.CreateAdd(Offset, scaled, "newJ");

      return builder.CreateTrunc(newJ, builder.getInt32Ty(), "newJ.i32");
    }

    /* Implementation inspired by  https://llvm.org/doxygen/LoopFuse_8cpp_source.html */
    bool accessDiffIsPositive(const Loop &l1, const Loop &l2, Instruction &i1, Instruction &i2, ScalarEvolution &se)
    {
      Value *ptr1 = getLoadStorePointerOperand(&i1);
      Value *ptr2 = getLoadStorePointerOperand(&i2);
      if (!ptr1 || !ptr2)
        return false;

      auto *SCEVPtr1 = se.getSCEVAtScope(ptr1, &l1);
      auto *SCEVPtr2 = se.getSCEVAtScope(ptr2, &l2);

      AddRecLoopReplacer Rewriter(se, l1, l2);
      SCEVPtr1 = Rewriter.visit(SCEVPtr1);
      if(!SCEVPtr1)
        return false;

      bool IsAlwaysGE = se.isKnownPredicate(ICmpInst::ICMP_SGE, SCEVPtr1, SCEVPtr2);
      return IsAlwaysGE;
    }

    bool dependencesAllowFusion(Loop *l1, Loop *l2, DependenceInfo &di, ScalarEvolution &se, DominatorTreeAnalysis::Result &dt)
    {
      for (auto *bbL1 : l1->getBlocks())
      {
        for (auto &instrL1 : *bbL1)
        {
          if (!instrL1.mayReadOrWriteMemory())
            continue; // Skip instruction that are different from load and store

          for (auto *bbL2 : l2->getBlocks())
          {
            for (auto &instrL2 : *bbL2)
            {
              if (!instrL2.mayReadOrWriteMemory())
                continue; // Skip instruction that are different from load and store

              if (auto dep = di.depends(&instrL1, &instrL2, true))
              {
                if(dep->isFlow() && !accessDiffIsPositive(*l1, *l2, instrL1, instrL2, se)) // isFlow to check if it is a Read after Write dependence
                  return false;
              }
            }
          }
        }
      }

      return true;
    }

    bool tripCountEquivalent(ScalarEvolution &se, Loop *l1, Loop *l2)
    {
      auto *tripCountl1 = se.getBackedgeTakenCount(l1);
      auto *tripCountl2 = se.getBackedgeTakenCount(l2);

      if (tripCountl1 == nullptr || tripCountl2 == nullptr || tripCountl1->getSCEVType()==scCouldNotCompute || tripCountl2->getSCEVType()==scCouldNotCompute )
      {
        return false;
      }

      if(tripCountl1 == tripCountl2)
        return true;

      auto diff = se.computeConstantDifference(tripCountl1, tripCountl2);
      if (diff.has_value() && diff.value().isZero())
      {
        return true;
      }

      return false;
    }

    BasicBlock* getLoopEntryBB(Loop* l)
    {
      if(l->isGuarded())
        return l->getLoopGuardBranch()->getParent();
      
      return l->getLoopPreheader();
    }

    /* L1 dominates L2 if every path that goes into L2 must pass thru L1 */
    /* L2 postdominates L1 if every path that goes thru L1 also goes thru L2 */
    bool areControlFlowEquivalent(Loop* l1, Loop* l2, DominatorTree &DT, PostDominatorTree &PDT)
    {
      auto *l1EntryBB = getLoopEntryBB(l1);
      auto *l2EntryBB = getLoopEntryBB(l2);

      if (DT.dominates(l1EntryBB, l2EntryBB) && PDT.dominates(l2EntryBB, l1EntryBB))
        return true;

      return false;
    }

    /* The check by requirements is weak, there can be cases where
     * the two loops are adjacent but there are instructions in the middle, 
     * eg: if both are not guarded check if outBB == loop2Preheader it is 
     * it is not sufficient to guarantee that there aren't any instructions 
     * between the two loops since we can have them in loop2 preheader 
     */
    /* To be executed on 2 simple loops with one nesting level */
    bool areLoopsAdjacent(Loop* l1, Loop* l2)
    {
      if(!l1 || !l2 || l1 == l2) 
      {
        return false; // loops must exists and be distinct
      }

      bool l1Guarded = l1->isGuarded();
      bool l2Guarded = l2->isGuarded();

      if(l1Guarded && l2Guarded)
      {
        auto *guardBranchL1 = l1->getLoopGuardBranch();

        /* Extra checks */
        if(!guardBranchL1 || guardBranchL1->getNumSuccessors() != 2) 
          return false;

        BasicBlock* outLoopSucc = nullptr;

        /* Since we are considering loops in normal form, one of the guard branch must be 
         * the loop preheader and the other must point to the guard of the second loop */

        if(guardBranchL1->getSuccessor(0) == l1->getLoopPreheader())
          outLoopSucc = guardBranchL1->getSuccessor(1);
        
        else if(guardBranchL1->getSuccessor(1) == l1->getLoopPreheader())
          outLoopSucc = guardBranchL1->getSuccessor(0);

        if(!outLoopSucc)
          return false;

        auto *guardBranchL2 = l2->getLoopGuardBranch();

        if(guardBranchL2->getOpcode() != Instruction::Br)
        {
          return false;
        }
        
        if(!guardBranchL2)
          return false;

        return outLoopSucc == guardBranchL2->getParent();
      }
      else
      {
        auto *l1Exit = l1->getExitBlock(); // l1 must have one exit block
        auto *l2Preheader = l2->getLoopPreheader();

        if(!l1Exit || !l2Preheader)
          return false;

        if(l2Preheader->size() != 1 || l2Preheader->front().getOpcode() != Instruction::Br)
        {
          // Preheader must contain only one instruction which is a br
          return false;
        }

        return l1Exit == l2Preheader;
      }
    }
    
    static bool isRequired()
    {
      return true;
    }
  };
}

llvm::PassPluginLibraryInfo assignment4PluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "Assignment4", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "myloopfusion")
                  {
                    FPM.addPass(LoopFusion());
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
  return assignment4PluginInfo();
}
