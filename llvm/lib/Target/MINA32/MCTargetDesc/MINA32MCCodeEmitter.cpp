//===-- MINA32MCCodeEmitter.cpp - Convert MINA32 Code to Machine Code -----===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the MINA32MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MINA32MCCodeEmitter.h"
#include "MINA32InstrInfo.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted.");

#define GET_INSTRMAP_INFO
#include "MINA32GenInstrInfo.inc"
#undef GET_INSTRMAP_INFO

using namespace llvm;

void MINA32MCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  verifyInstructionPredicates(MI,
                              computeAvailableFeatures(STI.getFeatureBits()));

  uint64_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);
  support::endian::write<uint32_t>(OS, Binary, support::little);
  ++MCNumEmitted; // Keep track of the # of mi's emitted.
}

unsigned
MINA32MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  llvm_unreachable("Unhandled expression!");
  return 0;
}

template <unsigned N> unsigned
MINA32MCCodeEmitter::getImmOpValue(const MCInst &MI, unsigned OpNo,
                                   SmallVectorImpl<MCFixup> &Fixups,
                                   const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (MO.isImm()) {
    int Res = MO.getImm();
    int Abs = Res ^ (Res >> 31);

    int Shift = Log2_32(Abs) - N - 10;
    Shift = Shift > 0 ? Shift : 0;
    int Base = (Res >> (Shift + N)) & 0xfff;

    int Enc = SignExtend32<12>(Base) << (Shift + N);
    assert(Shift < 16 && "Unencodable shift");
    assert(Enc == Res && "Unencodable immediate");
    return (Shift << 12) | Base;
  }

  llvm_unreachable("Unhandled expression!");
  return 0;
}

#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "MINA32GenMCCodeEmitter.inc"

// TODO

MCCodeEmitter *llvm::createMINA32MCCodeEmitter(const MCInstrInfo &MCII,
                                               const MCRegisterInfo &MRI,
                                               MCContext &Ctx) {
  return new MINA32MCCodeEmitter(MCII, Ctx);
}
