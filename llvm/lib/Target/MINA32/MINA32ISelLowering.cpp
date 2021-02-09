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
#include "MCTargetDesc/MINA32MCTargetDesc.h"
#include "MINA32Subtarget.h"
#include "MINA32TargetMachine.h"

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
  switch (Opcode) {
  default:              return nullptr;
  case MINA32ISD::RET:  return "MINA32ISD::RET";
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

#include "MINA32GenCallingConv.inc"

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

// Transform physical registers into virtual registers.
SDValue MINA32TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_MINA32);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue;

    if (VA.isRegLoc()) {
      Register VReg = RegInfo.createVirtualRegister(&MINA32::GPRRegClass);
      RegInfo.addLiveIn(VA.getLocReg(), VReg);
      ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, VA.getLocVT());
    } else {
      assert(VA.isMemLoc());  // sanity check
      llvm_unreachable("Lowering formal arguments from MemLoc unimplemented");
      // create frame index object for parameter
      /*int FI = MFI.CreateFixedObject(VA.getValVT().getSizeInBits() / 8,
                                     VA.getLocMemOffset(), true);
      // create SelectionDAG nodes for load from this parameter
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
      ArgValue = DAG.getLoad(VA.getLocVT(), DL, Chain, FIN,
                             MachinePointerInfo::getFixedStack(MF, FI));*/
    }
    InVals.push_back(ArgValue);
  }

  if (isVarArg) {
    llvm_unreachable("Lowering VarArgs unimplemented");
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

/*bool MINA32TargetLowering::
CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
               bool isVarArg,
               const SmallVectorImpl<ISD::OutputArg> &Outs,
               LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, MINA32_CRetConv);
}*/

SDValue
MINA32TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                  bool isVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  const SDLoc &DL, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();

  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_MINA32);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);

    // Guarantee that all emitted copies are stuck together.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // Update chain, add glue node
  RetOps[0] = Chain;
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  return DAG.getNode(MINA32ISD::RET, DL, MVT::Other, RetOps);
}

} // end namespace llvm
