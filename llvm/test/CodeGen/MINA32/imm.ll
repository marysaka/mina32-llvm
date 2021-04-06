; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=mina32 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s -check-prefix=M32I

; Materializing constants

define i32 @zero() nounwind {
; M32I-LABEL: zero:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movi r0, 0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 0
}

define i32 @pos_small() nounwind {
; M32I-LABEL: pos_small:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movi r0, 2047
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 2047
}

define i32 @neg_small() nounwind {
; M32I-LABEL: neg_small:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movi r0, -2048
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 -2048
}

define i32 @pos_shift() nounwind {
; M32I-LABEL: pos_shift:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movi r0, 67076096
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 67076096
}

define i32 @neg_shift() nounwind {
; M32I-LABEL: neg_shift:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movi r0, -67108864
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 -67108864
}

define i32 @pos_over() nounwind {
; M32I-LABEL: pos_over:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movu r0, 0
; M32I-NEXT:    movl r0, 2049
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 2049
}

define i32 @neg_over() nounwind {
; M32I-LABEL: neg_over:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movu r0, 0
; M32I-NEXT:    movl r0, 4095
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 4095
}

define i32 @pos_i32() nounwind {
; M32I-LABEL: pos_i32:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movu r0, 26488
; M32I-NEXT:    movl r0, 10991
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 1735928559
}

define i32 @neg_i32() nounwind {
; M32I-LABEL: neg_i32:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    movu r0, 57005
; M32I-NEXT:    movl r0, 48879
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  ret i32 -559038737
}
