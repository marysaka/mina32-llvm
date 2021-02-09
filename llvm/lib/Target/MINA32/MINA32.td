//===-- MINA32.td - Describe the MINA32 Target Machine ----*- tablegen -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===---------------------------------------------------------------------===//
// This is the top level entry point for the MINA32 target.
//===---------------------------------------------------------------------===//

//===---------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===---------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===---------------------------------------------------------------------===//
// Register File, Calling Conv, and Instruction Descriptions
//===---------------------------------------------------------------------===//

include "MINA32RegisterInfo.td"
include "MINA32InstrInfo.td"
include "MINA32CallingConv.td"

def MINA32InstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// MINA32 Subtarget features.
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------===//
// Target Declaration
//===---------------------------------------------------------------------===//

def MINA32 : Target {
  let InstructionSet = MINA32InstrInfo;
}

// One CPU definition as it's needed.
def : ProcessorModel<"generic", NoSchedModel, []>;
