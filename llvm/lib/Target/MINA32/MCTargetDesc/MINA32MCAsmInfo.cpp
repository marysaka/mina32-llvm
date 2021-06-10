//===-- MINA32MCAsmInfo.cpp - MINA32 asm properties -----------------------===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the MINA32MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "MINA32MCAsmInfo.h"

#include "llvm/ADT/Triple.h"

namespace llvm {

MINA32MCAsmInfo::MINA32MCAsmInfo(const Triple &TT,
                                 const MCTargetOptions &Options) {
  Data16bitsDirective = "\t.half\t";
  Data32bitsDirective = "\t.word\t";
  Data64bitsDirective = nullptr;

  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";

  CommentString = ";";
  SupportsDebugInformation = true;
  UseIntegratedAssembler = true;
}

} // end namespace llvm
