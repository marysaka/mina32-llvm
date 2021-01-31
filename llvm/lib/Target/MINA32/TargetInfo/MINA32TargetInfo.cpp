//===-- MINA32TargetInfo.cpp - MINA32 Target Implementation ---------------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/MINA32TargetInfo.h"

#include "llvm/Support/TargetRegistry.h"

namespace llvm {

Target &getTheMINA32Target() {
  static Target TheMINA32Target;
  return TheMINA32Target;
}

} // end namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMINA32TargetInfo() {
  llvm::RegisterTarget<llvm::Triple::mina32> X(
      llvm::getTheMINA32Target(), "MINA32", "MaryCorp MINA32 Microprocessor",
      "MINA32");
}
