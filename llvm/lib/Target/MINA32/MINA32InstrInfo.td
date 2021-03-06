//===-- MINA32InstrInfo.td - MINA32 Instruction defs -------*- tablegen -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the MINA32 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "MINA32InstrFormats.td"

// Procedure return
def mina32_ret : SDNode<"MINA32ISD::RET", SDTNone,
                        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                     SDTCisVT<1, i32>]>;

// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

// Pseudo-instructions

// Pessimistically assume the stack pointer will be clobbered
let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_end timm:$amt1, timm:$amt2)]>;
} // Defs = [SP], Uses = [SP]

// Immediate Patterns

def immS12W : ImmLeaf<i32, [{
  unsigned abs = Imm ^ (Imm >> 31), tmp = Imm;
  int top = Log2_32(abs), end = countTrailingZeros(tmp);
  return top < 28 && end >= 2 && top < end + 11;
}]>;
def immS12H : ImmLeaf<i32, [{
  unsigned abs = Imm ^ (Imm >> 31), tmp = Imm;
  int top = Log2_32(abs), end = countTrailingZeros(tmp);
  return top < 27 && end >= 1 && top < end + 11;
}]>;
def immS12B : ImmLeaf<i32, [{
  unsigned abs = Imm ^ (Imm >> 31), tmp = Imm;
  int top = Log2_32(abs), end = countTrailingZeros(tmp);
  return top < 26 && end >= 0 && top < end + 11;
}]>;

// frameindex can't be matched directly
def addrFI : ComplexPattern<iPTR, 1, "SelectAddrFI", [frameindex], []>;

def lo16x : SDNodeXForm<imm, [{
  return getImm(N, (N->getZExtValue() >>  0) & 0xffff);
}]>;
def up16x : SDNodeXForm<imm, [{
  return getImm(N, (N->getZExtValue() >> 16) & 0xffff);
}]>;
def rol32x : SDNodeXForm<imm, [{
  return getImm(N, 32 - N->getZExtValue());
}]>;

def immL16 : ImmLeaf<i32, [{ return isUInt<16>(Imm); }]>;
def immU16 : ImmLeaf<i32, [{ return isShiftedUInt<16, 16>(Imm); }], up16x>;

def addr16 : Operand<OtherVT> {}

// Instruction Templates

class ALUI<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstImmediate<group, opcode,
                          (outs GPR:$DEST), (ins GPR:$SRC1, i32imm:$IMMS),
                          !strconcat(opstr, " $DEST, $SRC1, $IMMS"),
                          [(set GPR:$DEST, (opnode GPR:$SRC1, immS12B:$IMMS))]>;

class ALU<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<group, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, $SRC1, $SRC2"),
                         [(set GPR:$DEST, (opnode GPR:$SRC1, GPR:$SRC2))]>;

class LOADI<bits<4> opcode, string opstr, SDPatternOperator opnode, ImmLeaf off>
    : MINA32InstImmediate<0b0100, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, i32imm:$IMMS),
                         !strconcat(opstr, " $DEST, [$SRC1, $IMMS]"),
                         [(set GPR:$DEST, (opnode (add GPR:$SRC1, off:$IMMS)))]>;

class LOAD<bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<0b0100, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, [$SRC1, $SRC2]"),
                         [(set GPR:$DEST, (opnode (add GPR:$SRC1, GPR:$SRC2)))]>;

class STOREI<bits<4> opcode, string opstr, SDPatternOperator opnode, ImmLeaf off>
    : MINA32InstImmediate<0b0100, opcode,
                         (outs), (ins GPR:$DEST, GPR:$SRC1, i32imm:$IMMS),
                         !strconcat(opstr, " $DEST, [$SRC1, $IMMS]"),
                         [(opnode GPR:$DEST, (add GPR:$SRC1, off:$IMMS))]>;

class STORE<bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<0b0100, opcode,
                         (outs), (ins GPR:$DEST, GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, [$SRC1, $SRC2]"),
                         [(opnode GPR:$DEST, (add GPR:$SRC1, GPR:$SRC2))]>;

class CMPI<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstImmediate<group, opcode,
                          (outs TFR:$MCR), (ins GPR:$DEST, i32imm:$IMMS),
                          !strconcat(opstr, " $DEST, $IMMS"),
                          [(set TFR:$MCR, (opnode GPR:$DEST, immS12B:$IMMS))]>;

class CMP<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<group, opcode,
                         (outs TFR:$MCR), (ins GPR:$DEST, GPR:$SRC1),
                         !strconcat(opstr, " $DEST, $SRC1"),
                         [(set TFR:$MCR, (opnode GPR:$DEST, GPR:$SRC1))]>;

// Arithmetic/Divide/NOP Group

def ADDI  : ALUI<0b0000, 0b0000, "addi",  add>;
def MULTI : ALUI<0b0000, 0b0001, "multi", mul>;
def DIVI  : ALUI<0b0000, 0b0010, "divi",  udiv>;
def REMI  : ALUI<0b0000, 0b0011, "remi",  urem>;
def SLTI  : ALUI<0b0000, 0b0100, "slti",  setlt>;
def SLTIU : ALUI<0b0000, 0b0101, "sltiu", setult>;

let hasSideEffects = 0, DEST = 0x0, SRC1 = 0x0, SRC2 = 0x0 in
def NOP : MINA32InstStandard<0b0000, 0b0110, (outs), (ins), "nop", []>;

def ADD   : ALU<0b0000, 0b1000, "add",  add>;
def MULT  : ALU<0b0000, 0b1001, "mult", mul>;
def DIV   : ALU<0b0000, 0b1010, "div",  udiv>;
def REM   : ALU<0b0000, 0b1011, "rem",  urem>;
def SUB   : ALU<0b0000, 0b1110, "sub",  sub>;
def SLT   : ALU<0b0000, 0b1100, "slt",  setlt>;
def SLTU  : ALU<0b0000, 0b1101, "sltu", setult>;

// Logic/Bitwise Group

def nand : PatFrag<(ops node:$src1, node:$src2),
                   (not (and node:$src1, node:$src2))>;

def ANDI : ALUI<0b0001, 0b0000, "andi", and>;
def ORI  : ALUI<0b0001, 0b0001, "ori",  or>;
def XORI : ALUI<0b0001, 0b0010, "xori", xor>;
def NANDI : ALUI<0b0001, 0b0011, "nandi", nand>;

def AND  : ALU<0b0001, 0b1000, "and", and>;
def OR   : ALU<0b0001, 0b1001, "or",  or>;
def XOR  : ALU<0b0001, 0b1010, "xor", xor>;
def NAND : ALU<0b0001, 0b1011, "nand", nand>;

// Compare Group

def CMPIEQ : CMPI<0b0010, 0b0000, "cmpi/eq", seteq>;
def CMPILO : CMPI<0b0010, 0b0001, "cmpi/lo", setult>;
def CMPILS : CMPI<0b0010, 0b0010, "cmpi/ls", setule>;
def CMPILT : CMPI<0b0010, 0b0011, "cmpi/lt", setlt>;
def CMPILE : CMPI<0b0010, 0b0100, "cmpi/le", setle>;

def CMPEQ  : CMP<0b0010, 0b1000, "cmp/eq", seteq>;
def CMPLO  : CMP<0b0010, 0b1001, "cmp/lo", setult>;
def CMPLS  : CMP<0b0010, 0b1010, "cmp/ls", setule>;
def CMPLT  : CMP<0b0010, 0b1011, "cmp/lt", setlt>;
def CMPLE  : CMP<0b0010, 0b1100, "cmp/le", setle>;

// Register Branch Group

let isReturn = 1, isTerminator = 1, isBarrier = 1,
    IMM = 0x0, DEST = 0x0, SHIFT = 0x0, SRC1 = 0x0 in
def RET : MINA32InstImmediate<0b0011, 0b0010,
                              (outs), (ins), "ret", [(mina32_ret)]>;
// Memory Group

def LD   : LOADI<0b0000, "ld",  load,        immS12W>;
def LDH  : LOADI<0b0001, "ldh", zextloadi16, immS12H>;
def LDB  : LOADI<0b0010, "ldb", zextloadi8,  immS12B>;

def ST   : STOREI<0b0011, "st",  store,         immS12W>;
def STH  : STOREI<0b0100, "sth", truncstorei16, immS12H>;
def STB  : STOREI<0b0101, "stb", truncstorei8,  immS12B>;

def RLD  : LOAD<0b1000, "rld",  load>;
def RLDH : LOAD<0b1001, "rldh", zextloadi16>;
def RLDB : LOAD<0b1010, "rldb", zextloadi8>;

def RST  : STORE<0b1011, "rst",  store>;
def RSTH : STORE<0b1100, "rsth", truncstorei16>;
def RSTB : STORE<0b1101, "rstb", truncstorei8>;

// Move Group

let SRC1 = 0x0 in
def MOVI : MINA32InstImmediate<0b0101, 0b0000,
                               (outs GPR:$DEST), (ins i32imm:$IMMS),
                               "movi $DEST, $IMMS",
                               [(set GPR:$DEST, immS12B:$IMMS)]>;

let Constraints = "$SRC = $DEST" in
def MOVL : MINA32InstMove<0b0101, 0b0011,
                          (outs GPR:$DEST), (ins GPR:$SRC, i32imm:$IMM),
                          "movl $DEST, $IMM", []>;

def MOVU : MINA32InstMove<0b0101, 0b0100,
                          (outs GPR:$DEST), (ins i32imm:$IMM),
                          "movu $DEST, $IMM",
                          [(set GPR:$DEST, immU16:$IMM)]>;


// Shift Group

def LSL  : ALUI<0b0110, 0b0000, "lsl", shl>;
def LSR  : ALUI<0b0110, 0b0001, "lsr", srl>;
def ASR  : ALUI<0b0110, 0b0010, "asr", sra>;
def ROR  : ALUI<0b0110, 0b0011, "ror", rotr>;

def RLSL : ALU<0b0110, 0b1000, "rlsl", shl>;
def RLSR : ALU<0b0110, 0b1001, "rlsr", srl>;
def RASR : ALU<0b0110, 0b1010, "rasr", sra>;
def RROR : ALU<0b0110, 0b1011, "rror", rotr>;

// PC-relative Branch Group

let isBranch = 1, isTerminator = 1 in
def BRA : MINA32InstBranch<0b1000, 0b0000,
                           (outs), (ins addr16:$IMM),
                           "bra $IMM", [(br bb:$IMM)]>;

let isBranch = 1, isTerminator = 1 in
def BT  : MINA32InstBranch<0b1000, 0b0001,
                           (outs), (ins TFR:$MCR, addr16:$IMM),
                           "bt $IMM", [(brcond TFR:$MCR, bb:$IMM)]>;

let isBranch = 1, isTerminator = 1 in
def BF  : MINA32InstBranch<0b1000, 0b0001,
                           (outs), (ins TFR:$MCR, addr16:$IMM),
                           "bf $IMM", [(brcond (not TFR:$MCR), bb:$IMM)]>;

// Non-Instruction Patterns

def : Pat<(i32 imm:$IMM), (MOVL (MOVU (up16x imm:$IMM)), (lo16x imm:$IMM))>;
def : Pat<(rotl GPR:$SRC1, immS12B:$IMMS), (ROR GPR:$SRC1, (rol32x imm:$IMMS))>;
def : Pat<(rotl GPR:$SRC1, GPR:$SRC2), (RROR GPR:$SRC1, (MULTI GPR:$SRC2, -1))>;

def : Pat<(extloadi16 (add GPR:$SRC1, immS12H:$IMMS)), (LDH GPR:$SRC1, imm:$IMMS)>;
def : Pat<(extloadi8  (add GPR:$SRC1, immS12B:$IMMS)), (LDB GPR:$SRC1, imm:$IMMS)>;
def : Pat<(extloadi16 (add GPR:$SRC1, GPR:$SRC2)), (RLDH GPR:$SRC1, GPR:$SRC2)>;
def : Pat<(extloadi8  (add GPR:$SRC1, GPR:$SRC2)), (RLDB GPR:$SRC1, GPR:$SRC2)>;

def : Pat<(load        GPR:$SRC1), (LD  GPR:$SRC1, 0)>;
def : Pat<(zextloadi16 GPR:$SRC1), (LDH GPR:$SRC1, 0)>;
def : Pat<(zextloadi8  GPR:$SRC1), (LDB GPR:$SRC1, 0)>;
def : Pat<(extloadi16  GPR:$SRC1), (LDH GPR:$SRC1, 0)>;
def : Pat<(extloadi8   GPR:$SRC1), (LDB GPR:$SRC1, 0)>;

def : Pat<(load        addrFI:$SRC1), (LD  addrFI:$SRC1, 0)>;
def : Pat<(zextloadi16 addrFI:$SRC1), (LDH addrFI:$SRC1, 0)>;
def : Pat<(zextloadi8  addrFI:$SRC1), (LDB addrFI:$SRC1, 0)>;
def : Pat<(extloadi16  addrFI:$SRC1), (LDH addrFI:$SRC1, 0)>;
def : Pat<(extloadi8   addrFI:$SRC1), (LDB addrFI:$SRC1, 0)>;

def : Pat<(store         GPR:$DEST, GPR:$SRC1), (ST  GPR:$DEST, GPR:$SRC1, 0)>;
def : Pat<(truncstorei16 GPR:$DEST, GPR:$SRC1), (STH GPR:$DEST, GPR:$SRC1, 0)>;
def : Pat<(truncstorei8  GPR:$DEST, GPR:$SRC1), (STB GPR:$DEST, GPR:$SRC1, 0)>;

def : Pat<(store         GPR:$DEST, addrFI:$SRC1), (ST  GPR:$DEST, addrFI:$SRC1, 0)>;
def : Pat<(truncstorei16 GPR:$DEST, addrFI:$SRC1), (STH GPR:$DEST, addrFI:$SRC1, 0)>;
def : Pat<(truncstorei8  GPR:$DEST, addrFI:$SRC1), (STB GPR:$DEST, addrFI:$SRC1, 0)>;
