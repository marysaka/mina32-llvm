//===-- MINA32InstrInfo.h - MINA32 Instruction Information ------*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the MINA32 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_INSTR_INFO_H
#define LLVM_TARGET_MINA32_INSTR_INFO_H

#include "llvm/CodeGen/TargetInstrInfo.h"

#include "MINA32RegisterInfo.h"

#define GET_INSTRINFO_HEADER
#include "MINA32GenInstrInfo.inc"
#undef GET_INSTRINFO_HEADER

namespace llvm {

/// Utilities related to the MINA32 instruction set.
class MINA32InstrInfo : public MINA32GenInstrInfo {
public:
  explicit MINA32InstrInfo();

  const MINA32RegisterInfo &getRegisterInfo() const { return RI; }

private:
  const MINA32RegisterInfo RI;
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_INSTR_INFO_H