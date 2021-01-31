//===-- MINA32FrameLowering.cpp - MINA32 Frame Information ----------------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the MINA32 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "MINA32FrameLowering.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegionInfo.h"

#include "MINA32InstrInfo.h"
#include "MINA32Subtarget.h"

namespace llvm {

MINA32FrameLowering::MINA32FrameLowering()
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 0,
                          Align(4)) {}

// TODO: Prologue and Epilogue.

void MINA32FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {}

void MINA32FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {}

bool MINA32FrameLowering::hasFP(const MachineFunction &MF) const {
  return MF.getFrameInfo().hasVarSizedObjects();
}

bool MINA32FrameLowering::isFPCloseToIncomingSP() const { return false; }

void MINA32FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                               BitVector &SavedRegs,
                                               RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
}

MachineBasicBlock::iterator MINA32FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  return MBB.erase(I);
}

int MINA32FrameLowering::getFrameIndexReference(const MachineFunction &MF,
                                                int FI,
                                                Register &FrameReg) const {
  const MINA32Subtarget &Subtarget = MF.getSubtarget<MINA32Subtarget>();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const MINA32RegisterInfo *RegInfo = Subtarget.getRegisterInfo();

  // Fill in FrameReg output argument.
  FrameReg = RegInfo->getFrameRegister(MF);

  // Start with the offset of FI from the top of the allocated frame.
  // As the stack grows downwards, this offset will be negative.
  int64_t FrameOffset = MFI.getObjectOffset(FI) + MFI.getOffsetAdjustment();
  // Make the frame offset relative to the incoming stack pointer.
  FrameOffset -= getOffsetOfLocalArea();
  // Make the frame offset relative to the bottom of the frame.
  FrameOffset += MFI.getStackSize();

  return FrameOffset;
}

} // end namespace llvm
