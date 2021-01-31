//===-- MINA32MCTargetDesc.h - MINA32 Target Descriptions -------*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides MINA32-specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_MCTARGET_DESC_H
#define LLVM_TARGET_MINA32_MCTARGET_DESC_H

#include <memory>

#include "llvm/Support/DataTypes.h"

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCInstrInfo *createMINA32MCInstrInfo();

/// Creates a machine code emitter for MINA32.
MCCodeEmitter *createMINA32MCCodeEmitter(const MCInstrInfo &MCII,
                                         const MCRegisterInfo &MRI,
                                         MCContext &Ctx);

/// Creates an assembly backend for MINA32.
MCAsmBackend *createMINA32AsmBackend(const Target &T,
                                     const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const llvm::MCTargetOptions &TO);

/// Creates an ELF object writer for MINA32.
std::unique_ptr<MCObjectTargetWriter>
createMINA32ELFObjectWriter(uint8_t OSABI);

} // end namespace llvm

#define GET_REGINFO_ENUM
#include "MINA32GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "MINA32GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "MINA32GenSubtargetInfo.inc"

#endif // LLVM_TARGET_MINA32_MCTARGET_DESC_H
