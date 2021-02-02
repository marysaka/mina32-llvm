//===-- MINA32AsmBackend.cpp - MINA32 Assembler Backend -------------------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the MINA32AsmBackend class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MINA32AsmBackend.h"

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "MCTargetDesc/MINA32FixupKinds.h"
#include "MCTargetDesc/MINA32MCTargetDesc.h"

namespace adjust {

using namespace llvm;

static void assertUnsignedWidth(unsigned Width, uint64_t Value,
                                std::string Description, const MCFixup &Fixup,
                                MCContext *Ctx = nullptr) {
  if (!isUIntN(Width, Value)) {
    std::string Diagnostic = "out of range " + Description;

    int64_t Max = maxUIntN(Width);

    Diagnostic +=
        " (expected an integer in the range 0 to " + std::to_string(Max) + ")";

    if (Ctx) {
      Ctx->reportFatalError(Fixup.getLoc(), Diagnostic);
    } else {
      llvm_unreachable(Diagnostic.c_str());
    }
  }
}

} // end namespace adjust

namespace llvm {

void MINA32AsmBackend::adjustFixupValue(const MCFixup &Fixup,
                                        const MCValue &Target, uint64_t &Value,
                                        MCContext *Ctx) const {
  // The size of the fixup in bits.
  // uint64_t Size =
  //     MINA32AsmBackend::getFixupKindInfo(Fixup.getKind()).TargetSize;

  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("unhandled fixup");
  case MINA32::fixup_u8:
    adjust::assertUnsignedWidth(8, Value, "immediate", Fixup, Ctx);
  }
}

std::unique_ptr<MCObjectTargetWriter>
MINA32AsmBackend::createObjectTargetWriter() const {
  return createMINA32ELFObjectWriter(0);
}

void MINA32AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                  const MCValue &Target,
                                  MutableArrayRef<char> Data, uint64_t Value,
                                  bool IsResolved,
                                  const MCSubtargetInfo *STI) const {
  adjustFixupValue(Fixup, Target, Value, &Asm.getContext());
  if (!Value)
    return; // Doesn't change encoding.

  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  // The number of bits in the fixup mask.
  auto NumBits = Info.TargetSize + Info.TargetOffset;
  auto NumBytes = (NumBits / 8) + ((NumBits % 8) == 0 ? 0 : 1);

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  unsigned Offset = Fixup.getOffset();
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the
  // bits from the fixup value.
  for (unsigned i = 0; i < NumBytes; ++i) {
    uint8_t mask = ((Value >> (i * 8)) & 0xFF);
    Data[Offset + i] |= mask;
  }
}

const MCFixupKindInfo &
MINA32AsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[MINA32::NumTargetFixupKinds] = {
      // This table *must* be in same the order of fixup_* kinds in
      // MINA32FixupKinds.h
      //
      // name    offset  bits  flags
      {"fixup_u8", 0, 8, 0},
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");

  return Infos[Kind - FirstTargetFixupKind];
}

bool MINA32AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
  // The MINA32 does not have a NOP. That's because the code segment does not
  // enforce alignment rules for instructions fetches. As a result of that,
  // we won't ever find ourselves having gaps in instruction streams and hence
  // won't be needed to do any sort of padding through NOP constructions.
  return false;
}

bool MINA32AsmBackend::shouldForceRelocation(const MCAssembler &Asm,
                                             const MCFixup &Fixup,
                                             const MCValue &Target) {
  switch ((unsigned)Fixup.getKind()) {
  default:
    return false;
    // Fixups which should always be recorded as relocations.
    // TODO
  }
}

MCAsmBackend *createMINA32AsmBackend(const Target &T,
                                     const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const llvm::MCTargetOptions &TO) {
  return new MINA32AsmBackend();
}

} // end namespace llvm
