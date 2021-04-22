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
#include "MINA32MachineFunctionInfo.h"

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

// Determines the size of the frame and maximum call frame size.
void MINA32FrameLowering::determineFrameLayout(MachineFunction &MF) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const MINA32Subtarget &Subtarget = MF.getSubtarget<MINA32Subtarget>();
  const MINA32RegisterInfo *RegInfo = Subtarget.getRegisterInfo();

  // Get the number of bytes to allocate from the FrameInfo.
  unsigned FrameSize = MFI.getStackSize();

  // Get the alignment.
  Align StackAlign = getStackAlign();
  if (RegInfo->needsStackRealignment(MF)) {
    Align MaxStackAlign = std::max(StackAlign, MFI.getMaxAlign());
    FrameSize += (MaxStackAlign.value() - StackAlign.value());
    StackAlign = MaxStackAlign;
  }

  // Get the maximum call frame size of all the calls.
  unsigned MaxCallFrameSize = MFI.getMaxCallFrameSize();

  // If we have dynamic alloca then MaxCallFrameSize needs to be aligned so
  // that allocations will be aligned.
  if (MFI.hasVarSizedObjects()) {
    MaxCallFrameSize = alignTo(MaxCallFrameSize, StackAlign);
    MFI.setMaxCallFrameSize(MaxCallFrameSize);
  }

  // Include call frame size in total.
  if (!(hasReservedCallFrame(MF) && MFI.adjustsStack()))
    FrameSize += MaxCallFrameSize;

  // Make sure the frame is aligned.
  FrameSize = alignTo(FrameSize, StackAlign);
  MFI.setStackSize(FrameSize);
}

void MINA32FrameLowering::adjustReg(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    const DebugLoc &DL, unsigned DestReg,
                                    unsigned SrcReg, int64_t Val,
                                    MachineInstr::MIFlag Flag) const {
  const MINA32Subtarget &Subtarget =
    MBB.getParent()->getSubtarget<MINA32Subtarget>();
  const MINA32InstrInfo *TII = Subtarget.getInstrInfo();

  if (DestReg == SrcReg && Val == 0)
    return;

  // TODO: Check against actual MINA immediate range
  if (!isInt<12>(Val)) {
    report_fatal_error("adjustReg cannot yet handle adjustments >12 bits");
  }

  BuildMI(MBB, MBBI, DL, TII->get(MINA32::ADDI), DestReg)
      .addReg(SrcReg).addImm(Val).setMIFlag(Flag);
}

void MINA32FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

  if (!hasFP(MF)) {
    report_fatal_error(
        "emitPrologue doesn't support framepointer-less functions");
  }

  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();

  Register FrameReg = MINA32::R14;
  Register StackReg = MINA32::SP;

  // Debug location must be unknown since the first debug location is used
  // to determine the end of the prologue.
  DebugLoc DL;

  // Determine the correct frame layout
  determineFrameLayout(MF);

  // FIXME (note copied from Lanai): This appears to be overallocating.  Needs
  // investigation. Get the number of bytes to allocate from the FrameInfo.
  const auto *FuncInfo = MF.getInfo<MINA32MachineFunctionInfo>();
  int64_t StackSize = MFI.getStackSize();
  int64_t FPOffset = StackSize - FuncInfo->getVarArgsSaveSize();

  // Early exit if there is no need to allocate on the stack
  if (StackSize == 0 && !MFI.adjustsStack())
    return;

  // Allocate space on the stack if necessary.
  MachineInstr::MIFlag FrameSetup = MachineInstr::FrameSetup;
  adjustReg(MBB, MBBI, DL, StackReg, StackReg, -StackSize, FrameSetup);

  // The frame pointer is callee-saved, and code has been generated for us to
  // save it to the stack. We need to skip over the storing of callee-saved
  // registers as the frame pointer must be modified after it has been saved
  // to the stack, not before.
  // FIXME: assumes exactly one instruction is used to save each callee-saved
  // register.
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  std::advance(MBBI, CSI.size());

  // Generate new FP.
  adjustReg(MBB, MBBI, DL, FrameReg, StackReg, FPOffset, FrameSetup);
}

void MINA32FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  if (!hasFP(MF)) {
    report_fatal_error(
        "emitEpilogue doesn't support framepointer-less functions");
  }

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const MINA32Subtarget &Subtarget = MF.getSubtarget<MINA32Subtarget>();
  const MINA32RegisterInfo *RegInfo = Subtarget.getRegisterInfo();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  DebugLoc DL = MBBI->getDebugLoc();

  Register FrameReg = MINA32::R14;
  Register StackReg = MINA32::SP;

  // Skip to before the restores of callee-saved registers
  // FIXME: assumes exactly one instruction is used to restore each
  // callee-saved register.
  MachineBasicBlock::iterator LastFrameDestroy = MBBI;
  std::advance(LastFrameDestroy, -MFI.getCalleeSavedInfo().size());

  const auto *FuncInfo = MF.getInfo<MINA32MachineFunctionInfo>();
  int64_t StackSize = MFI.getStackSize();
  int64_t FPOffset = StackSize - FuncInfo->getVarArgsSaveSize();

  // Restore the stack pointer using the value of the frame pointer. Only
  // necessary if the stack pointer was modified, meaning the stack size is
  // unknown.
  if (RegInfo->needsStackRealignment(MF) || MFI.hasVarSizedObjects()) {
    adjustReg(MBB, LastFrameDestroy, DL, StackReg, FrameReg, -FPOffset,
              MachineInstr::FrameDestroy);
  }

  // Deallocate stack
  MachineInstr::MIFlag FrameDestroy = MachineInstr::FrameDestroy;
  adjustReg(MBB, MBBI, DL, StackReg, StackReg, StackSize, FrameDestroy);
}

bool MINA32FrameLowering::hasFP(const MachineFunction &MF) const {
  // return MF.getFrameInfo().hasVarSizedObjects();
  return true;
}

bool MINA32FrameLowering::isFPCloseToIncomingSP() const { return false; }

void MINA32FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                               BitVector &SavedRegs,
                                               RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  // TODO: Once frame pointer elimination is implemented, don't
  // unconditionally spill the frame pointer
  SavedRegs.set(MINA32::R14);
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
  const auto *FuncInfo = MF.getInfo<MINA32MachineFunctionInfo>();

  // Callee-saved registers should be referenced relative to the stack
  // pointer (positive offset), otherwise use the frame pointer (negative
  // offset).
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  int MinCSFI = 0, MaxCSFI = -1;

  int Offset = MFI.getObjectOffset(FI) - getOffsetOfLocalArea() +
               MFI.getOffsetAdjustment();

  if (CSI.size()) {
    MinCSFI = CSI[0].getFrameIdx();
    MaxCSFI = CSI[CSI.size() - 1].getFrameIdx();
  }

  if (FI >= MinCSFI && FI <= MaxCSFI) {
    FrameReg = MINA32::SP;
    Offset += MF.getFrameInfo().getStackSize();
  } else {
    FrameReg = RegInfo->getFrameRegister(MF);
    Offset += FuncInfo->getVarArgsSaveSize();
  }
  return Offset;
}

} // end namespace llvm
