; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=mina32 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s -check-prefix=M32I

; TODO: check the generated instructions for the equivalent of seqz, snez,
; sltz, sgtz map to something simple

define i32 @icmp_eq(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_eq:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    xor r0, r0, r1
; M32I-NEXT:    sltiu r0, r0, 1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp eq i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_ne(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_ne:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    xor r0, r0, r1
; M32I-NEXT:    sltiu r0, r0, 1
; M32I-NEXT:    xori r0, r0, 1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp ne i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_ugt(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_ugt:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    sltu r0, r1, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp ugt i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_uge(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_uge:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    sltu r0, r0, r1
; M32I-NEXT:    xori r0, r0, 1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp uge i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_ult(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_ult:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    sltu r0, r0, r1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp ult i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_ule(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_ule:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    sltu r0, r1, r0
; M32I-NEXT:    xori r0, r0, 1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp ule i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_sgt(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_sgt:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    slt r0, r1, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp sgt i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_sge(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_sge:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    slt r0, r0, r1
; M32I-NEXT:    xori r0, r0, 1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp sge i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_slt(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_slt:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    slt r0, r0, r1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp slt i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

define i32 @icmp_sle(i32 %a, i32 %b) nounwind {
; M32I-LABEL: icmp_sle:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    slt r0, r1, r0
; M32I-NEXT:    xori r0, r0, 1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = icmp sle i32 %a, %b
  %2 = zext i1 %1 to i32
  ret i32 %2
}

; TODO: check variants with an immediate?
