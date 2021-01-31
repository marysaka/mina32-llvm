//===-- MINA32TargetMachine.cpp - Define TargetMachine for MINA32 ---------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about MINA32 target spec.
//
//===----------------------------------------------------------------------===//

#include "MINA32TargetMachine.h"

#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetRegistry.h"

//#include "MINA32.h"
#include "TargetInfo/MINA32TargetInfo.h"

namespace llvm {

static const char *MINA32DataLayout = "e-p:32:32"; // TODO: more?

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMINA32Target() {
  // Register the target.
  RegisterTargetMachine<MINA32TargetMachine> X(getTheMINA32Target());
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  return RM.hasValue() ? *RM : Reloc::Static;
}

MINA32TargetMachine::MINA32TargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         Optional<CodeModel::Model> CM,
                                         CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, MINA32DataLayout, TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      SubTarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}

namespace {

/// MINA32 Code Generator Pass Configuration Options.
class MINA32PassConfig : public TargetPassConfig {
public:
  MINA32PassConfig(MINA32TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  MINA32TargetMachine &getMINA32TargetMachine() const {
    return getTM<MINA32TargetMachine>();
  }

  bool addInstSelector() override;
};

} // end namespace

TargetPassConfig *MINA32TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new MINA32PassConfig(*this, PM);
}

const MINA32Subtarget *MINA32TargetMachine::getSubtargetImpl() const {
  return &SubTarget;
}

const MINA32Subtarget *
MINA32TargetMachine::getSubtargetImpl(const Function &) const {
  return &SubTarget;
}

//===----------------------------------------------------------------------===//
// Pass Pipeline Configuration
//===----------------------------------------------------------------------===//

bool MINA32PassConfig::addInstSelector() {
  // Install an instruction selector.
  addPass(createMINA32ISelDag(getMINA32TargetMachine(), getOptLevel()));

  return false;
}

} // end namespace llvm
