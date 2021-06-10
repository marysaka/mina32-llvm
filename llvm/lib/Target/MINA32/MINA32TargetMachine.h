//===-- MINA32TargetMachine.cpp - Define TargetMachine for MINA32 ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MINA32_TARGET_MACHINE_H
#define LLVM_LIB_TARGET_MINA32_TARGET_MACHINE_H

#include "llvm/Target/TargetMachine.h"

#include "MINA32.h"
#include "MINA32InstrInfo.h"
#include "MINA32Subtarget.h"

namespace llvm {

/// A generic MINA32 implementation.
class MINA32TargetMachine : public LLVMTargetMachine {
public:
  MINA32TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                      CodeGenOpt::Level OL, bool JIT);

  const MINA32Subtarget *getSubtargetImpl() const;
  const MINA32Subtarget *getSubtargetImpl(const Function &) const override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  bool isMachineVerifierClean() const override { return false; }

private:
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  MINA32Subtarget SubTarget;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_MINA32_TARGET_MACHINE_H
