#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"

#include "MemoryProfiler.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

bool MemoryProfiler::runOnModule(Module &M) {
    LLVMContext &Context = M.getContext();
    const DataLayout &Layout = M.getDataLayout();

    if (M.getNamedMetadata("llvm101.memory-profiler")) return false;
    SmallVector<Instruction *, 64> Work;
    for (Function &F : M) {
        if (F.getName().startswith("trace") || F.getName() == "printMemorySummary") continue;
        for (BasicBlock &BB : F) for (Instruction &I : BB)
            if (isa<LoadInst>(I) || isa<StoreInst>(I) || isa<CallInst>(I)) Work.push_back(&I);
    }
    Type *Void = Type::getVoidTy(Context), *Ptr = Type::getInt8PtrTy(Context);
    IntegerType *Size = Layout.getIntPtrType(Context);
    auto Malloc = M.getOrInsertFunction("traceMalloc", Void, Ptr, Size);
    auto Load = M.getOrInsertFunction("traceLoad", Void, Ptr, Size);
    auto Store = M.getOrInsertFunction("traceStore", Void, Ptr, Size);
    auto Free = M.getOrInsertFunction("traceFree", Void, Ptr);
    for (Instruction *I : Work) {
        IRBuilder<> B(I);
        Value *Address = nullptr;
        Type *ValueTy = nullptr;
        bool IsLoad = false;
        if (auto *L = dyn_cast<LoadInst>(I)) {
            Address = L->getPointerOperand(); ValueTy = L->getType(); IsLoad = true;
        } else if (auto *S = dyn_cast<StoreInst>(I)) {
            Address = S->getPointerOperand(); ValueTy = S->getValueOperand()->getType();
        } else if (auto *Call = dyn_cast<CallInst>(I)) {
            auto *F = dyn_cast<Function>(Call->getCalledOperand()->stripPointerCasts());
            if (!F) continue;
            if (F->getName() == "malloc" && Call->arg_size() == 1 &&
                Call->getType()->isPointerTy() && Call->getArgOperand(0)->getType()->isIntegerTy()) {
                // A musttail call must remain immediately before its return.
                if (Call->isMustTailCall() || Call->getType()->getPointerAddressSpace() != 0) continue;
                Call->setTailCallKind(CallInst::TCK_None);
                B.SetInsertPoint(Call->getNextNode());
                B.CreateCall(Malloc, {B.CreatePointerCast(Call, Ptr),
                    B.CreateZExtOrTrunc(Call->getArgOperand(0), Size)});
            } else if (F->getName() == "free" && Call->arg_size() == 1 &&
                       Call->getArgOperand(0)->getType()->isPointerTy() &&
                       Call->getArgOperand(0)->getType()->getPointerAddressSpace() == 0) {
                B.CreateCall(Free, {B.CreatePointerCast(Call->getArgOperand(0), Ptr)});
            }
            continue;
        }
        if (!Address || Address->getType()->getPointerAddressSpace() != 0) continue;
        TypeSize Bytes = Layout.getTypeStoreSize(ValueTy);
        if (Bytes.isScalable()) continue;
        B.CreateCall(IsLoad ? Load : Store, {B.CreatePointerCast(Address, Ptr),
            ConstantInt::get(Size, Bytes.getFixedValue())});
    }
    auto *Ty = FunctionType::get(Void, false);
    Function *Finish = Function::Create(Ty, GlobalValue::InternalLinkage, "llvm101.memory.finish", M);
    IRBuilder<> B(BasicBlock::Create(Context, "entry", Finish));
    B.CreateCall(M.getOrInsertFunction("printMemorySummary", Ty));
    B.CreateRetVoid();
    appendToGlobalDtors(M, Finish, 0);
    M.getOrInsertNamedMetadata("llvm101.memory-profiler");
    return true;
}

void MemoryProfiler::getAnalysisUsage(AnalysisUsage &AU) const {
    // Instrumentation calls and a destructor change the module.
}

char MemoryProfiler::ID = 0;
static RegisterPass<MemoryProfiler> Y("mem-prof", "MemoryProfiler Pass ");
