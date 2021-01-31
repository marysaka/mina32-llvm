//===-- MINA32MCTargetDesc.cpp - MINA32 Target Descriptions ---------------===//
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

#include "MCTargetDesc/MINA32MCTargetDesc.h"

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

#include "MCTargetDesc/MINA32InstPrinter.h"
#include "MINA32MCAsmInfo.h"
#include "TargetInfo/MINA32TargetInfo.h"

#define GET_INSTRINFO_MC_DESC
#include "MINA32GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "MINA32GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MINA32GenRegisterInfo.inc"

namespace llvm {

MCInstrInfo *createMINA32MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitMINA32MCInstrInfo(X);

  return X;
}

static MCRegisterInfo *createMINA32MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitMINA32MCRegisterInfo(X, 0);

  return X;
}

static MCSubtargetInfo *
createMINA32MCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createMINA32MCSubtargetInfoImpl(TT, CPU, FS);
}

static MCInstPrinter *createMINA32MCInstPrinter(const Triple &TT,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  if (SyntaxVariant == 0)
    return new MINA32InstPrinter(MAI, MII, MRI);

  return nullptr;
}

static MCStreamer *createMCStreamer(const Triple &TT, MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> &&MAB,
                                    std::unique_ptr<MCObjectWriter> &&OW,
                                    std::unique_ptr<MCCodeEmitter> &&Emitter,
                                    bool RelaxAll) {
  return createELFStreamer(Context, std::move(MAB), std::move(OW),
                           std::move(Emitter), RelaxAll);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMINA32TargetMC() {
  // Register the MC ASM info.
  RegisterMCAsmInfo<MINA32MCAsmInfo> X(getTheMINA32Target());

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheMINA32Target(),
                                      createMINA32MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheMINA32Target(),
                                    createMINA32MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheMINA32Target(),
                                          createMINA32MCSubtargetInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheMINA32Target(),
                                        createMINA32MCInstPrinter);

  // Register the MC Code Emitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheMINA32Target(),
                                        createMINA32MCCodeEmitter);

  // Register the object streamer.
  TargetRegistry::RegisterELFStreamer(getTheMINA32Target(), createMCStreamer);

  // Register the ASM backend (as little endian).
  TargetRegistry::RegisterMCAsmBackend(getTheMINA32Target(),
                                       createMINA32AsmBackend);
}

} // end namespace llvm
