//===-- MINA32AsmPrinter.cpp - MINA32 LLVM Assembly Printer ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to MINA32 assembly language.
//
//===----------------------------------------------------------------------===//

#include "MINA32InstrInfo.h"
#include "MINA32TargetMachine.h"
#include "MCTargetDesc/MINA32InstPrinter.h"
#include "TargetInfo/MINA32TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/TargetRegistry.h"

#include "MCTargetDesc/MINA32MCExpr.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace llvm {
class MINA32AsmPrinter : public AsmPrinter {
public:
  explicit MINA32AsmPrinter(TargetMachine &TM,
                            std::unique_ptr<MCStreamer> Streamer)
    : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "MINA32 Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);

private:
  void LowerInstruction(const MachineInstr *MI, MCInst &OutMI) const;
  MCOperand LowerOperand(const MachineOperand& MO) const;
  MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;
};
}

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "MINA32GenMCPseudoLowering.inc"

void MINA32AsmPrinter::emitInstruction(const MachineInstr *MI) {
  // Do any auto-generated pseudo lowerings.
  if (emitPseudoExpansionLowering(*OutStreamer, MI))
    return;

  MCInst TmpInst;
  LowerInstruction(MI, TmpInst);
  EmitToStreamer(*OutStreamer, TmpInst);
}

// TODO: Usually in seperate MINA32MCInstLower.cpp

void MINA32AsmPrinter::LowerInstruction(const MachineInstr *MI,
                                        MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp = LowerOperand(MO);
    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}

MCOperand MINA32AsmPrinter::LowerOperand(const MachineOperand& MO) const {
  switch (MO.getType()) {
  default:
    report_fatal_error("unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return MCOperand();
    return MCOperand::createReg(MO.getReg());

  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm());

  case MachineOperand::MO_MachineBasicBlock:
    return LowerSymbolOperand(MO, MO.getMBB()->getSymbol());

  case MachineOperand::MO_GlobalAddress:
    return LowerSymbolOperand(MO, getSymbol(MO.getGlobal()));

  case MachineOperand::MO_BlockAddress:
    return LowerSymbolOperand(MO, GetBlockAddressSymbol(MO.getBlockAddress()));

  case MachineOperand::MO_ExternalSymbol:
    return LowerSymbolOperand(MO, GetExternalSymbolSymbol(MO.getSymbolName()));

  case MachineOperand::MO_ConstantPoolIndex:
    return LowerSymbolOperand(MO, GetCPISymbol(MO.getIndex()));

  case MachineOperand::MO_RegisterMask:
    return MCOperand();

  }
}

MCOperand MINA32AsmPrinter::LowerSymbolOperand(const MachineOperand &MO,
                                               MCSymbol *Sym) const {
  MCContext &Ctx = OutContext;
  MINA32MCExpr::VariantKind Kind;

  switch (MO.getTargetFlags()) {
  default:
    llvm_unreachable("Unknown target flag on GV operand");
  case MINA32II::MO_None:
    Kind = MINA32MCExpr::VK_MINA32_None;
    break;
  case MINA32II::MO_LO:
    Kind = MINA32MCExpr::VK_MINA32_LO;
    break;
  case MINA32II::MO_HI:
    Kind = MINA32MCExpr::VK_MINA32_HI;
    break;
  }

  const MCExpr *Expr =
    MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);

  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    Expr = MCBinaryExpr::createAdd(
        Expr, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  Expr = MINA32MCExpr::create(Kind, Expr, Ctx);
  return MCOperand::createExpr(Expr);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMINA32AsmPrinter() {
  RegisterAsmPrinter<MINA32AsmPrinter> X(getTheMINA32Target());
}
