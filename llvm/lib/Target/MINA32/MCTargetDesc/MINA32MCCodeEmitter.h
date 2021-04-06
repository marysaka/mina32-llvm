//=-- MINA32MCCodeEmitter.h - Convert MINA32 Code to Machine Code -*- C++ -*-=//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the MINA32MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_CODE_EMITTER_H
#define LLVM_TARGET_MINA32_CODE_EMITTER_H

#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/DataTypes.h"

#include "MINA32FixupKinds.h"

#define GET_INSTRINFO_OPERAND_TYPES_ENUM
#include "MINA32GenInstrInfo.inc"

namespace llvm {

class MCContext;
class MCExpr;
class MCFixup;
class MCInst;
class MCInstrInfo;
class MCOperand;
class MCSubtargetInfo;
class raw_ostream;

/// Writes MINA32 machine code to a stream.
class MINA32MCCodeEmitter : public MCCodeEmitter {
public:
  MINA32MCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx)
      : MCII(MCII), Ctx(Ctx) {}

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

private:
  const MCInstrInfo &MCII;
  MCContext &Ctx;

  MINA32MCCodeEmitter(const MINA32MCCodeEmitter &) = delete;
  void operator=(const MINA32MCCodeEmitter &) = delete;

  FeatureBitset computeAvailableFeatures(const FeatureBitset &FB) const;
  void
  verifyInstructionPredicates(const MCInst &MI,
                              const FeatureBitset &AvailableFeatures) const;

  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  template <unsigned N>
  unsigned getImmOpValue(const MCInst &MI, unsigned OpNo,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_CODE_EMITTER_H
