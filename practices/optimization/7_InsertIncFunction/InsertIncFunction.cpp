#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/ErrorHandling.h"

#include "InsertIncFunction.h"

bool InsertIncFunction::runOnModule(Module &M) {
    auto &C = M.getContext();
    auto *Ty = FunctionType::get(Type::getInt32Ty(C), {Type::getInt32Ty(C)}, false);
    Function *F = M.getFunction("inc");
    if (M.getNamedValue("inc") && (!F || F->getFunctionType() != Ty))
        report_fatal_error("inc must have signature i32 (i32)");
    if (F && !F->isDeclaration()) return false;
    if (!F) F = Function::Create(Ty, GlobalValue::ExternalLinkage, "inc", M);
    F->getArg(0)->setName("value");
    IRBuilder<> B(BasicBlock::Create(C, "entry", F));
    // Plain add has wrapping semantics; do not claim nsw/nuw.
    B.CreateRet(B.CreateAdd(F->getArg(0), B.getInt32(1), "incremented"));
    return true;
}

void InsertIncFunction::getAnalysisUsage(AnalysisUsage &AU) const {
    // This pass adds a function body and therefore changes module analyses.
}

char InsertIncFunction::ID = 0;
static RegisterPass<InsertIncFunction> X("insert-inc-fun", "Hello World Pass ");
