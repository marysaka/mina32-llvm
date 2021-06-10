//===-- MINA32InstrInfo.td - MINA32 Instruction defs -------*- tablegen -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// MINA32 Instruction Format Definitions.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// MINA32 instruction definition
//===----------------------------------------------------------------------===//

/// A generic MINA32 instruction.
class MINA32Inst<dag outs, dag ins, string asmstr,
                 list<dag> pattern> : Instruction {
  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
  let Size = 4;

  let Namespace = "MINA32";
  let DecoderNamespace = "MINA32";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let Pattern = pattern;
  let AsmString = asmstr;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string asmstr = "">
    : MINA32Inst<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

// TODO


// Standard format
class MINA32InstStandard<bits<4> group, bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : MINA32Inst<outs, ins, asmstr, pattern>
{
  bits<4> DEST;
  bits<4> SRC2;
  bits<4> SRC1;
  
  let Inst{11-0}  = 0;
  let Inst{15-12} = DEST;
  let Inst{19-16} = SRC2;
  let Inst{23-20} = SRC1;
  let Inst{27-24} = opcode;
  let Inst{31-28} = group;
}

// Immediate format
class MINA32InstImmediate<bits<4> group, bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : MINA32Inst<outs, ins, asmstr, pattern>
{
  bits<16> IMMS;
  bits<4>  DEST;
  bits<4>  SRC1;
  
  let Inst{11-0}  = IMMS{11-0};
  let Inst{15-12} = DEST;
  let Inst{19-16} = IMMS{15-12};
  let Inst{23-20} = SRC1;
  let Inst{27-24} = opcode;
  let Inst{31-28} = group;
}

// Move format
class MINA32InstMove<bits<4> group, bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : MINA32Inst<outs, ins, asmstr, pattern>
{
  bits<16> IMM;
  bits<4>  DEST;
  
  let Inst{11-0}  = IMM{11-0};
  let Inst{15-12} = DEST;
  let Inst{19-16} = IMM{15-12};
  let Inst{23-20} = 0;
  let Inst{27-24} = opcode;
  let Inst{31-28} = group;
}

// Funnel Shift format
class MINA32InstFunnelShift<bits<4> group, bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : MINA32Inst<outs, ins, asmstr, pattern>
{
  bits<4> RSHIFT;
  bits<4> DEST;
  bits<4> SRC2;
  bits<4> SRC1;
  
  let Inst{7-0}   = 0;
  let Inst{11-8}  = RSHIFT;
  let Inst{15-12} = DEST;
  let Inst{19-16} = SRC2;
  let Inst{23-20} = SRC1;
  let Inst{27-24} = opcode;
  let Inst{31-28} = group;
}

// Branch format
class MINA32InstBranch<bits<4> group, bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
  : MINA32Inst<outs, ins, asmstr, pattern>
{
  bits<24> IMM;
  
  let Inst{23-0}  = IMM;
  let Inst{27-24} = opcode;
  let Inst{31-28} = group;
}
