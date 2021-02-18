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

void MINA32InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  const DebugLoc &DL, MCRegister DstReg,
                                  MCRegister SrcReg, bool KillSrc) const {
  assert(MINA32::GPRRegClass.contains(DstReg, SrcReg) &&
         "Impossible reg-to-reg copy");

  BuildMI(MBB, MBBI, DL, get(MINA32::ADDI), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc)).addImm(0);
}

void MINA32InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          Register SrcReg, bool IsKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();
  assert(MINA32::GPRRegClass.hasSubClassEq(RC) &&
         "Can't store register to stack slot");

  BuildMI(MBB, I, DL, get(MINA32::ST)).addReg(SrcReg, getKillRegState(IsKill))
      .addFrameIndex(FI).addImm(0);
}

void MINA32InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                           MachineBasicBlock::iterator I,
                                           Register DstReg, int FI,
                                           const TargetRegisterClass *RC,
                                           const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();
  assert(MINA32::GPRRegClass.hasSubClassEq(RC) &&
         "Can't load register from stack slot");

  BuildMI(MBB, I, DL, get(MINA32::LD), DstReg).addFrameIndex(FI).addImm(0);
}

} // end namespace llvm
