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

// Type requirements with target-specific formats.
def SDT_MINA32Call   : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                     SDTCisVT<1, i32>]>;

// Nodes with target-specific formats.
def mina32_ret   : SDNode<"MINA32ISD::RET", SDTNone,
                          [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def mina32_call  : SDNode<"MINA32ISD::CALL", SDT_MINA32Call,
                          [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
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

def immS12W : Operand<i32>, ImmLeaf<i32, [{
  unsigned abs = Imm ^ (Imm >> 31), tmp = Imm;
  int top = Log2_32(abs), end = countTrailingZeros(tmp);
  return top < 28 && end >= 2 && top < end + 11;
}]> {
  let EncoderMethod = "getImmOpValue<2>";
  let DecoderMethod = "decodeImmOperand<2>";
}

def immS12H : Operand<i32>, ImmLeaf<i32, [{
  unsigned abs = Imm ^ (Imm >> 31), tmp = Imm;
  int top = Log2_32(abs), end = countTrailingZeros(tmp);
  return top < 27 && end >= 1 && top < end + 11;
}]> {
  let EncoderMethod = "getImmOpValue<1>";
  let DecoderMethod = "decodeImmOperand<1>";
}

def immS12B : Operand<i32>, ImmLeaf<i32, [{
  unsigned abs = Imm ^ (Imm >> 31), tmp = Imm;
  int top = Log2_32(abs), end = countTrailingZeros(tmp);
  return top < 26 && end >= 0 && top < end + 11;
}]> {
  let EncoderMethod = "getImmOpValue<0>";
  let DecoderMethod = "decodeImmOperand<0>";
}

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

def immL16 : Operand<i32>, ImmLeaf<i32, [{
  return isUInt<16>(Imm);
}]> {
  let EncoderMethod = "getAddrOpValue<0>";
}

def immU16 : Operand<i32>, ImmLeaf<i32, [{
  return isShiftedUInt<16, 16>(Imm);
}], up16x> {
  let EncoderMethod = "getAddrOpValue<0>";
}

def addr24 : Operand<OtherVT> {
  let EncoderMethod = "getAddrOpValue<2>";
  let DecoderMethod = "decodeAddrOperand";
}

// Instruction Templates

class ALUI<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstImmediate<group, opcode,
                          (outs GPR:$DEST), (ins GPR:$SRC1, immS12B:$IMMS),
                          !strconcat(opstr, " $DEST, $SRC1, $IMMS"),
                          [(set GPR:$DEST, (opnode GPR:$SRC1, immS12B:$IMMS))]>;

class ALU<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<group, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, $SRC1, $SRC2"),
                         [(set GPR:$DEST, (opnode GPR:$SRC1, GPR:$SRC2))]>;

class LOADI<bits<4> opcode, string opstr, SDPatternOperator opnode, ImmLeaf off>
    : MINA32InstImmediate<0b0100, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, off:$IMMS),
                         !strconcat(opstr, " $DEST, [$SRC1, $IMMS]"),
                         [(set GPR:$DEST, (opnode (add GPR:$SRC1, off:$IMMS)))]>;

class LOAD<bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<0b0100, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, [$SRC1, $SRC2]"),
                         [(set GPR:$DEST, (opnode (add GPR:$SRC1, GPR:$SRC2)))]>;

class STOREI<bits<4> opcode, string opstr, SDPatternOperator opnode, ImmLeaf off>
    : MINA32InstImmediate<0b0100, opcode,
                         (outs), (ins GPR:$DEST, GPR:$SRC1, off:$IMMS),
                         !strconcat(opstr, " $DEST, [$SRC1, $IMMS]"),
                         [(opnode GPR:$DEST, (add GPR:$SRC1, off:$IMMS))]>;

class STORE<bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<0b0100, opcode,
                         (outs), (ins GPR:$DEST, GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, [$SRC1, $SRC2]"),
                         [(opnode GPR:$DEST, (add GPR:$SRC1, GPR:$SRC2))]>;

let DEST = 0x0 in
class CMPI<bits<4> group, bits<4> opcode, string opstr>
    : MINA32InstImmediate<group, opcode,
                          (outs TF:$MCR), (ins GPR:$SRC1, immS12B:$IMMS),
                          !strconcat(opstr, " $SRC1, $IMMS"), []>;

let DEST = 0x0 in
class CMP<bits<4> group, bits<4> opcode, string opstr>
    : MINA32InstStandard<group, opcode,
                         (outs TF:$MCR), (ins GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $SRC1, $SRC2"), []>;

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

let SRC2 = 0x0 in
def POPCNT : MINA32InstStandard<0b0001, 0b1100,
                                (outs GPR:$DEST), (ins GPR:$SRC1),
                                "popcnt $DEST, $SRC1",
                                [(set GPR:$DEST, (ctpop GPR:$SRC1))]>;

let SRC2 = 0x0 in
def CLO : MINA32InstStandard<0b0001, 0b1101,
                             (outs GPR:$DEST), (ins GPR:$SRC1),
                             "clo $DEST, $SRC1", []>;

let SRC2 = 0x0 in
def PLO : MINA32InstStandard<0b0001, 0b1110,
                             (outs GPR:$DEST), (ins GPR:$SRC1),
                             "plo $DEST, $SRC1", []>;

// Compare Group

def CMPIEQ : CMPI<0b0010, 0b0000, "cmpi.eq">;
def CMPILO : CMPI<0b0010, 0b0001, "cmpi.lo">;
def CMPILS : CMPI<0b0010, 0b0010, "cmpi.ls">;
def CMPILT : CMPI<0b0010, 0b0011, "cmpi.lt">;
def CMPILE : CMPI<0b0010, 0b0100, "cmpi.le">;

def CMPEQ  : CMP<0b0010, 0b1000, "cmp.eq">;
def CMPLO  : CMP<0b0010, 0b1001, "cmp.lo">;
def CMPLS  : CMP<0b0010, 0b1010, "cmp.ls">;
def CMPLT  : CMP<0b0010, 0b1011, "cmp.lt">;
def CMPLE  : CMP<0b0010, 0b1100, "cmp.le">;

// Register Branch Group

let isBranch = 1, isTerminator = 1, DEST = 0x0 in
def RBRA : MINA32InstImmediate<0b0011, 0b0000,
                               (outs), (ins GPR:$SRC1, immS12W:$IMMS),
                               "rbra $SRC1, $IMMS", []>;

let isCall = 1, DEST = 0x0, Defs = [SP] in
def RCALL : MINA32InstImmediate<0b0011, 0b0001,
                                (outs), (ins GPR:$SRC1, immS12W:$IMMS),
                                "rcall $SRC1, $IMMS",
                                [(mina32_call (add GPR:$SRC1, immS12W: $IMMS))]>;

let isReturn = 1, isTerminator = 1, isBarrier = 1,
    IMMS = 0x0, DEST = 0x0, SRC1 = 0x0 in
def RET : MINA32InstImmediate<0b0011, 0b0010,
                              (outs), (ins), "ret", [(mina32_ret)]>;

let isBranch = 1, isTerminator = 1, DEST = 0x0 in
def ROBRA : MINA32InstStandard<0b0011, 0b1000,
                               (outs), (ins GPR:$SRC1, GPR:$SRC2),
                               "robra $SRC1, $SRC2", []>;

let isCall = 1, DEST = 0x0, Defs = [SP] in
def ROCALL : MINA32InstStandard<0b0011, 0b1001,
                                (outs), (ins GPR:$SRC1, GPR:$SRC2),
                                "rocall $SRC1, $SRC2", []>;

// Memory Group

def LD   : LOADI<0b0000, "ld",  load,        immS12W>;
def LDH  : LOADI<0b0001, "ldh", zextloadi16, immS12H>;
def LDB  : LOADI<0b0010, "ldb", zextloadi8,  immS12B>;

def ST   : STOREI<0b0011, "st",  store,         immS12W>;
def STH  : STOREI<0b0100, "sth", truncstorei16, immS12H>;
def STB  : STOREI<0b0101, "stb", truncstorei8,  immS12B>;

let DEST = 0x0, Defs = [MCR] in
def LDC : MINA32InstImmediate<0b0100, 0b0110,
                              (outs), (ins GPR:$SRC1, immS12W:$IMMS),
                              "ldc [$SRC1, $IMMS]", []>;

let DEST = 0x0, Uses = [MCR] in
def STC : MINA32InstImmediate<0b0100, 0b0111,
                              (outs), (ins GPR:$SRC1, immS12W:$IMMS),
                              "stc [$SRC1, $IMMS]", []>;

def RLD  : LOAD<0b1000, "rld",  load>;
def RLDH : LOAD<0b1001, "rldh", zextloadi16>;
def RLDB : LOAD<0b1010, "rldb", zextloadi8>;

def RST  : STORE<0b1011, "rst",  store>;
def RSTH : STORE<0b1100, "rsth", truncstorei16>;
def RSTB : STORE<0b1101, "rstb", truncstorei8>;

let SRC1 = 0x0, SRC2 = 0x0 in
def POP : MINA32InstStandard<0b0100, 0b1110,
                             (outs GPR:$DEST), (ins),
                             "pop $DEST", []>;

let SRC1 = 0x0, SRC2 = 0x0 in
def PUSH : MINA32InstStandard<0b0100, 0b1111,
                              (outs), (ins GPR:$DEST),
                              "push $DEST", []>;

// Move Group

let SRC1 = 0x0 in
def MOVI : MINA32InstImmediate<0b0101, 0b0000,
                               (outs GPR:$DEST), (ins immS12B:$IMMS),
                               "movi $DEST, $IMMS",
                               [(set GPR:$DEST, immS12B:$IMMS)]>;

let Constraints = "$SRC = $DEST", SRC1 = 0x0 in
def MTI  : MINA32InstImmediate<0b0101, 0b0001,
                               (outs GPR:$DEST), (ins TF:$MCR, GPR:$SRC, immS12B:$IMMS),
                               "mti $DEST, $IMMS", []>;

let Constraints = "$SRC = $DEST", SRC1 = 0x0 in
def MFI  : MINA32InstImmediate<0b0101, 0b0010,
                               (outs GPR:$DEST), (ins TF:$MCR, GPR:$SRC, immS12B:$IMMS),
                               "mfi $DEST, $IMMS", []>;

let Constraints = "$SRC = $DEST" in
def MOVL : MINA32InstMove<0b0101, 0b0011,
                          (outs GPR:$DEST), (ins GPR:$SRC, immL16:$IMM),
                          "movl $DEST, $IMM", []>;

def MOVU : MINA32InstMove<0b0101, 0b0100,
                          (outs GPR:$DEST), (ins immU16:$IMM),
                          "movu $DEST, $IMM",
                          [(set GPR:$DEST, immU16:$IMM)]>;

let SRC2 = 0x0 in
def MOV : MINA32InstStandard<0b0101, 0b1000,
                             (outs GPR:$DEST), (ins GPR:$SRC1),
                             "mov $DEST, $SRC1", []>;

let Constraints = "$SRC = $DEST", SRC2 = 0x0 in
def MT  : MINA32InstStandard<0b0101, 0b1001,
                             (outs GPR:$DEST), (ins TF:$MCR, GPR:$SRC, GPR:$SRC1),
                             "mt $DEST, $SRC1", []>;

let Constraints = "$SRC = $DEST", SRC2 = 0x0 in
def MF  : MINA32InstStandard<0b0101, 0b1010,
                             (outs GPR:$DEST), (ins TF:$MCR, GPR:$SRC, GPR:$SRC1),
                             "mf $DEST, $SRC1", []>;

let SRC1 = 0x0, SRC2 = 0x0, Defs = [MCR] in
def MTOC : MINA32InstStandard<0b0101, 0b1011,
                              (outs), (ins GPR:$DEST),
                              "mtoc $DEST", []>;

let SRC1 = 0x0, SRC2 = 0x0, Uses = [MCR] in
def MFRC : MINA32InstStandard<0b0101, 0b1100,
                              (outs GPR:$DEST), (ins),
                              "mfrc $DEST", []>;

let SRC2 = 0x0 in
def MTOU : MINA32InstStandard<0b0101, 0b1101,
                             (outs GPR:$DEST), (ins GPR:$SRC1),
                             "mtou $DEST, $SRC1", []>;

let SRC2 = 0x0 in
def MFRU : MINA32InstStandard<0b0101, 0b1110,
                             (outs GPR:$DEST), (ins GPR:$SRC1),
                             "mfru $DEST, $SRC1", []>;

// Shift Group

def LSL  : ALUI<0b0110, 0b0000, "lsl", shl>;
def LSR  : ALUI<0b0110, 0b0001, "lsr", srl>;
def ASR  : ALUI<0b0110, 0b0010, "asr", sra>;
def ROR  : ALUI<0b0110, 0b0011, "ror", rotr>;

def RLSL : ALU<0b0110, 0b1000, "rlsl", shl>;
def RLSR : ALU<0b0110, 0b1001, "rlsr", srl>;
def RASR : ALU<0b0110, 0b1010, "rasr", sra>;
def RROR : ALU<0b0110, 0b1011, "rror", rotr>;

def FLSL : MINA32InstFunnelShift<0b0110, 0b1100,
                                 (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2, GPR:$RSHIFT),
                                 "flsl $DEST, $SRC1, $SRC2, $RSHIFT",
                                 [(set GPR:$DEST, (fshl GPR:$SRC1, GPR:$SRC2, GPR:$RSHIFT))]>;

def FLSR : MINA32InstFunnelShift<0b0110, 0b1101,
                                 (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2, GPR:$RSHIFT),
                                 "flsr $DEST, $SRC1, $SRC2, $RSHIFT",
                                 [(set GPR:$DEST, (fshr GPR:$SRC1, GPR:$SRC2, GPR:$RSHIFT))]>;

// Control Group

let DEST = 0x0, SRC1 = 0x0, IMMS = 0x0 in {
def STOP   : MINA32InstImmediate<0b0111, 0b0000, (outs), (ins), "stop"  , []>;
def WFI    : MINA32InstImmediate<0b0111, 0b0001, (outs), (ins), "wfi"   , []>;
def SETT   : MINA32InstImmediate<0b0111, 0b0010, (outs), (ins), "sett"  , []>;
def CLRT   : MINA32InstImmediate<0b0111, 0b0011, (outs), (ins), "clrt"  , []>;
def SWITCH : MINA32InstImmediate<0b0111, 0b0100, (outs), (ins), "switch", []>;
} // DEST = 0x0, SRC1 = 0x0, IMMS = 0x0

let SRC1 = 0x0, SRC2 = 0x0 in {
def SVCALL : MINA32InstStandard<0b0111, 0b1000, (outs), (ins GPR:$DEST), "svcall $DEST", []>;
def FAULT  : MINA32InstStandard<0b0111, 0b1001, (outs), (ins GPR:$DEST), "fault $DEST" , []>;
def MTOF   : MINA32InstStandard<0b0111, 0b1010, (outs), (ins GPR:$DEST), "mtof $DEST"  , []>;
def MFRF   : MINA32InstStandard<0b0111, 0b1011, (outs), (ins GPR:$DEST), "mfrf $DEST"  , []>;
} // SRC1 = 0x0, SRC2 = 0x0

// PC-relative Branch Group

let isBranch = 1, isTerminator = 1 in
def BRA : MINA32InstBranch<0b1000, 0b0000,
                           (outs), (ins addr24:$IMM),
                           "bra $IMM", [(br bb:$IMM)]>;

let isBranch = 1, isTerminator = 1 in
def BT  : MINA32InstBranch<0b1000, 0b0001,
                           (outs), (ins TF:$MCR, addr24:$IMM),
                           "bt $IMM", []>;

let isBranch = 1, isTerminator = 1 in
def BF  : MINA32InstBranch<0b1000, 0b0010,
                           (outs), (ins TF:$MCR, addr24:$IMM),
                           "bf $IMM", []>;

let isCall = 1, Defs = [SP] in
def CALL : MINA32InstBranch<0b1000, 0b1000,
                            (outs), (ins addr24:$IMM),
                            "call $IMM", []>;

let isCall = 1, Defs = [SP] in
def CT  : MINA32InstBranch<0b1000, 0b1001,
                           (outs), (ins TF:$MCR, addr24:$IMM),
                           "ct $IMM", []>;

let isCall = 1, Defs = [SP] in
def CF  : MINA32InstBranch<0b1000, 0b1010,
                           (outs), (ins TF:$MCR, addr24:$IMM),
                           "cf $IMM", []>;

// Non-Instruction Patterns

def : Pat<(i32 imm:$IMM), (MOVL (MOVU (up16x imm:$IMM)), (lo16x imm:$IMM))>;
def : Pat<(rotl GPR:$SRC1, immS12B:$IMMS), (ROR GPR:$SRC1, (rol32x imm:$IMMS))>;
def : Pat<(rotl GPR:$SRC1, GPR:$SRC2), (RROR GPR:$SRC1, (MULTI GPR:$SRC2, -1))>;
def : Pat<(ctlz GPR:$SRC1), (CLO (XORI GPR:$SRC1, -1))>;
def : Pat<(mina32_call GPR:$SRC1), (RCALL GPR:$SRC1, 0)>;

def : Pat<(seteq GPR:$SRC1, GPR:$SRC2), (SLTIU (XOR GPR:$SRC1, GPR:$SRC2), 1)>;
def : Pat<(setne GPR:$SRC1, GPR:$SRC2), (XORI (SLTIU (XOR GPR:$SRC1, GPR:$SRC2), 1), 1)>;
def : Pat<(setugt GPR:$SRC1, GPR:$SRC2), (SLTU GPR:$SRC2, GPR:$SRC1)>;
def : Pat<(setuge GPR:$SRC1, GPR:$SRC2), (XORI (SLTU GPR:$SRC1, GPR:$SRC2), 1)>;
def : Pat<(setule GPR:$SRC1, GPR:$SRC2), (XORI (SLTU GPR:$SRC2, GPR:$SRC1), 1)>;
def : Pat<(setgt GPR:$SRC1, GPR:$SRC2), (SLT GPR:$SRC2, GPR:$SRC1)>;
def : Pat<(setge GPR:$SRC1, GPR:$SRC2), (XORI (SLT GPR:$SRC1, GPR:$SRC2), 1)>;
def : Pat<(setle GPR:$SRC1, GPR:$SRC2), (XORI (SLT GPR:$SRC2, GPR:$SRC1), 1)>;

// Memory Patterns

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

// Compare Patterns

multiclass CmpiPat<PatFrag opt, PatFrag opf, MINA32InstImmediate cmpi> {
    def mti : Pat<(select (opt GPR:$SRC1, immS12B:$IMMS), immS12B:$IMMT, GPR:$SRCF),
                  (MT (cmpi GPR:$SRC1, imm:$IMMS), GPR:$SRCF, imm:$IMMT)>;
    def mfi : Pat<(select (opf GPR:$SRC1, immS12B:$IMMS), immS12B:$IMMT, GPR:$SRCF),
                  (MF (cmpi GPR:$SRC1, imm:$IMMS), GPR:$SRCF, imm:$IMMT)>;
    def mt  : Pat<(select (opt GPR:$SRC1, immS12B:$IMMS), GPR:$SRCT, GPR:$SRCF),
                  (MT (cmpi GPR:$SRC1, imm:$IMMS), GPR:$SRCF, GPR:$SRCT)>;
    def mf  : Pat<(select (opf GPR:$SRC1, immS12B:$IMMS), GPR:$SRCT, GPR:$SRCF),
                  (MF (cmpi GPR:$SRC1, imm:$IMMS), GPR:$SRCF, GPR:$SRCT)>;
    def bt  : Pat<(brcond (opt GPR:$SRC1, immS12B:$IMMS), bb:$IMM),
                  (BT (cmpi GPR:$SRC1, imm:$IMMS), addr24:$IMM)>;
    def bf  : Pat<(brcond (opf GPR:$SRC1, immS12B:$IMMS), bb:$IMM),
                  (BF (cmpi GPR:$SRC1, imm:$IMMS), addr24:$IMM)>;
}

multiclass CmpPat<PatFrag opt, PatFrag opf, MINA32InstStandard cmp> {
    def mti : Pat<(select (opt GPR:$SRC1, GPR:$SRC2), immS12B:$IMMT, GPR:$SRCF),
                  (MT (cmp GPR:$SRC1, GPR:$SRC2), GPR:$SRCF, imm:$IMMT)>;
    def mfi : Pat<(select (opf GPR:$SRC1, GPR:$SRC2), immS12B:$IMMT, GPR:$SRCF),
                  (MF (cmp GPR:$SRC1, GPR:$SRC2), GPR:$SRCF, imm:$IMMT)>;
    def mt  : Pat<(select (opt GPR:$SRC1, GPR:$SRC2), GPR:$SRCT, GPR:$SRCF),
                  (MT (cmp GPR:$SRC1, GPR:$SRC2), GPR:$SRCF, GPR:$SRCT)>;
    def mf  : Pat<(select (opf GPR:$SRC1, GPR:$SRC2), GPR:$SRCT, GPR:$SRCF),
                  (MF (cmp GPR:$SRC1, GPR:$SRC2), GPR:$SRCF, GPR:$SRCT)>;
    def bt  : Pat<(brcond (opt GPR:$SRC1, GPR:$SRC2), bb:$IMM),
                  (BT (cmp GPR:$SRC1, GPR:$SRC2), addr24:$IMM)>;
    def bf  : Pat<(brcond (opf GPR:$SRC1, GPR:$SRC2), bb:$IMM),
                  (BF (cmp GPR:$SRC1, GPR:$SRC2), addr24:$IMM)>;
}

defm : CmpiPat<seteq,  setne,  CMPIEQ>;
defm : CmpiPat<setult, setuge, CMPILO>;
defm : CmpiPat<setule, setugt, CMPILS>;
defm : CmpiPat<setlt,  setge,  CMPILT>;
defm : CmpiPat<setle,  setgt,  CMPILE>;

defm : CmpPat<seteq,  setne,  CMPEQ>;
defm : CmpPat<setult, setuge, CMPLO>;
defm : CmpPat<setule, setugt, CMPLS>;
defm : CmpPat<setlt,  setge,  CMPLT>;
defm : CmpPat<setle,  setgt,  CMPLE>;

def : Pat<(select GPR:$SRC1, immS12B:$IMMT, GPR:$SRCF),
          (MF (CMPIEQ GPR:$SRC1, 0), GPR:$SRCF, imm:$IMMT)>;
def : Pat<(select GPR:$SRC1, GPR:$SRCT, GPR:$SRCF),
          (MF (CMPIEQ GPR:$SRC1, 0), GPR:$SRCF, GPR:$SRCT)>;
def : Pat<(brcond GPR:$SRC1, bb:$IMM), (BF (CMPIEQ GPR:$SRC1, 0), bb:$IMM)>;
