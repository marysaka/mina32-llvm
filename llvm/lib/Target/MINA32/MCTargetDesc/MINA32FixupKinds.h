//===-- MINA32FixupKinds.h - MINA32-specific Fixup Entries ------*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_FIXUP_KINDS_H
#define LLVM_TARGET_MINA32_FIXUP_KINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace MINA32 {

// The set of supported fixups.
//
// Although most of the current fixup types reflect a unique relocation
// one can have multiple fixup types for a given relocation and thus need
// to be uniquely named.
//
// \note This table *must* be in the same order of
//       MCFixupKindInfo Infos[MINA32::NumTargetFixupKinds]
//       in `MINA32AsmBackend.cpp`.
enum Fixups {
  // 16-bit fixup corresponding to hi(foo) for MOVU
  fixup_mina32_hi16 = FirstTargetFixupKind,

  // 16-bit fixup corresponding to lo(foo) for MOVL
  fixup_mina32_lo16,

  // 24-bit fixup for PC relative branch
  // instructions such as BRA, CALL, BT, etc.
  fixup_mina32_bra,

  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind,
};
} // end namespace MINA32
} // end namespace llvm

#endif // LLVM_TARGET_MINA32_FIXUP_KINDS_H
