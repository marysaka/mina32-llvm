//===-- MINA32ISelDAGToDAG.cpp - A dag to dag inst selector for MINA32 ----===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the MINA32 target.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "MCTargetDesc/MINA32MCTargetDesc.h"
#include "MINA32.h"
#include "MINA32TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "MINA32-isel"

/// Lowers LLVM IR (in DAG form) to MINA32 MC instructions (in DAG form).
class MINA32DAGToDAGISel : public SelectionDAGISel {
public:
  explicit MINA32DAGToDAGISel(MINA32TargetMachine &TM,
                              CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(TM, OptLevel), Subtarget(nullptr) {}

  StringRef getPassName() const override {
    return "MINA32 DAG->DAG Instruction Selection";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  // Include the pieces autogenerated from the target description.
#include "MINA32GenDAGISel.inc"

private:
  const MINA32Subtarget *Subtarget;
  void Select(SDNode *N) override;
  bool trySelect(SDNode *N);
  bool SelectAddrFI(SDValue Addr, SDValue &Base);
  SDValue getImm(const SDNode *Node, unsigned Imm);
};

bool MINA32DAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &MF.getSubtarget<MINA32Subtarget>();

  return SelectionDAGISel::runOnMachineFunction(MF);
}

void MINA32DAGToDAGISel::Select(SDNode *N) {
  // If we have a custom node, we already have selected!
  if (N->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; N->dump(CurDAG); errs() << "\n");
    N->setNodeId(-1);
    return;
  }

  // See if subclasses can handle this node.
  // if (trySelect(N)) ...

  // Select the default instruction.
  SelectCode(N);
}

bool MINA32DAGToDAGISel::SelectAddrFI(SDValue Addr, SDValue &Base) {
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
    return true;
  }
  return false;
}

SDValue MINA32DAGToDAGISel::getImm(const SDNode *Node, unsigned Imm) {
  return CurDAG->getTargetConstant(Imm, SDLoc(Node), MVT::i32);
}

FunctionPass *llvm::createMINA32ISelDag(MINA32TargetMachine &TM,
                                        CodeGenOpt::Level OptLevel) {
  return new MINA32DAGToDAGISel(TM, OptLevel);
}
