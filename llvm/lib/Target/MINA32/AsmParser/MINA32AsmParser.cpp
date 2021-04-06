//==-- MINA32AsmParser.cpp - Parse MINA32 assembly to MCInst instructions -===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MINA32MCExpr.h"
#include "MCTargetDesc/MINA32MCTargetDesc.h"
#include "MINA32.h"
#include "MINA32RegisterInfo.h"
#include "TargetInfo/MINA32TargetInfo.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "MINA32-asm-parser"

namespace {
class MINA32AssemblerOptions {
public:
  MINA32AssemblerOptions() : reorder(true), macro(true) {}

  bool isReorder() { return reorder; }
  void setReorder() { reorder = true; }
  void setNoreorder() { reorder = false; }

  bool isMacro() { return macro; }
  void setMacro() { macro = true; }
  void setNomacro() { macro = false; }

private:
  bool reorder;
  bool macro;
};
} // namespace

namespace {
class MINA32AsmParser : public MCTargetAsmParser {
  MINA32AssemblerOptions Options;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;
  OperandMatchResultTy tryParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                        SMLoc &EndLoc) override;

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool ParseDirective(AsmToken DirectiveID) override;

// Auto-generated instruction matching functions
#define GET_ASSEMBLER_HEADER
#include "MINA32GenAsmMatcher.inc"

  OperandMatchResultTy parseImmediate(OperandVector &Operands);
  OperandMatchResultTy parseRegister(OperandVector &Operands);
  bool parseOperand(OperandVector &Operands);

public:
  MINA32AsmParser(const MCSubtargetInfo &sti, MCAsmParser &parser,
                  const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, sti, MII) {
    MCAsmParserExtension::Initialize(parser);

    // Initialize the set of available features.
    setAvailableFeatures(ComputeAvailableFeatures(getSTI().getFeatureBits()));
  }
};
} // namespace

namespace {

/// MINA32Operand - Instances of this class represent a parsed MINA32 machine
/// instruction.
class MINA32Operand : public MCParsedAsmOperand {

  enum KindTy { k_Immediate, k_Memory, k_Register, k_Token } Kind;

public:
  MINA32Operand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

  struct Token {
    const char *Data;
    unsigned Length;
  };
  struct PhysRegOp {
    unsigned RegNum; /// Register Number
  };
  struct ImmOp {
    const MCExpr *Val;
  };
  struct MemOp {
    unsigned Base;
    const MCExpr *Off;
  };

  union {
    struct Token Tok;
    struct PhysRegOp Reg;
    struct ImmOp Imm;
    struct MemOp Mem;
  };

  SMLoc StartLoc, EndLoc;

public:
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediate when possible.  Null MCExpr = 0.
    if (Expr == 0)
      Inst.addOperand(MCOperand::createImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCExpr *Expr = getImm();
    addExpr(Inst, Expr);
  }

  void addMemOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getMemBase()));
    const MCExpr *Expr = getMemOff();
    addExpr(Inst, Expr);
  }

  bool isReg() const override { return Kind == k_Register; }
  bool isImm() const override { return Kind == k_Immediate; }
  bool isToken() const override { return Kind == k_Token; }
  bool isMem() const override { return Kind == k_Memory; }

  StringRef getToken() const {
    assert(Kind == k_Token && "Invalid access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  unsigned getReg() const override {
    assert((Kind == k_Register) && "Invalid access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert((Kind == k_Immediate) && "Invalid access!");
    return Imm.Val;
  }

  unsigned getMemBase() const {
    assert((Kind == k_Memory) && "Invalid access!");
    return Mem.Base;
  }

  const MCExpr *getMemOff() const {
    assert((Kind == k_Memory) && "Invalid access!");
    return Mem.Off;
  }

  static std::unique_ptr<MINA32Operand> CreateToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<MINA32Operand>(k_Token);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  /// Internal constructor for register kinds
  static std::unique_ptr<MINA32Operand> CreateReg(unsigned RegNum, SMLoc S,
                                                  SMLoc E) {
    auto Op = std::make_unique<MINA32Operand>(k_Register);
    Op->Reg.RegNum = RegNum;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<MINA32Operand> CreateImm(const MCExpr *Val, SMLoc S,
                                                  SMLoc E) {
    auto Op = std::make_unique<MINA32Operand>(k_Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<MINA32Operand>
  CreateMem(unsigned Base, const MCExpr *Off, SMLoc S, SMLoc E) {
    auto Op = std::make_unique<MINA32Operand>(k_Memory);
    Op->Mem.Base = Base;
    Op->Mem.Off = Off;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  /// getStartLoc - Get the location of the first token of this operand.
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Get the location of the last token of this operand.
  SMLoc getEndLoc() const override { return EndLoc; }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case k_Immediate:
      OS << "Imm<";
      OS << *Imm.Val;
      OS << ">";
      break;
    case k_Memory:
      OS << "Mem<";
      OS << Mem.Base;
      OS << ", ";
      OS << *Mem.Off;
      OS << ">";
      break;
    case k_Register:
      OS << "Register<" << Reg.RegNum << ">";
      break;
    case k_Token:
      OS << Tok.Data;
      break;
    }
  }
};
} // namespace

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "MINA32GenAsmMatcher.inc"

bool MINA32AsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                              OperandVector &Operands,
                                              MCStreamer &Out,
                                              uint64_t &ErrorInfo,
                                              bool MatchingInlineAsm) {
  MCInst Inst;
  unsigned MatchResult =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);
  switch (MatchResult) {
  default:
    break;
  case Match_Success: {
    Inst.setLoc(IDLoc);
    Out.emitInstruction(Inst, getSTI());
    return false;
  }
  case Match_MissingFeature:
    Error(IDLoc, "instruction requires a CPU feature not currently enabled");
    return true;
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(IDLoc, "too few operands for instruction");

      ErrorLoc = ((MINA32Operand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }

    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_MnemonicFail:
    return Error(IDLoc, "invalid instruction");
  }
  return true;
}

bool MINA32AsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                    SMLoc &EndLoc) {
  if (tryParseRegister(RegNo, StartLoc, EndLoc) != MatchOperand_Success)
    return Error(StartLoc, "invalid register name");
  return false;
}

OperandMatchResultTy MINA32AsmParser::tryParseRegister(unsigned &RegNo,
                                                       SMLoc &StartLoc,
                                                       SMLoc &EndLoc) {
  const AsmToken &Tok = getParser().getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  StringRef Name = Tok.getIdentifier();

  if (!MatchRegisterName(Name) || !MatchRegisterAltName(Name)) {
    getParser().Lex(); // Eat identifier token.
    return MatchOperand_Success;
  }

  return MatchOperand_NoMatch;
}

OperandMatchResultTy
MINA32AsmParser::parseRegister(OperandVector &Operands) {
  SMLoc S = getParser().getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);

  if (getLexer().isNot(AsmToken::Identifier))
    return MatchOperand_NoMatch;

  StringRef Name = getLexer().getTok().getIdentifier();
  unsigned RegNo = MatchRegisterName(Name);
  if (RegNo == 0) RegNo = MatchRegisterAltName(Name);
  if (RegNo == 0) return MatchOperand_NoMatch;

  getLexer().Lex();
  Operands.push_back(MINA32Operand::CreateReg(RegNo, S, E));
  return MatchOperand_Success;
}

OperandMatchResultTy
MINA32AsmParser::parseImmediate(OperandVector &Operands) {
  SMLoc S = getParser().getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);
  const MCExpr *IdVal;

  switch (getLexer().getKind()) {
  default:
    return MatchOperand_NoMatch;
  case AsmToken::LParen:
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Integer:
  case AsmToken::String:
    if (getParser().parseExpression(IdVal))
      return MatchOperand_ParseFail;
    break;
  /*case AsmToken::Identifier: {
    StringRef Identifier;
    if (getParser().parseIdentifier(Identifier))
      return MatchOperand_ParseFail;
    MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);
    IdVal = MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, getContext());
    break;
  }*/
  }

  Operands.push_back(MINA32Operand::CreateImm(IdVal, S, E));
  return MatchOperand_Success;
}

// Looks at a token type and creates the relevant operand
// from this information, adding to Operands.
// If operand was parsed, returns false, else true.
bool MINA32AsmParser::parseOperand(OperandVector &Operands) {
  // Attempt to parse token as a register.
  if (parseRegister(Operands) == MatchOperand_Success)
    return false;

  // Attempt to parse token as an immediate.
  if (parseImmediate(Operands) == MatchOperand_Success)
    return false;

  // Finally we have exhausted all options and must declare defeat.
  SMLoc Loc = getParser().getTok().getLoc();
  return Error(Loc, "unknown operand");
}

bool MINA32AsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                       StringRef Name, SMLoc NameLoc,
                                       OperandVector &Operands) {
  // Create the leading tokens for the mnemonic.
  MCAsmParser &Parser = getParser();
  Operands.push_back(MINA32Operand::CreateToken(Name, NameLoc));

  // Read the remaining operands.
  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    // Read the first operand.
    if (parseOperand(Operands)) {
      SMLoc Loc = getLexer().getLoc();
      Parser.eatToEndOfStatement();
      return Error(Loc, "unexpected token in argument list (first)");
    }

    while (getLexer().is(AsmToken::Comma)) {
      getLexer().Lex(); // Eat the comma.

      // Parse left bracket.
      if (Parser.parseOptionalToken(AsmToken::LBrac)) {
        SMLoc Loc = Parser.getTok().getLoc();
        Operands.push_back(MINA32Operand::CreateToken("[", Loc));
      }

      // Parse and remember the operand.
      if (parseOperand(Operands)) {
        SMLoc Loc = getLexer().getLoc();
        Parser.eatToEndOfStatement();
        return Error(Loc, "unexpected token in argument list");
      }
    }
  }

  // Parse right bracket.
  if (Parser.parseOptionalToken(AsmToken::RBrac)) {
    SMLoc Loc = Parser.getTok().getLoc();
    Operands.push_back(MINA32Operand::CreateToken("]", Loc));
  }

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    Parser.eatToEndOfStatement();
    return Error(Loc, "unexpected token in argument list");
  }

  Parser.Lex(); // Consume the EndOfStatement
  return false;
}

bool MINA32AsmParser::ParseDirective(AsmToken DirectiveID) {
  MCAsmParser &Parser = getParser();

  if (DirectiveID.getString() == ".ent") {
    // ignore this directive for now
    Parser.Lex();
    return false;
  }

  if (DirectiveID.getString() == ".end") {
    // ignore this directive for now
    Parser.Lex();
    return false;
  }

  if (DirectiveID.getString() == ".frame") {
    // ignore this directive for now
    Parser.eatToEndOfStatement();
    return false;
  }

  if (DirectiveID.getString() == ".fmask") {
    // ignore this directive for now
    Parser.eatToEndOfStatement();
    return false;
  }

  if (DirectiveID.getString() == ".mask") {
    // ignore this directive for now
    Parser.eatToEndOfStatement();
    return false;
  }

  if (DirectiveID.getString() == ".gpword") {
    // ignore this directive for now
    Parser.eatToEndOfStatement();
    return false;
  }

  return true;
}

extern "C" void LLVMInitializeMINA32AsmParser() {
  RegisterMCAsmParser<MINA32AsmParser> X(getTheMINA32Target());
}
