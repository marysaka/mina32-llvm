//===-- MINA32RegisterInfo.h - MINA32 Register Information Impl -*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the MINA32 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_REGISTER_INFO_H
#define LLVM_TARGET_MINA32_REGISTER_INFO_H

#include "MCTargetDesc/MINA32MCTargetDesc.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "MINA32GenRegisterInfo.inc"

namespace llvm {

/// Utilities relating to MINA32 registers.
class MINA32RegisterInfo : public MINA32GenRegisterInfo {
public:
  // Follow ARM here
  unsigned BasePtr = MINA32::R6;

  MINA32RegisterInfo();

  /// Code Generation Methods
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const override;
  BitVector getReservedRegs(const MachineFunction &MF) const override;

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS) const override;
  Register getFrameRegister(const MachineFunction &MF) const override;

  bool hasBasePointer(const MachineFunction &MF) const;

  const TargetRegisterClass *
  getCrossCopyRegClass(const TargetRegisterClass *RC) const override;
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_REGISTER_INFO_H
