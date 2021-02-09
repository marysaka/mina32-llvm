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

// Arithmetic/Divide/NOP Group

// TODO rest before

let hasSideEffects = 0, DEST = 0x0, SRC1 = 0x0, SRC2 = 0x0 in
  def NOP : MINA32InstStandard<0b0000, 0b0110, (outs), (ins), "nop", []>;

let hasSideEffects = 0, isCommutable = 1 in
  def ADD : MINA32InstStandard<0b0000, 0b1000,
              (outs GPR:$DEST), (ins GPR:$SRC1, GPR:$SRC2),
              "add $DEST, $SRC1, $SRC2",
              [(set GPR:$DEST, (add GPR:$SRC1, GPR:$SRC2))]>;

// Register Branch Group

let isReturn = 1, isTerminator = 1, isBarrier = 1,
    IMM = 0x0, DEST = 0x0, SHIFT = 0x0, SRC1 = 0x0 in
  def RET : MINA32InstImmediate<0b0011, 0b0010,
              (outs), (ins), "ret", [(mina32_ret)]>;

// TODO rest after
