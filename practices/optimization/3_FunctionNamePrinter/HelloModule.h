#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
    struct HelloModule : public ModulePass {
        static char ID;
        HelloModule() : ModulePass(ID) {}

        bool runOnModule(Module &M) override;

        void getAnalysisUsage(AnalysisUsage &AU) const override;
    };
}
