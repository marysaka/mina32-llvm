//===-- MINA32ISelLowering.h - MINA32 DAG Lowering Interface ----*- C++ -*-===//
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

#ifndef LLVM_TARGET_MINA32_ISEL_LOWERING_H
#define LLVM_TARGET_MINA32_ISEL_LOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace MINA32ISD {

/// MINA32-specific DAG Nodes.
enum NodeType {
  // Start the numbering where ISD NodeType finishes
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET, // subroutine return instruction
  CALL // subroutine call instruction
};

} // end namespace MINA32ISD

class MINA32Subtarget;
class MINA32TargetMachine;

/// Performs target lowering for the MINA32.
class MINA32TargetLowering : public TargetLowering {
public:
  explicit MINA32TargetLowering(const TargetMachine &TM,
                                const MINA32Subtarget &STI);

  const char *getTargetNodeName(unsigned Opcode) const override;

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *MBB) const override;

protected:
  const MINA32Subtarget &Subtarget;

private:
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerExternalSymbol(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals,
                      const SDLoc &dl, SelectionDAG &DAG) const override;
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_ISEL_LOWERING_H
