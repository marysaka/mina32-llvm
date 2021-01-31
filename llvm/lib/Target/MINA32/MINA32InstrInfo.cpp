//===-- MINA32InstrInfo.cpp - MINA32 Instruction Information --------------===//
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

#include "MINA32InstrInfo.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "MINA32GenInstrInfo.inc"

namespace llvm {

MINA32InstrInfo::MINA32InstrInfo()
    : MINA32GenInstrInfo(MINA32::ADJCALLSTACKDOWN, MINA32::ADJCALLSTACKUP),
      RI() {}

} // end namespace llvm