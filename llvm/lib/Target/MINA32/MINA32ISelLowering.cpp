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
#include "MINA32MachineFunctionInfo.h"
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

  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32, Expand);
  setOperationAction(ISD::STACKSAVE, MVT::Other, Expand);
  setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);

  setOperationAction(ISD::VASTART, MVT::Other, Custom);
  setOperationAction(ISD::VAARG, MVT::Other, Expand);
  setOperationAction(ISD::VACOPY, MVT::Other, Expand);
  setOperationAction(ISD::VAEND, MVT::Other, Expand);

  setOperationAction(ISD::BR_JT, MVT::Other, Expand);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  setBooleanContents(ZeroOrOneBooleanContent);
  setStackPointerRegisterToSaveRestore(MINA32::SP);

  setCondCodeAction(ISD::SETNE, MVT::i32, Expand);
  setCondCodeAction(ISD::SETGT, MVT::i32, Expand);
  setCondCodeAction(ISD::SETGE, MVT::i32, Expand);
  setCondCodeAction(ISD::SETUGT, MVT::i32, Expand);
  setCondCodeAction(ISD::SETUGE, MVT::i32, Expand);

  for (auto VT : MVT::integer_valuetypes()) {
    setLoadExtAction(ISD::EXTLOAD,  VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i8,  Expand);
    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i16, Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG, VT, Expand);
  }

  setOperationAction(ISD::ADDC, MVT::i32, Expand);
  setOperationAction(ISD::ADDE, MVT::i32, Expand);
  setOperationAction(ISD::SUBC, MVT::i32, Expand);
  setOperationAction(ISD::SUBE, MVT::i32, Expand);

  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::MULHS, MVT::i32, Expand);
  setOperationAction(ISD::MULHU, MVT::i32, Expand);

  setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::FSHL, MVT::i32, Legal);
  setOperationAction(ISD::FSHR, MVT::i32, Legal);

  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ, MVT::i32, Expand);
  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
  setOperationAction(ISD::BlockAddress, MVT::i32, Custom);

  // Effectively disable jump table generation.
  setMinimumJumpTableEntries(INT_MAX);
}

const char *MINA32TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  default:              return nullptr;
  case MINA32ISD::RET:  return "MINA32ISD::RET";
  case MINA32ISD::CALL: return "MINA32ISD::CALL";
  }
}

SDValue MINA32TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("LowerOperation() unimplemented");
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::BlockAddress:
    return LowerBlockAddress(Op, DAG);
  case ISD::VASTART:
    return LowerVASTART(Op, DAG);
  }
}

MachineBasicBlock *MINA32TargetLowering::EmitInstrWithCustomInserter(
    MachineInstr &MI, MachineBasicBlock *MBB) const {
  // TODO
  llvm_unreachable("EmitInstrWithCustomInserter() unimplemented");
}

SDValue MINA32TargetLowering::LowerGlobalAddress(SDValue Op,
                                                 SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT Ty = Op.getValueType();
  GlobalAddressSDNode *N = cast<GlobalAddressSDNode>(Op);
  const GlobalValue *GV = N->getGlobal();
  int64_t Offset = N->getOffset();

  if (isPositionIndependent()) {
    report_fatal_error("Unable to LowerGlobalAddress");
  }

  SDValue GAHi =
      DAG.getTargetGlobalAddress(GV, DL, Ty, Offset, MINA32II::MO_HI);
  SDValue GALo =
      DAG.getTargetGlobalAddress(GV, DL, Ty, Offset, MINA32II::MO_LO);
  SDValue MNHi = SDValue(DAG.getMachineNode(MINA32::MOVU, DL, Ty, GAHi), 0);
  return SDValue(DAG.getMachineNode(MINA32::MOVL, DL, Ty, MNHi, GALo), 0);
}

SDValue MINA32TargetLowering::LowerBlockAddress(SDValue Op,
                                                SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT Ty = Op.getValueType();
  BlockAddressSDNode *N = cast<BlockAddressSDNode>(Op);
  const BlockAddress *BA = N->getBlockAddress();
  int64_t Offset = N->getOffset();

  if (isPositionIndependent()) {
    report_fatal_error("Unable to LowerBlockAddress");
  }

  SDValue BAHi = DAG.getTargetBlockAddress(BA, Ty, Offset, MINA32II::MO_HI);
  SDValue BALo = DAG.getTargetBlockAddress(BA, Ty, Offset, MINA32II::MO_LO);
  SDValue MNHi = SDValue(DAG.getMachineNode(MINA32::MOVU, DL, Ty, BAHi), 0);
  return SDValue(DAG.getMachineNode(MINA32::MOVL, DL, Ty, MNHi, BALo), 0);
}

SDValue MINA32TargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  auto *FuncInfo = MF.getInfo<MINA32MachineFunctionInfo>();

  SDLoc DL(Op);
  SDValue FI = DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(),
                                 getPointerTy(MF.getDataLayout()));

  // vastart just stores the address of the VarArgsFrameIndex slot into the
  // memory location argument.
  const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), DL, FI, Op.getOperand(1),
                      MachinePointerInfo(SV));
}

SDValue MINA32TargetLowering::LowerExternalSymbol(SDValue Op,
                                                  SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT Ty = Op.getValueType();
  ExternalSymbolSDNode *N = cast<ExternalSymbolSDNode>(Op);
  const char *Sym = N->getSymbol();

  if (isPositionIndependent()) {
    report_fatal_error("Unable to LowerExternalSymbol");
  }

  SDValue ESHi = DAG.getTargetExternalSymbol(Sym, Ty, MINA32II::MO_HI);
  SDValue ESLo = DAG.getTargetExternalSymbol(Sym, Ty, MINA32II::MO_LO);
  SDValue MNHi = SDValue(DAG.getMachineNode(MINA32::MOVU, DL, Ty, ESHi), 0);
  return SDValue(DAG.getMachineNode(MINA32::MOVL, DL, Ty, MNHi, ESLo), 0);
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
    MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RI = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_MINA32);

  if (isVarArg) {
    auto *FuncInfo = MF.getInfo<MINA32MachineFunctionInfo>();
    unsigned Offset = CCInfo.getNextStackOffset();
    int FI = MFI.CreateFixedObject(4, 4 + Offset, true);
    FuncInfo->setVarArgsFrameIndex(FI);
  }

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue;

    if (VA.isRegLoc()) {
      Register VReg = RI.createVirtualRegister(&MINA32::GPRRegClass);
      RI.addLiveIn(VA.getLocReg(), VReg);
      ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, VA.getLocVT());
    } else {
      assert(VA.isMemLoc());  // sanity check
      // create frame index object for parameter
      int FI = MFI.CreateFixedObject(VA.getValVT().getSizeInBits() / 8,
                                     4 + VA.getLocMemOffset(), true);
      // create SelectionDAG nodes for load from this parameter
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
      ArgValue = DAG.getLoad(VA.getLocVT(), DL, Chain, FIN,
                             MachinePointerInfo::getFixedStack(MF, FI));
    }
    InVals.push_back(ArgValue);
  }

  return Chain;
}

SDValue
MINA32TargetLowering::LowerCall(CallLoweringInfo &CLI,
                                SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CLI.IsTailCall = false;
  CallingConv::ID CallConv = CLI.CallConv;
  bool isVarArg = CLI.IsVarArg;
  EVT PtrVT = getPointerTy(DAG.getDataLayout());

  MachineFunction &MF = DAG.getMachineFunction();

  // Analyze the operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState ArgCCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  ArgCCInfo.AnalyzeCallOperands(Outs, CC_MINA32);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = ArgCCInfo.getNextStackOffset();

  // Create local copies for byval args
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    Align Alignment = Flags.getNonZeroByValAlign();

    int FI = MF.getFrameInfo().CreateStackObject(Size, Alignment, false);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue SizeNode = DAG.getConstant(Size, DL, MVT::i32);

    Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Alignment,
                          false, false, false, MachinePointerInfo(),
                          MachinePointerInfo());
    ByValArgs.push_back(FIPtr);
  }

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

  // Copy argument values to their designated locations.
  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue = OutVals[i];
    ISD::ArgFlagsTy Flags = Outs[i].Flags;

    // Promote the value if needed.
    // For now, only handle fully promoted arguments.
    switch (VA.getLocInfo()) {
    case CCValAssign::Full:
      break;
    default:
      llvm_unreachable("Unknown loc info!");
    }

    // Use local copy if it is a byval arg.
    if (Flags.isByVal())
      ArgValue = ByValArgs[j++];

    if (VA.isRegLoc()) {
      // Queue up the argument copies and emit them at the end.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");
      // Work out the address of the stack slot.
      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, MINA32::SP, PtrVT);
      SDValue Address =
          DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                      DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));

      // Emit the store.
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo()));
    }
  }

  // Join the stores, which are independent of one another.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  SDValue Glue;

  // Build a sequence of copy-to-reg nodes, chained and glued together.
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
    Glue = Chain.getValue(1);
  }

  if (isa<GlobalAddressSDNode>(Callee)) {
    Callee = LowerGlobalAddress(Callee, DAG);
  } else if (isa<ExternalSymbolSDNode>(Callee)) {
    Callee = LowerExternalSymbol(Callee, DAG);
  }

  // The first call operand is the chain and the second is the target address.
  SmallVector<SDValue, 4> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  // Glue the call to the argument copies, if any.
  if (Glue.getNode())
    Ops.push_back(Glue);

  // Emit the call.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  Chain = DAG.getNode(MINA32ISD::CALL, DL, NodeTys, Ops);
  Glue = Chain.getValue(1);

  // Mark the end of the call, which is glued to the call itself.
  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getConstant(NumBytes, DL, PtrVT, true),
                             DAG.getConstant(0, DL, PtrVT, true),
                             Glue, DL);
  Glue = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetCCInfo(CallConv, isVarArg, MF, RVLocs, *DAG.getContext());
  RetCCInfo.AnalyzeCallResult(Ins, RetCC_MINA32);

  // Copy all of the result registers out of their specified physreg.
  for (auto &VA : RVLocs) {
    // Copy the value out, gluing the copy to the end of the call sequence.
    SDValue RetValue = DAG.getCopyFromReg(Chain, DL, VA.getLocReg(),
                                          VA.getLocVT(), Glue);
    Chain = RetValue.getValue(1);
    Glue = RetValue.getValue(2);
    InVals.push_back(Chain.getValue(0));
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
