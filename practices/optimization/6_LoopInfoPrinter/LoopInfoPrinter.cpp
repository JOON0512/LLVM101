#include "LoopInfoPrinter.h"
#include "queue"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

bool LoopInfoPrinter::runOnFunction(Function& F)
{
  LoopInfo& LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  std::queue<Loop*> q;

  for (Loop* L : LI.getLoopsInPreorder()) {
    q.push(L);
    dbgs() << "Loop Name: " << L->getName() << "\n";
    dbgs() << "Loop ID: " << L->getLoopID() << "\n";
    dbgs() << "Depth of Loop: " << L->getLoopDepth() << "\n";
    if (L->isInnermost()) {
      dbgs() << "Innermost Depth of Loop: " << L->getLoopDepth() << "\n";
    }
    PHINode* Node = L->getCanonicalInductionVariable();
    if (Node) {
      dbgs() << "  Induction Variable: \n";
      Node->dump();
    }
    dbgs() << "\n";
  }

  return false;
}

void LoopInfoPrinter::getAnalysisUsage(AnalysisUsage& AU) const
{
  AU.setPreservesAll();
  AU.addRequired<LoopInfoWrapperPass>();
}

char LoopInfoPrinter::ID = 0;
static RegisterPassqLoopInfoPrinter > Y("loop-info-printer", "LoopInfoPrinter Pass ");
