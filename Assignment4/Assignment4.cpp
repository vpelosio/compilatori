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
   class AddRecLoopReplacer : public SCEVRewriteVisitor<AddRecLoopReplacer> {
  public:
    AddRecLoopReplacer(ScalarEvolution &SE, const Loop &OldL, const Loop &NewL,
                       bool UseMax = true)
        : SCEVRewriteVisitor(SE), Valid(true), UseMax(UseMax), OldL(OldL),
          NewL(NewL) {}
 
    const SCEV *visitAddRecExpr(const SCEVAddRecExpr *Expr) {
      const Loop *ExprL = Expr->getLoop();
      SmallVector<const SCEV *, 2> Operands;
      if (ExprL == &OldL) {
        append_range(Operands, Expr->operands());
        return SE.getAddRecExpr(Operands, &NewL, Expr->getNoWrapFlags());
      }
 
      if (OldL.contains(ExprL)) {
        bool Pos = SE.isKnownPositive(Expr->getStepRecurrence(SE));
        if (!UseMax || !Pos || !Expr->isAffine()) {
          Valid = false;
          return Expr;
        }
        return visit(Expr->getStart());
      }
 
      for (const SCEV *Op : Expr->operands())
        Operands.push_back(visit(Op));
      return SE.getAddRecExpr(Operands, ExprL, Expr->getNoWrapFlags());
    }
 
    bool wasValidSCEV() const { return Valid; }
 
  private:
    bool Valid, UseMax;
    const Loop &OldL, &NewL;
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
          /* Normal form not checked otherwise I cannot test areControlFlowEquivalent */
          loopsVec.push_back(loop);
        }
      }
      if(loopsVec.size()%2 != 0) 
      {
        errs() << "\nError, loops are not multiple of 2\n";
      }
      outs() << "\nIn function " << f.getName() << "\n";
      
      reverse(loopsVec.begin(), loopsVec.end());
      for(int i=0;i<loopsVec.size();i+=2)
      {
        outs() << "\n\nCHECKING\n";
        outs() << *loopsVec[i];
        outs() << "AND\n";
        outs() << *loopsVec[i+1];
        bool res = areLoopsAdjacent(loopsVec[i], loopsVec[i+1]);
        outs() << "\nADJACENT: " << res << "\n\n";
        outs() << "\nCONTROLFLOWEQ: " << areControlFlowEquivalent(loopsVec[i], loopsVec[i+1], dt, pdt);
        bool tripCount = tripCountEquivalent(se, loopsVec[i], loopsVec[i+1]);
        outs() << "\nTRIPCOUNTEQUIVALENT: " << tripCount << "\n";
        bool depAllowFusion = dependencesAllowFusion(loopsVec[i], loopsVec[i+1], di, se, dt);
        outs() << "\n DEP ALLOW FUSION: " << depAllowFusion << "\n\n";
      }
      return PreservedAnalyses::all();
    }

    bool accessDiffIsPositive(const Loop &L0, const Loop &L1, Instruction &I0,
                              Instruction &I1, bool EqualIsInvalid, DominatorTreeAnalysis::Result &DT, ScalarEvolution &SE)
    {
      Value *Ptr0 = getLoadStorePointerOperand(&I0);
      Value *Ptr1 = getLoadStorePointerOperand(&I1);
      if (!Ptr0 || !Ptr1)
        return false;

      const SCEV *SCEVPtr0 = SE.getSCEVAtScope(Ptr0, &L0);
      const SCEV *SCEVPtr1 = SE.getSCEVAtScope(Ptr1, &L1);
        outs() << "    Access function check: " << *SCEVPtr0 << " vs "
                          << *SCEVPtr1 << "\n";
      AddRecLoopReplacer Rewriter(SE, L0, L1);
      SCEVPtr0 = Rewriter.visit(SCEVPtr0);
      outs() << "    Access function after rewrite: " << *SCEVPtr0
                          << " [Valid: " << Rewriter.wasValidSCEV() << "]\n";

      outs() << " Access function ptr 2 " << *SCEVPtr1 << "\n";
      if (!Rewriter.wasValidSCEV())
        return false;

      // TODO: isKnownPredicate doesnt work well when one SCEV is loop carried (by
      //       L0) and the other is not. We could check if it is monotone and test
      //       the beginning and end value instead.

      BasicBlock *L0Header = L0.getHeader();
      auto HasNonLinearDominanceRelation = [&](const SCEV *S)
      {
        const SCEVAddRecExpr *AddRec = dyn_cast<SCEVAddRecExpr>(S);
        if (!AddRec)
          return false;
        return !DT.dominates(L0Header, AddRec->getLoop()->getHeader()) &&
               !DT.dominates(AddRec->getLoop()->getHeader(), L0Header);
      };
      if (SCEVExprContains(SCEVPtr1, HasNonLinearDominanceRelation))
        return false;

      ICmpInst::Predicate Pred =
          EqualIsInvalid ? ICmpInst::ICMP_SGT : ICmpInst::ICMP_SGE;
      bool IsAlwaysGE = SE.isKnownPredicate(Pred, SCEVPtr0, SCEVPtr1);
      outs() << "    Relation: " << *SCEVPtr0
                          << (IsAlwaysGE ? "  >=  " : "  may <  ") << *SCEVPtr1
                          << "\n";
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

              if (di.depends(&instrL1, &instrL2, true))
              {
                bool isPositive = accessDiffIsPositive(*l1, *l2, instrL1, instrL2, false, dt, se);
                outs() << "IS POSITIVE? " << isPositive << "\n";
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

      outs() << "***TRIP COUNT ANALYSIS***\n";
      outs() << *tripCountl1 << "\n";
      outs() << *tripCountl2 << "\n";

      if (tripCountl1 == nullptr || tripCountl2 == nullptr || tripCountl1->getSCEVType()==scCouldNotCompute || tripCountl2->getSCEVType()==scCouldNotCompute )
      {
        return false;
      }

      if(tripCountl1 == tripCountl2)
        return true;

      auto diff = se.computeConstantDifference(tripCountl1, tripCountl2);
      if(diff.has_value())
        outs() << "DIFF VALUE: " << diff << "\n\n";
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
