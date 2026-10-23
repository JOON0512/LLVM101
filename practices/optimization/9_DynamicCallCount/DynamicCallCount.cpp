#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "DynamicCallCount.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

bool DynamicCallCount::runOnModule(Module &M) {
    if (M.getNamedMetadata("llvm101.dynamic-count")) return false;
    SmallVector<CallBase *, 32> Calls;
    for (Function &F : M) {
        if (F.getName() == "countCall" || F.getName() == "printResult" ||
            F.getName() == "countCallPerInst" || F.getName() == "printResultPerInst") continue;
        for (BasicBlock &BB : F) for (Instruction &I : BB)
            if (auto *CB = dyn_cast<CallBase>(&I)) {
                if (auto *Target = dyn_cast<Function>(CB->getCalledOperand()->stripPointerCasts()))
                    if (Target->isIntrinsic() || Target->getName() == "countCall" ||
                        Target->getName() == "printResult") continue;
                Calls.push_back(CB);
            }
    }
    if (Calls.empty()) return false;
    auto &C = M.getContext();
    auto *Ty = FunctionType::get(Type::getVoidTy(C), false);
    FunctionCallee Count = M.getOrInsertFunction("countCall", Ty);
    FunctionCallee Print = M.getOrInsertFunction("printResult", Ty);
    for (CallBase *CB : Calls) IRBuilder<>(CB).CreateCall(Count);
    // Runs for normal main return and exit(), without instrumenting itself.
    Function *Finish = Function::Create(Ty, GlobalValue::InternalLinkage, "llvm101.call.finish", M);
    IRBuilder<> B(BasicBlock::Create(C, "entry", Finish));
    B.CreateCall(Print);
    B.CreateRetVoid();
    appendToGlobalDtors(M, Finish, 0);
    M.getOrInsertNamedMetadata("llvm101.dynamic-count");
    return true;
}

void DynamicCallCount::getAnalysisUsage(AnalysisUsage &AU) const {
    // Calls and a module destructor are added.
}

char DynamicCallCount::ID = 0;
static RegisterPass<DynamicCallCount> Y("dyn-call-count", "DynamicCallCount Pass ");
