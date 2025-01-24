#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Instructions.h"
#include "CallCount.h"

bool CallCount::runOnFunction(Function &F) {

    int call_cnt = 0;
    for(BasicBlock &BB : F) {
        for(Instruction &I : BB) {
            // either use CallInst* CI = dyn_cast<CallInst>(&I)
            // or use isa<CallInst>(&I)
            if(isa<CallInst>(&I)) {
                call_cnt++;
            }
        }
    }

    dbgs() << "Function Name: " << F.getName() << "\n";
    dbgs() << "# of CallInst: " << call_cnt << "\n";

    return false;
}

void CallCount::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

char CallCount::ID = 0;
static RegisterPass<CallCount> Y("callcount", "CallCount Pass ");
