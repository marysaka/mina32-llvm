//===-- MINA32Subtarget.cpp - MINA32 Subtarget Information ----------------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the MINA32-specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "MINA32Subtarget.h"

#include "llvm/Support/TargetRegistry.h"

#include "MINA32.h"
#include "MINA32TargetMachine.h"

#define DEBUG_TYPE "MINA32-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "MINA32GenSubtargetInfo.inc"

namespace llvm {

// Pin the VTable to this file.
void MINA32Subtarget::anchor() {}

MINA32Subtarget::MINA32Subtarget(const Triple &TT, const std::string &CPU,
                                 const std::string &FS, const TargetMachine &TM)
    : MINA32GenSubtargetInfo(TT, CPU, FS), InstrInfo(), FrameLowering(),
      TLInfo(TM, initializeSubtargetDependencies(CPU, FS, TM)) {
  // Parse features string.
  ParseSubtargetFeatures(CPU, FS);
}

MINA32Subtarget &
MINA32Subtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                                 const TargetMachine &TM) {
  // Parse features string.
  ParseSubtargetFeatures(CPU, FS);
  return *this;
}

} // end namespace llvm
