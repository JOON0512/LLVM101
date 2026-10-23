#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "PrintEven.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/ErrorHandling.h"

bool PrintEven::runOnModule(Module &M) {
    Function *F = M.getFunction("printInt");
    if (!F || F->isDeclaration() || F->hasFnAttribute("llvm101.print-even"))
        return false;
    if (!F->getReturnType()->isVoidTy() || F->arg_size() != 1 ||
        !F->getArg(0)->getType()->isIntegerTy(32) || F->isVarArg())
        report_fatal_error("printInt must have signature void (i32)");
    auto &C = M.getContext();
    BasicBlock *Body = &F->getEntryBlock();
    BasicBlock *Guard = BasicBlock::Create(C, "even.guard", F, Body);
    BasicBlock *Skip = BasicBlock::Create(C, "odd.skip", F);
    IRBuilder<> B(Guard);
    Value *Even = B.CreateICmpEQ(B.CreateAnd(F->getArg(0), B.getInt32(1)), B.getInt32(0));
    B.CreateCondBr(Even, Body, Skip);
    B.SetInsertPoint(Skip);
    B.CreateRetVoid();
    F->addFnAttr("llvm101.print-even");
    return true;
}

void PrintEven::getAnalysisUsage(AnalysisUsage &AU) const {
    // The new guard changes control flow.
}

char PrintEven::ID = 0;
static RegisterPass<PrintEven> X("print-even", "Control Flow Practice Pass");
