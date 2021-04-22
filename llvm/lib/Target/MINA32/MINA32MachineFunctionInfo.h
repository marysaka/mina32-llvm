//=- MINA32MachineFunctionInfo.h - MINA32 machine function info ---*- C++ -*-=//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares MINA32-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MINA32_MACHINE_FUNCTION_INFO_H
#define LLVM_LIB_TARGET_MINA32_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// MINA32MachineFunctionInfo - This class is derived from MachineFunctionInfo
/// and contains private MINA32-specific information for each MachineFunction.
class MINA32MachineFunctionInfo : public MachineFunctionInfo {

  /// FrameIndex for start of varargs area
  int VarArgsFrameIndex = 0;
  /// Size of the save area used for varargs
  int VarArgsSaveSize = 0;

public:
  MINA32MachineFunctionInfo() = default;

  explicit MINA32MachineFunctionInfo(MachineFunction &MF) {}

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  unsigned getVarArgsSaveSize() const { return VarArgsSaveSize; }
  void setVarArgsSaveSize(int Size) { VarArgsSaveSize = Size; }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_MINA32_MACHINE_FUNCTION_INFO_H
