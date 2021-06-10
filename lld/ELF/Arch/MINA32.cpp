//===- MINA32.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "SyntheticSections.h"
#include "Target.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {

class MINA32 final : public TargetInfo {
public:
  MINA32();
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
};

} // end anonymous namespace

MINA32::MINA32() { noneRel = R_RISCV_NONE; }

RelExpr MINA32::getRelExpr(const RelType type, const Symbol &s,
                           const uint8_t *loc) const {
  switch (type) {
    case R_MINA32_NONE:
    return R_NONE;
  case R_MINA32_32:
  case R_MINA32_HI16:
  case R_MINA32_LO16:
    return R_ABS;
  case R_MINA32_BRA:
    return R_PC;
  default:
    error(getErrorLocation(loc) + "unknown relocation (" + Twine(type) +
          ") against symbol " + toString(s));
    return R_NONE;
  }
}

void MINA32::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch (rel.type) {
  case R_MINA32_32:
    write32le(loc, val);
    return;

  case R_MINA32_HI16: {
    uint32_t insn = read32le(loc) & 0xfff0f000;
    uint16_t hi = (val >> 28) & 0xf;
    uint16_t lo = (val >> 16) & 0xfff;
    write32le(loc, insn | (hi << 16) | lo);
    return;
  }

  case R_MINA32_LO16: {
    uint32_t insn = read32le(loc) & 0xfff0f000;
    uint16_t hi = (val >> 12) & 0xf;
    uint16_t lo = (val >>  0) & 0xfff;
    write32le(loc, insn | (hi << 16) | lo);
    return;
  }

  case R_MINA32_BRA: {
    checkInt(loc, val, 26, rel);
    checkAlignment(loc, val, 4, rel);
    uint32_t insn = read32le(loc) & 0xff000000;
    uint32_t imm = (val >> 2) & 0xffffff;
    write32le(loc, insn | imm);
    return;
  }

  default:
    llvm_unreachable("unknown relocation");
  }
}

TargetInfo *elf::getMINA32TargetInfo() {
  static MINA32 target;
  return &target;
}
