//===--- MINA32.cpp - Implement MINA32 target feature support ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements MINA32 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "MINA32.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

ArrayRef<const char *> MINA32TargetInfo::getGCCRegNames() const {
  static const char *const GCCRegNames[] = {
      "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
      "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "sp"
  };
  return llvm::makeArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> MINA32TargetInfo::getGCCRegAliases() const {
  static const TargetInfo::GCCRegAlias GCCRegAliases[] = {{{"r15"}, "sp"}};
  return llvm::makeArrayRef(GCCRegAliases);
}

void MINA32TargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("__ELF__");
  Builder.defineMacro("__mina32");
}
