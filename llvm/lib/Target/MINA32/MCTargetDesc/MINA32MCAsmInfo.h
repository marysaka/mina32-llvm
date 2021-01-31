//===-- MINA32MCAsmInfo.h - MINA32 asm properties ---------------*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MINA32MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_ASM_INFO_H
#define LLVM_TARGET_MINA32_ASM_INFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {

class Triple;

/// Specifies the format of MINA32 assembly files.
class MINA32MCAsmInfo : public MCAsmInfo {
public:
  explicit MINA32MCAsmInfo(const Triple &TT, const MCTargetOptions &Options);
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_ASM_INFO_H
