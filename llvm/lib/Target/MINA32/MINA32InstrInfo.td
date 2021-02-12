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

def lo16x : SDNodeXForm<imm, [{
    return getImm(N, (N->getZExtValue() >>  0) & 0xffff);
}]>;

def up16x : SDNodeXForm<imm, [{
    return getImm(N, (N->getZExtValue() >> 16) & 0xffff);
}]>;

def immS12 : ImmLeaf<i32, [{
    unsigned abs = Imm ^ (Imm >> 31);
    int top = Log2_32(abs), end = countTrailingZeros(abs);
    return top < 26 && top < end + 11;
}]>;

def immL16 : ImmLeaf<i32, [{ return isUInt<16>(Imm); }]>;
def immU16 : ImmLeaf<i32, [{ return isShiftedUInt<16, 16>(Imm); }], up16x>;

// Instruction Templates

class ALUI<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstImmediate<group, opcode,
                          (outs GPR:$DEST), (ins GPR:$SRC1, i32imm:$IMMS),
                          !strconcat(opstr, " $DEST, $SRC1, $IMMS"),
                          [(set GPR:$DEST, (opnode GPR:$SRC1, immS12:$IMMS))]>;

class ALU<bits<4> group, bits<4> opcode, string opstr, SDPatternOperator opnode>
    : MINA32InstStandard<group, opcode,
                         (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2),
                         !strconcat(opstr, " $DEST, $SRC1, $SRC2"),
                         [(set GPR:$DEST, (opnode GPR:$SRC1, GPR:$SRC2))]>;

// Arithmetic/Divide/NOP Group

def ADDI  : ALUI<0b0000, 0b0000, "addi", add>;
def MULTI : ALUI<0b0000, 0b0001, "multi", mul>;
def DIVI  : ALUI<0b0000, 0b0010, "divi", sdiv>;
def REMI  : ALUI<0b0000, 0b0011, "remi", srem>;
def SLTI  : ALUI<0b0000, 0b0100, "slti", setlt>;
def SLTIU : ALUI<0b0000, 0b0101, "sltiu", setult>;

let hasSideEffects = 0, DEST = 0x0, SRC1 = 0x0, SRC2 = 0x0 in
def NOP : MINA32InstStandard<0b0000, 0b0110, (outs), (ins), "nop", []>;

def ADD   : ALU<0b0000, 0b1000, "add", add>;
def MULT  : ALU<0b0000, 0b1001, "mult", mul>;
def DIV   : ALU<0b0000, 0b1010, "div", sdiv>;
def REM   : ALU<0b0000, 0b1011, "rem", srem>;
def SLT   : ALU<0b0000, 0b1100, "slt", setlt>;
def SLTU  : ALU<0b0000, 0b1101, "sltu", setult>;
def SUB   : ALU<0b0000, 0b1110, "sub", sub>;

// Logic/Bitwise Group

//def nand : PatFrag<(ops node:$src1, node:$src2),
//                   (not (and node:$src1, node:$src2))>;

def ANDI  : ALUI<0b0001, 0b0000, "andi", and>;
def ORI   : ALUI<0b0001, 0b0001, "ori", or>;
def XORI  : ALUI<0b0001, 0b0010, "xori", xor>;

def AND   : ALU<0b0001, 0b1000, "and", and>;
def OR    : ALU<0b0001, 0b1001, "or", or>;
def XOR   : ALU<0b0001, 0b1010, "xor", xor>;

// Register Branch Group

let isReturn = 1, isTerminator = 1, isBarrier = 1,
    IMM = 0x0, DEST = 0x0, SHIFT = 0x0, SRC1 = 0x0 in
def RET : MINA32InstImmediate<0b0011, 0b0010,
                              (outs), (ins), "ret", [(mina32_ret)]>;
// Memory Group

//def ST : MINA32InstImmediate<0b0100, 0b0011,
//           (outs), (ins GPR:$SRC1, GPR:$DEST),
//           "st $DEST, [$SRC1, $IMM]",
//           [(store GPR:$DEST, (add GPR:$SRC, $DEST

// Move Group

let Constraints = "$SRC = $DEST" in
def MOVL : MINA32InstMove<0b0101, 0b0011,
                          (outs GPR:$DEST), (ins GPR:$SRC, i32imm:$IMM),
                          "movl $DEST, $IMM",
                          [(set GPR:$DEST, (or GPR:$SRC, immL16:$IMM))]>;

def MOVU : MINA32InstMove<0b0101, 0b0011,
                          (outs GPR:$DEST), (ins i32imm:$IMM),
                          "movu $DEST, $IMM",
                          [(set GPR:$DEST, immU16:$IMM)]>;

// Shift Group

def LSL   : ALUI<0b0110, 0b0000, "lsl", shl>;
def LSR   : ALUI<0b0110, 0b0001, "lsr", srl>;
def ASR   : ALUI<0b0110, 0b0010, "asr", sra>;
def ROR   : ALUI<0b0110, 0b0011, "ror", rotr>;

def RLSL  : ALU<0b0110, 0b1000, "rlsl", shl>;
def RLSR  : ALU<0b0110, 0b1001, "rlsr", srl>;
def RASR  : ALU<0b0110, 0b1010, "rasr", sra>;
def RROR  : ALU<0b0110, 0b1011, "rror", rotr>;

// Non-Instruction Patterns

def : Pat<(i32 imm:$IMM), (MOVL (MOVU (up16x imm:$IMM)), (lo16x imm:$IMM))>;
