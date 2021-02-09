//===-- MINA32RegisterInfo.cpp - MINA32 Register Information --------------===//
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

#include "MINA32RegisterInfo.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/IR/Function.h"

#include "MCTargetDesc/MINA32MCTargetDesc.h"
#include "MINA32.h"
#include "MINA32Subtarget.h"
#include "MINA32TargetMachine.h"

#define GET_REGINFO_TARGET_DESC
#include "MINA32GenRegisterInfo.inc"

namespace llvm {

MINA32RegisterInfo::MINA32RegisterInfo() : MINA32GenRegisterInfo(MINA32::PC) {}

const MCPhysReg *
MINA32RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

const uint32_t *
MINA32RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                         CallingConv::ID CC) const {
  return CSR_RegMask;
}

bool MINA32RegisterInfo::hasBasePointer(const MachineFunction &MF) const {
  // When we need stack realignment, we can't reference off of the
  // stack pointer, so we reserve a base pointer.
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return MFI.getNumFixedObjects() && needsStackRealignment(MF);
}

BitVector MINA32RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();

  // FIXME: avoid re-calculating this every time.
  BitVector Reserved(getNumRegs());
  markSuperRegs(Reserved, MINA32::SP);
  markSuperRegs(Reserved, MINA32::PC);
  markSuperRegs(Reserved, MINA32::R8_USER);
  markSuperRegs(Reserved, MINA32::R9_USER);
  markSuperRegs(Reserved, MINA32::R10_USER);
  markSuperRegs(Reserved, MINA32::R11_USER);
  markSuperRegs(Reserved, MINA32::R12_USER);
  markSuperRegs(Reserved, MINA32::SP_USER);
  markSuperRegs(Reserved, MINA32::LR_USER);
  markSuperRegs(Reserved, MINA32::PC_USER);
  markSuperRegs(Reserved, MINA32::MCR);
  markSuperRegs(Reserved, MINA32::FRET);
  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, MINA32::R11);
  if (hasBasePointer(MF))
    markSuperRegs(Reserved, BasePtr);

  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

void MINA32RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  // TODO
}

Register MINA32RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? MINA32::R11 : MINA32::SP;
}

} // end namespace llvm
