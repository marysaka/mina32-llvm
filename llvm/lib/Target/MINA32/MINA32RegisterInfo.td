//===-- MINA32RegisterInfo.td - MINA32 Register defs -------*- tablegen -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the MINA32 register file
//===----------------------------------------------------------------------===//


class MINA32Reg<bits<16> Enc, string n, list<Register> subregs = [],
             list<string> altNames = []> : Register<n, altNames> {
  let HWEncoding = Enc;
  let Namespace = "MINA32";
  let SubRegs = subregs;
}

// All modes registers
def R0  : MINA32Reg< 0, "r0">;
def R1  : MINA32Reg< 1, "r1">;
def R2  : MINA32Reg< 2, "r2">;
def R3  : MINA32Reg< 3, "r3">;
def R4  : MINA32Reg< 4, "r4">;
def R5  : MINA32Reg< 5, "r5">;
def R6  : MINA32Reg< 6, "r6">;
def R7  : MINA32Reg< 7, "r7">;
def R8  : MINA32Reg< 8, "r8">;
def R9  : MINA32Reg< 9, "r9">;
def R10 : MINA32Reg<10, "r10">;
def R11 : MINA32Reg<11, "r11">;
def R12 : MINA32Reg<12, "r12">;
def SP  : MINA32Reg<13, "sp", [], ["r13"]>;
def LR  : MINA32Reg<14, "lr", [], ["r14"]>;
def PC  : MINA32Reg<15, "pc", [], ["r15"]>;

def GPR : RegisterClass<"MINA32", [i32], 32, (add (sequence "R%u", 0, 12), SP, LR, PC)>;

// User registers
def R8_USER  : MINA32Reg< 8, "r8_usr">;
def R9_USER  : MINA32Reg< 9, "r9_usr">;
def R10_USER : MINA32Reg<10, "r10_usr">;
def R11_USER : MINA32Reg<11, "r11_usr">;
def R12_USER : MINA32Reg<12, "r12_usr">;
def SP_USER  : MINA32Reg<13, "sp_usr", [], ["r13_usr"]>;
def LR_USER  : MINA32Reg<14, "lr_usr", [], ["r14_usr"]>;
def PC_USER  : MINA32Reg<15, "pc_usr", [], ["r15_usr"]>;


// System registers
def MCR  : MINA32Reg<0, "mcr">;
def FRET : MINA32Reg<1, "fret">;

def TF : RegisterClass<"MINA32", [i32], 32, (add MCR)>;

// Supervior only
def SPR  : RegisterClass<"MINA32", [i32], 32, (add (sequence "R%u_USER", 8, 12), SP_USER, LR_USER, PC_USER, MCR, FRET)> {
  let CopyCost = -1;  // Don't allow copying of special registers.
  let isAllocatable = 0;
}
