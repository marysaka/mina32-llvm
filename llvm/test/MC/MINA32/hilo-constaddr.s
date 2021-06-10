; RUN: llvm-mc -filetype=obj -triple=mina32 %s \
; RUN:  | llvm-objdump -d - | FileCheck %s -check-prefix=CHECK-INSTR

; RUN: llvm-mc -filetype=obj -triple=mina32 %s \
; RUN:  | llvm-readobj -r | FileCheck %s -check-prefix=CHECK-REL

; Check the assembler can handle hi and lo expressions with a constant 
; address, and constant expressions involving labels. Test case derived from 
; test/MC/Mips/hilo-addressing.s

.equ addr, 0xdeadbeef
  movu r1, %hi(addr)
  movl r1, %lo(addr)
; CHECK-INSTR: movu r1, 57005
; CHECK-INSTR: movl r1, 48879

; Check that assembler can handle %hi(label1 - label2) and %lo(label1 - label2)
; expressions.

tmp1:
; Emit zeros so that difference between tmp1 and tmp3 is 0x30124 bytes.
  .fill 0x30124-8
tmp2:
  movu r1, %hi(tmp3-tmp1)
  movl r1, %lo(tmp3-tmp1)
; CHECK-INSTR: movu r1, 3
; CHECK-INSTR: movl r1, 292

tmp3:
  movu r1, %hi(tmp2-tmp3)
  movl r1, %lo(tmp2-tmp3)
; CHECK-INSTR: movu r1, 65535
; CHECK-INSTR: movl r1, 65528

; Check that a relocation isn't emitted for %hi(label1 - label2) and
; %lo(label1 - label2) expressions.

; CHECK-REL-NOT: R_RISCV
