//===- MINA32FrameLowering.h - Define frame lowering for MINA32 -*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class implements MINA32-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_FRAME_LOWERING_H
#define LLVM_TARGET_MINA32_FRAME_LOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

/// Utilities for creating function call frames.
class MINA32FrameLowering : public TargetFrameLowering {
public:
  explicit MINA32FrameLowering();

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  bool hasFP(const MachineFunction &MF) const override;
  bool isFPCloseToIncomingSP() const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS = nullptr) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  int getFrameIndexReference(const MachineFunction &MF, int FI,
                             Register &FrameReg) const override;
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_FRAME_LOWERING_H
