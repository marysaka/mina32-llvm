//==-- MINA32.h - Top-level interface for MINA32 representation --*- C++ -*-==//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// MINA32 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_H
#define LLVM_TARGET_MINA32_H

#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class MINA32TargetMachine;
class FunctionPass;

FunctionPass *createMINA32ISelDag(MINA32TargetMachine &TM,
                                  CodeGenOpt::Level OptLevel);

namespace MINA32 {} // end namespace MINA32

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_H
