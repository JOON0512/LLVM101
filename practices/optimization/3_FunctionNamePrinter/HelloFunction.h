#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
    struct HelloFunction : public FunctionPass {
        static char ID;
        HelloFunction() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override;

        void getAnalysisUsage(AnalysisUsage &AU) const override;
    };
}
