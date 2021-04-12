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

MINA32RegisterInfo::MINA32RegisterInfo() : MINA32GenRegisterInfo(MINA32::SP) {}

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
  markSuperRegs(Reserved, MINA32::R8_USER);
  markSuperRegs(Reserved, MINA32::R9_USER);
  markSuperRegs(Reserved, MINA32::R10_USER);
  markSuperRegs(Reserved, MINA32::R11_USER);
  markSuperRegs(Reserved, MINA32::R12_USER);
  markSuperRegs(Reserved, MINA32::R13_USER);
  markSuperRegs(Reserved, MINA32::R14_USER);
  markSuperRegs(Reserved, MINA32::SP_USER);
  markSuperRegs(Reserved, MINA32::FRET);
  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, MINA32::R14);
  if (hasBasePointer(MF))
    markSuperRegs(Reserved, BasePtr);

  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

void MINA32RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  // TODO: this implementation is a temporary placeholder which does just
  // enough to allow other aspects of code generation to be tested

  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineFunction &MF = *MI->getParent()->getParent();
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  DebugLoc DL = MI->getDebugLoc();

  Register FrameReg = getFrameRegister(MF);
  int FrameIndex = MI->getOperand(FIOperandNum).getIndex();
  int Offset = TFI->getFrameIndexReference(MF, FrameIndex, FrameReg);
  Offset += MI->getOperand(FIOperandNum + 1).getImm();

  assert(TFI->hasFP(MF) && "eliminateFrameIndex currently requires hasFP");

  // Offsets must be directly encoded in a 12-bit immediate field
  // TODO: Check against actual MINA immediate range
  if (!isInt<12>(Offset)) {
    report_fatal_error(
        "Frame offsets outside of the signed 12-bit range not supported");
  }

  MI->getOperand(FIOperandNum).ChangeToRegister(FrameReg, false);
  MI->getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
}

Register MINA32RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? MINA32::R14 : MINA32::SP;
}

} // end namespace llvm
