; RUN: llvm-mc -triple mina32 < %s -show-encoding \
; RUN:     | FileCheck -check-prefix=CHECK-FIXUP %s
; RUN: llvm-mc -filetype=obj -triple mina32 < %s \
; RUN:     | llvm-objdump -d - | FileCheck -check-prefix=CHECK-INSTR %s
; RUN: llvm-mc -filetype=obj -triple=mina32 %s \
; RUN:     | llvm-readobj -r | FileCheck %s -check-prefix=CHECK-REL

; Checks that fixups that can be resolved within the same object file are
; applied correctly

.LBB0:
movu r1, %hi(val)
; CHECK-FIXUP: fixup A - offset: 0, value: %hi(val), kind: fixup_mina32_hi16
; CHECK-INSTR: movu r1, 4660
movl r1, %lo(val)
; CHECK-FIXUP: fixup A - offset: 0, value: %lo(val), kind: fixup_mina32_lo16
; CHECK-INSTR: movl r1, 22136

bra .LBB0
; CHECK-FIXUP: fixup A - offset: 0, value: .LBB0, kind: fixup_mina32_bra
; CHECK-INSTR: bra -2
bra .LBB2
; CHECK-FIXUP: fixup A - offset: 0, value: .LBB2, kind: fixup_mina32_bra
; CHECK-INSTR: bra 82749
bt .LBB0
; CHECK-FIXUP: fixup A - offset: 0, value: .LBB0, kind: fixup_mina32_bra
; CHECK-INSTR: bt -4
bf .LBB1
; CHECK-FIXUP: fixup A - offset: 0, value: .LBB1, kind: fixup_mina32_bra
; CHECK-INSTR: bf 277

.fill 1104

.LBB1:

.fill 329876
nop
.LBB2:

.set val, 0x12345678

; CHECK-REL-NOT: R_MINA32
