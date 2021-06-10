# RUN: llvm-mc -triple mina32 < %s -show-encoding \
# RUN:     | FileCheck -check-prefix=INSTR -check-prefix=FIXUP %s
# RUN: llvm-mc -filetype=obj -triple mina32 < %s \
# RUN:     | llvm-readobj -r | FileCheck -check-prefix=RELOC %s

# Check prefixes:
# RELOC - Check the relocation in the object.
# FIXUP - Check the fixup on the instruction.
# INSTR - Check the instruction is handled properly by the ASMPrinter

.long foo
# RELOC: R_MINA32_32 foo

movu r1, %hi(foo)
# RELOC: R_MINA32_HI16 foo 0x0
# INSTR: movu r1, %hi(foo)
# FIXUP: fixup A - offset: 0, value: %hi(foo), kind: fixup_mina32_hi16

movu r1, %hi(foo+4)
# RELOC: R_MINA32_HI16 foo 0x4
# INSTR: movu r1, %hi(foo+4)
# FIXUP: fixup A - offset: 0, value: %hi(foo+4), kind: fixup_mina32_hi16

movl r1, %lo(foo)
# RELOC: R_MINA32_LO16 foo 0x0
# INSTR: movl r1, %lo(foo)
# FIXUP: fixup A - offset: 0, value: %lo(foo), kind: fixup_mina32_lo16

movl r1, %lo(foo+4)
# RELOC: R_MINA32_LO16 foo 0x4
# INSTR: movl r1, %lo(foo+4)
# FIXUP: fixup A - offset: 0, value: %lo(foo+4), kind: fixup_mina32_lo16

bra foo
# RELOC: R_MINA32_BRA
# INSTR: bra foo
# FIXUP: fixup A - offset: 0, value: foo, kind: fixup_mina32_bra
