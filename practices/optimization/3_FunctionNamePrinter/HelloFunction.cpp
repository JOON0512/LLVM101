#include "HelloFunction.h"
#include "llvm/Pass.h"

#define DEBUG_TYPE "hello"

bool HelloFunction::runOnFunction(Function &F) {
    //errs() << "Hello Function Name: " << F.getName() << "\n";
    return false;
}

void HelloFunction::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

char HelloFunction::ID = 0;
static RegisterPass<HelloFunction> Z("helloFunction", "Hello World Pass ");
