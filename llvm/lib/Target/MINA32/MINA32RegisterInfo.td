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
def R0  : MINA32Reg< 0, "r0">, DwarfRegNum<[0]>;
def R1  : MINA32Reg< 1, "r1">, DwarfRegNum<[1]>;
def R2  : MINA32Reg< 2, "r2">, DwarfRegNum<[2]>;
def R3  : MINA32Reg< 3, "r3">, DwarfRegNum<[3]>;
def R4  : MINA32Reg< 4, "r4">, DwarfRegNum<[4]>;
def R5  : MINA32Reg< 5, "r5">, DwarfRegNum<[5]>;
def R6  : MINA32Reg< 6, "r6">, DwarfRegNum<[6]>;
def R7  : MINA32Reg< 7, "r7">, DwarfRegNum<[7]>;
def R8  : MINA32Reg< 8, "r8">, DwarfRegNum<[8]>;
def R9  : MINA32Reg< 9, "r9">, DwarfRegNum<[9]>;
def R10 : MINA32Reg<10, "r10">, DwarfRegNum<[10]>;
def R11 : MINA32Reg<11, "r11">, DwarfRegNum<[11]>;
def R12 : MINA32Reg<12, "r12">, DwarfRegNum<[12]>;
def R13 : MINA32Reg<13, "r13">, DwarfRegNum<[13]>;
def R14 : MINA32Reg<14, "r14">, DwarfRegNum<[14]>;
def SP  : MINA32Reg<15, "sp", [], ["r15"]>, DwarfRegNum<[15]>;

def GPR : RegisterClass<"MINA32", [i32], 32, (add (sequence "R%u", 0, 14), SP)>;

// User registers
def R8_USER  : MINA32Reg< 8, "r8_usr">;
def R9_USER  : MINA32Reg< 9, "r9_usr">;
def R10_USER : MINA32Reg<10, "r10_usr">;
def R11_USER : MINA32Reg<11, "r11_usr">;
def R12_USER : MINA32Reg<12, "r12_usr">;
def R13_USER : MINA32Reg<13, "r13_usr">;
def R14_USER : MINA32Reg<14, "r14_usr">;
def SP_USER  : MINA32Reg<15, "sp_usr", [], ["r15_usr"]>;


// System registers
def MCR  : MINA32Reg<0, "mcr">;
def FRET : MINA32Reg<1, "fret">;

def TF : RegisterClass<"MINA32", [i32], 32, (add MCR)>;

// Supervior only
def SPR  : RegisterClass<"MINA32", [i32], 32, (add (sequence "R%u_USER", 8, 14), SP_USER, MCR, FRET)> {
  let CopyCost = -1;  // Don't allow copying of special registers.
  let isAllocatable = 0;
}
