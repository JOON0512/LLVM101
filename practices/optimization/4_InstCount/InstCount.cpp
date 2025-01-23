#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Instructions.h"
#include "InstCount.h"

bool InstCount::runOnFunction(Function &F) {

    // init inst cnt
    int inst_cnt = 0;

    // iterate for all basic blocks
    for(BasicBlock &BB : F) {

        // iterate for all instructions within basic block
        for(Instruction &I : BB) {
            inst_cnt++;
        }


    }

    dbgs() << "Function Name: " << F.getName() << "\n";
    dbgs() << "# of Instrunctions: " << inst_cnt << "\n";

    return false;
}

void InstCount::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

char InstCount::ID = 0;
static RegisterPass<InstCount> Y("inst-count", "InstCount Pass ");
