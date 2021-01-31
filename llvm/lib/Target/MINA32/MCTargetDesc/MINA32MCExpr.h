//===-- MINA32MCExpr.h - MINA32-specific MC expression classes --*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_MCEXPR_H
#define LLVM_TARGET_MINA32_MCEXPR_H

#include "llvm/MC/MCExpr.h"

#include "MCTargetDesc/MINA32FixupKinds.h"

namespace llvm {

/// An expression in MINA32 machine code.
class MINA32MCExpr : public MCTargetExpr {
public:
  /// Specifies the type of an expression.
  enum VariantKind {
    VK_MINA32_None,
    // TODO
  };

public:
  /// Creates an MINA32 machine code expression.
  static const MINA32MCExpr *create(VariantKind Kind, const MCExpr *Expr,
                                    MCContext &Ctx);

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCFragment *findAssociatedFragment() const override;
  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

private:
  const VariantKind Kind;
  const MCExpr *Expr;

  explicit MINA32MCExpr(VariantKind Kind, const MCExpr *Expr)
      : Kind(Kind), Expr(Expr) {}

  ~MINA32MCExpr() {}
};

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_MCEXPR_H
