//===-- MINA32.td - Describe the MINA32 Target Machine ----*- tablegen -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===---------------------------------------------------------------------===//
//
// This describes the calling conventions for MINA32 architecture.
//
//===----------------------------------------------------------------------===//

def CC_MINA32 : CallingConv<[
  // Handle i1, i8, i16 and i32 by passing in registers R0-R3
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>,

  // If more than fit in register, use the stack
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def RetCC_MINA32 : CallingConv<[
  // Return i1, i8, i16 and i32 in registers R0-R3
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>,
]>;

def CSR : CalleeSavedRegs<(add LR, (sequence "R%u", 4, 12))>;
