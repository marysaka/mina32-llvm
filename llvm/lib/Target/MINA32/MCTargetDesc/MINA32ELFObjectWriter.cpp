//===-- MINA32ELFObjectWriter.cpp - MINA32 ELF Writer ---------------------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

#include "MCTargetDesc/MINA32FixupKinds.h"
#include "MCTargetDesc/MINA32MCTargetDesc.h"

namespace llvm {

/// Writes MINA32 machine code into an ELF32 object file.
class MINA32ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  MINA32ELFObjectWriter(uint8_t OSABI);

  virtual ~MINA32ELFObjectWriter() {}

  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};

MINA32ELFObjectWriter::MINA32ELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_MINA32, true) {}

unsigned MINA32ELFObjectWriter::getRelocType(MCContext &Ctx,
                                             const MCValue &Target,
                                             const MCFixup &Fixup,
                                             bool IsPCRel) const {
  // TODO
  llvm_unreachable("getRelocType() unimplemented");
  return 0;
}

std::unique_ptr<MCObjectTargetWriter>
createMINA32ELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<MINA32ELFObjectWriter>(OSABI);
}

} // end namespace llvm
