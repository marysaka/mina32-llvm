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
  if (MINA32::GPRRegClass.contains(DstReg, SrcReg)) {
    BuildMI(MBB, MBBI, DL, get(MINA32::MOV), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (MINA32::GPRRegClass.contains(SrcReg) &&
             MINA32::MCR == DstReg) {
    BuildMI(MBB, MBBI, DL, get(MINA32::CMPIEQ))
        .addReg(SrcReg, getKillRegState(KillSrc)).addImm(1);
  } else if (MINA32::GPRRegClass.contains(DstReg) &&
             MINA32::MCR == SrcReg) {
    BuildMI(MBB, MBBI, DL, get(MINA32::MOVI), DstReg).addImm(0);
    BuildMI(MBB, MBBI, DL, get(MINA32::MTI), DstReg)
      .addReg(DstReg, getKillRegState(KillSrc)).addImm(1);
  } else {
    llvm_unreachable("Impossible reg-to-reg copy");
  }
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

// The contents of values added to Cond are not examined outside of
// MINA32InstrInfo, giving us flexibility in what to push to it.
static void parseCondBranch(MachineInstr &LastInst, MachineBasicBlock *&Target,
                            SmallVectorImpl<MachineOperand> &Cond) {
  // Block ends with fall-through condbranch.
  assert(LastInst.getDesc().isConditionalBranch() &&
         "Unknown conditional branch");
  Target = LastInst.getOperand(0).getMBB();
  Cond.push_back(MachineOperand::CreateImm(LastInst.getOpcode()));
}

bool MINA32InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  TBB = FBB = nullptr;
  Cond.clear();

  // If the block has no terminators, it just falls into the block after it.
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end() || !isUnpredicatedTerminator(*I))
    return false;

  // Count the number of terminators and find the first unconditional or
  // indirect branch.
  MachineBasicBlock::iterator FirstUncondOrIndirectBr = MBB.end();
  int NumTerminators = 0;
  for (auto J = I.getReverse(); J != MBB.rend() && isUnpredicatedTerminator(*J);
       J++) {
    NumTerminators++;
    if (J->getDesc().isUnconditionalBranch() ||
        J->getDesc().isIndirectBranch()) {
      FirstUncondOrIndirectBr = J.getReverse();
    }
  }

  // If AllowModify is true, we can erase any terminators after
  // FirstUncondOrIndirectBR.
  if (AllowModify && FirstUncondOrIndirectBr != MBB.end()) {
    while (std::next(FirstUncondOrIndirectBr) != MBB.end()) {
      std::next(FirstUncondOrIndirectBr)->eraseFromParent();
      NumTerminators--;
    }
    I = FirstUncondOrIndirectBr;
  }

  // We can't handle blocks that end in an indirect branch.
  if (I->getDesc().isIndirectBranch())
    return true;

  // We can't handle blocks with more than 2 terminators.
  if (NumTerminators > 2)
    return true;

  // Handle a single unconditional branch.
  if (NumTerminators == 1 && I->getDesc().isUnconditionalBranch()) {
    TBB = I->getOperand(0).getMBB();
    return false;
  }

  // Handle a single conditional branch.
  if (NumTerminators == 1 && I->getDesc().isConditionalBranch()) {
    parseCondBranch(*I, TBB, Cond);
    return false;
  }

  // Handle a conditional branch followed by an unconditional branch.
  if (NumTerminators == 2 && std::prev(I)->getDesc().isConditionalBranch() &&
      I->getDesc().isUnconditionalBranch()) {
    parseCondBranch(*std::prev(I), TBB, Cond);
    FBB = I->getOperand(0).getMBB();
    return false;
  }

  // Otherwise, we can't handle this.
  return true;
}

unsigned MINA32InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                       int *BytesRemoved) const {
  assert(!BytesRemoved && "Code size not handled");
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end())
    return 0;

  if (!I->getDesc().isUnconditionalBranch() &&
      !I->getDesc().isConditionalBranch())
    return 0;

  // Remove the branch.
  I->eraseFromParent();

  I = MBB.end();

  if (I == MBB.begin())
    return 1;
  --I;
  if (!I->getDesc().isConditionalBranch())
    return 1;

  // Remove the branch.
  I->eraseFromParent();
  return 2;
}

// Inserts a branch into the end of the specific MachineBasicBlock, returning
// the number of instructions inserted.
unsigned MINA32InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  assert(!BytesAdded && "Code size not handled.");

  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 1 || Cond.size() == 0) &&
         "MINA32 branch conditions have one component!");

  // Unconditional branch.
  if (Cond.empty()) {
    BuildMI(&MBB, DL, get(MINA32::BRA)).addMBB(TBB);
    return 1;
  }

  // Either a one or two-way conditional branch.
  unsigned Opc = Cond[0].getImm();
  BuildMI(&MBB, DL, get(Opc)).addMBB(TBB);

  // One-way conditional branch.
  if (!FBB)
    return 1;

  // Two-way conditional branch.
  BuildMI(&MBB, DL, get(MINA32::BRA)).addMBB(FBB);
  return 2;
}

bool MINA32InstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  assert((Cond.size() == 1) && "Invalid branch condition!");

  switch (Cond[0].getImm()) {
  default:
    llvm_unreachable("Unrecognized conditional branch");
  case MINA32::BT:
    Cond[0].setImm(MINA32::BF);
    return false;
  case MINA32::BF:
    Cond[0].setImm(MINA32::BT);
    return false;
  }
}
} // end namespace llvm
