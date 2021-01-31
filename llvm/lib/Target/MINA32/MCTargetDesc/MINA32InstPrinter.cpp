//===-- MINA32InstPrinter.cpp - Convert MINA32 MCInst to assembly syntax --===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an MINA32 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "MINA32InstPrinter.h"

#include <cstring>

#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

#include "MCTargetDesc/MINA32MCTargetDesc.h"

#define DEBUG_TYPE "asm-printer"

namespace llvm {

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "MINA32GenAsmWriter.inc"

void MINA32InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                  StringRef Annot, const MCSubtargetInfo &STI,
                                  raw_ostream &O) {
  unsigned Opcode = MI->getOpcode();

  // TODO
  llvm_unreachable("printInst() unimplemented");
}

void MINA32InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) {
  // TODO
  llvm_unreachable("printOperand() unimplemented");
}

void MINA32InstPrinter::printPCRelImm(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  // TODO
  llvm_unreachable("printPCRelImm() unimplemented");
}

} // end namespace llvm
