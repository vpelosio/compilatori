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

    /* The block which have as successor the latch is the body tail */
    BasicBlock *getBodyTail(Loop *l)
    {
      auto *latch = l->getLoopLatch();
      for (auto *bb : l->blocks())
      {
        for (auto *succ : successors(bb))
        {
          if (succ == latch)
          {
            return bb;
          }
        }
      }
      return nullptr;
    }

    /* The successor of the header which is inside the loop is the first block of the body */
    BasicBlock *getFirstBodyBlock(Loop *l)
    {
      auto *header = l->getHeader();
      for (auto *succ : successors(header))
      {
        if (l->contains(succ))
          return succ;
      }
      return nullptr;
    }


    /* Reference https://groups.google.com/g/llvm-dev/c/YfQRheMqMkM/m/Abl1DIWcAQAJ */
    /* getInductionVariable cannot be used since it requires and works only on rotated loops */
    PHINode *findInductionPhi(Loop *L, ScalarEvolution &SE)
    {
      BasicBlock *Header = L->getHeader();
      if (!Header)
      {
        return nullptr; // A loop must have a header.
      }

      // Iterate through all PHI nodes in the loop header.
      for (PHINode &PN : Header->phis())
      {
        // Get the SCEV (Scalar Evolution Expression) for the current PHI node.
        const SCEV *S = SE.getSCEV(&PN);

        // Check if the SCEV is an SCEVAddRecExpr within the current loop.
        // An SCEVAddRecExpr represents a value that is updated in a recurrence
        // pattern (e.g., A + B*i, which is characteristic of an induction variable).
        if (const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(S))
        {
          // Check if this AddRec expression is "controlled" by (i.e., belongs to)
          // the current loop. This ensures we're looking at an induction variable
          // for 'L' specifically.
          if (AR->getLoop() == L)
          {
            // We've found an induction variable. The PHI node 'PN' is it.
            // You could further inspect AR to get the start value (AR->getStart())
            // and the step value (AR->getStepRecurrence()), if needed.
            return &PN;
          }
        }
      }
      return nullptr; // No suitable induction variable PHI found
    }

    bool fuseLoops(Loop *l1, Loop *l2, LoopInfo &li, DominatorTree &dt, ScalarEvolution &se, DependenceInfo &di)
    {
      if (l1->isGuarded() && l2->isGuarded())
      {
        // exit of l1 guard will point to the exit bb of l2 guard
        auto *l1GuardBB = getLoopEntryBB(l1);
        auto *brInstr = dyn_cast<BranchInst>(l1GuardBB->getTerminator());
        auto *l2ExitBB = l2->getExitBlock();

        for (unsigned i = 0; i < brInstr->getNumSuccessors(); ++i)
        {
          auto *succBB = brInstr->getSuccessor(i);
          if (!l1->contains(succBB) && succBB != l1->getLoopPreheader())
          {
            brInstr->setSuccessor(i, l2ExitBB);
            break;
          }
        }
      }

      auto phiLoopL1 = findInductionPhi(l1, se);
      auto phiLoopL2 = findInductionPhi(l2, se);
      if(!phiLoopL1 || !phiLoopL2) return false;

      auto *addRecExprL1 = dyn_cast<SCEVAddRecExpr>(se.getSCEV(phiLoopL1));
      auto *addRecExprL2 = dyn_cast<SCEVAddRecExpr>(se.getSCEV(phiLoopL2));
      if(!addRecExprL1 || !addRecExprL2) return false;

      auto *startL1 = dyn_cast<SCEVConstant>(addRecExprL1->getStart());
      auto *stepL1 = dyn_cast<SCEVConstant>(addRecExprL1->getStepRecurrence(se));
      auto *startL2 = dyn_cast<SCEVConstant>(addRecExprL2->getStart());
      auto *stepL2 = dyn_cast<SCEVConstant>(addRecExprL2->getStepRecurrence(se));

      if(!startL1 || !stepL1 || !startL2 || !stepL2) return false;


      IRBuilder<> builder(&*l1->getHeader()->getFirstInsertionPt());
      auto *newIndVarL2 = transformLoopIndVar(builder, phiLoopL1, startL1, stepL1, startL2, stepL2);

      phiLoopL2->replaceAllUsesWith(newIndVarL2);

      auto *headerL1 = l1->getHeader();
      auto *bodyTailL1 = getBodyTail(l1);
      auto *latchL1 = l1->getLoopLatch();

      auto *firstBodyBBL2 = getFirstBodyBlock(l2);
      auto *bodyTailL2 = getBodyTail(l2);
      auto *exitL2 = l2->getExitBlock();

      if (!headerL1 || !bodyTailL1 || !latchL1 || !firstBodyBBL2 || !bodyTailL2 || !exitL2)
      {
        return false;
      }

      /* Connect the body of L1 to the body of L2*/
      auto *brInstr = dyn_cast<BranchInst>(bodyTailL1->getTerminator());
      brInstr->setSuccessor(0, firstBodyBBL2);

      /* Connect the body tail of L2 to the latch of L1*/
      BranchInst *brInstr2 = dyn_cast<BranchInst>(bodyTailL2->getTerminator());
      brInstr2->setSuccessor(0, latchL1);

      /* Connect the L1 header exit to the L2 exit */
      BranchInst *brInstr3 = dyn_cast<BranchInst>(headerL1->getTerminator());
      for (unsigned i = 0; i < brInstr3->getNumSuccessors(); ++i)
      {
        BasicBlock *Succ = brInstr3->getSuccessor(i);
        if (!l1->contains(Succ))
        {
          brInstr3->setSuccessor(i, exitL2);
          break;
        }
      }
      return true;
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
      auto &start1Val = start1->getAPInt();
      auto &step1Val = step1->getAPInt();
      auto &start2Val = start2->getAPInt();
      auto &step2Val = step2->getAPInt();

      uint64_t ratio = (step2Val.getZExtValue() / step1Val.getZExtValue());

      auto *Offset = builder.getInt64(start2Val.getZExtValue());
      auto *Start1 = builder.getInt64(start1Val.getZExtValue());
      auto *Ratio = builder.getInt64(ratio);

      // Ensure the input induction variable 'i' is also 64-bit for consistent arithmetic.
      // This is done to avoid potential issues with mixed bit-width arithmetic.
      auto *i64 = i;
      if (i->getType()->isIntegerTy())
      {
        unsigned bitw = cast<IntegerType>(i->getType())->getBitWidth();
        if (bitw < 64)
          i64 = builder.CreateZExt(i, builder.getInt64Ty(), "i.cast");
      }

      auto *diff = builder.CreateSub(i64, Start1, "diff");
      auto *scaled = builder.CreateMul(Ratio, diff, "scaled");
      auto *newIndVarL2 = builder.CreateAdd(Offset, scaled, "newIndVarL2");

      return builder.CreateTrunc(newIndVarL2, builder.getInt32Ty(), "newIndVarL2.i32");
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
