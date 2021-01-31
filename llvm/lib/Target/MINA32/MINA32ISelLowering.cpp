//===-- MINA32ISelLowering.cpp - MINA32 DAG Lowering Implementation -------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that MINA32 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "MINA32ISelLowering.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"

#include "MINA32.h"
//#include "MINA32MachineFunctionInfo.h"
#include "MINA32Subtarget.h"
#include "MINA32TargetMachine.h"
#include "MCTargetDesc/MINA32MCTargetDesc.h"

namespace llvm {

MINA32TargetLowering::MINA32TargetLowering(const TargetMachine &TM,
                                           const MINA32Subtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
  // Set up the register classes.
  addRegisterClass(MVT::i32, &MINA32::GPRRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(Subtarget.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(MINA32::SP);
}

const char *MINA32TargetLowering::getTargetNodeName(unsigned Opcode) const {
#define NODE(name)                                                             \
  case MINA32ISD::name:                                                        \
    return #name

  switch (Opcode) {
  default:
    return nullptr;
    // TODO: Populate this.
#undef NODE
  }
}

SDValue MINA32TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  // TODO
  llvm_unreachable("LowerOperation() unimplemented");
}

MachineBasicBlock *MINA32TargetLowering::EmitInstrWithCustomInserter(
    MachineInstr &MI, MachineBasicBlock *MBB) const {
  // TODO
  llvm_unreachable("EmitInstrWithCustomInserter() unimplemented");
}

} // end namespace llvm
