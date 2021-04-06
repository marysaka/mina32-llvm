; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=mina32 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s -check-prefix=M32I

declare i16 @llvm.bswap.i16(i16)
declare i32 @llvm.bswap.i32(i32)
declare i64 @llvm.bswap.i64(i64)
declare i8 @llvm.cttz.i8(i8, i1)
declare i16 @llvm.cttz.i16(i16, i1)
declare i32 @llvm.cttz.i32(i32, i1)
declare i64 @llvm.cttz.i64(i64, i1)
declare i32 @llvm.ctlz.i32(i32, i1)
declare i32 @llvm.ctpop.i32(i32)

define i16 @test_bswap_i16(i16 %a) nounwind {
; M32I-LABEL: test_bswap_i16:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    lsl r1, r0, 24
; M32I-NEXT:    lsl r0, r0, 8
; M32I-NEXT:    andi r0, r0, 16711680
; M32I-NEXT:    or r0, r1, r0
; M32I-NEXT:    lsr r0, r0, 16
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i16 @llvm.bswap.i16(i16 %a)
  ret i16 %tmp
}

define i32 @test_bswap_i32(i32 %a) nounwind {
; M32I-LABEL: test_bswap_i32:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    lsr r1, r0, 8
; M32I-NEXT:    andi r1, r1, 65280
; M32I-NEXT:    lsr r2, r0, 24
; M32I-NEXT:    or r1, r1, r2
; M32I-NEXT:    lsl r2, r0, 24
; M32I-NEXT:    lsl r0, r0, 8
; M32I-NEXT:    andi r0, r0, 16711680
; M32I-NEXT:    or r0, r2, r0
; M32I-NEXT:    or r0, r0, r1
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i32 @llvm.bswap.i32(i32 %a)
  ret i32 %tmp
}

define i64 @test_bswap_i64(i64 %a) nounwind {
; M32I-LABEL: test_bswap_i64:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    lsr r2, r1, 8
; M32I-NEXT:    andi r2, r2, 65280
; M32I-NEXT:    lsr r3, r1, 24
; M32I-NEXT:    or r2, r2, r3
; M32I-NEXT:    lsl r3, r1, 24
; M32I-NEXT:    lsl r1, r1, 8
; M32I-NEXT:    andi r1, r1, 16711680
; M32I-NEXT:    or r1, r3, r1
; M32I-NEXT:    or r2, r1, r2
; M32I-NEXT:    lsr r1, r0, 8
; M32I-NEXT:    andi r1, r1, 65280
; M32I-NEXT:    lsr r3, r0, 24
; M32I-NEXT:    or r1, r1, r3
; M32I-NEXT:    lsl r3, r0, 24
; M32I-NEXT:    lsl r0, r0, 8
; M32I-NEXT:    andi r0, r0, 16711680
; M32I-NEXT:    or r0, r3, r0
; M32I-NEXT:    or r1, r0, r1
; M32I-NEXT:    addi r0, r2, 0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i64 @llvm.bswap.i64(i64 %a)
  ret i64 %tmp
}

define i8 @test_cttz_i8(i8 %a) nounwind {
; M32I-LABEL: test_cttz_i8:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, 0
; M32I-NEXT:    movi r0, 8
; M32I-NEXT:    andi r2, r1, 255
; M32I-NEXT:    cmpi.eq r2, 0
; M32I-NEXT:    bt .LBB3_2
; M32I-NEXT:    bra .LBB3_1
; M32I-NEXT:  .LBB3_1: ; %cond.false
; M32I-NEXT:    addi r0, r1, -1
; M32I-NEXT:    xori r1, r1, -1
; M32I-NEXT:    and r0, r1, r0
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:  .LBB3_2: ; %cond.end
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i8 @llvm.cttz.i8(i8 %a, i1 false)
  ret i8 %tmp
}

define i16 @test_cttz_i16(i16 %a) nounwind {
; M32I-LABEL: test_cttz_i16:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, 0
; M32I-NEXT:    movi r0, 16
; M32I-NEXT:    movu r2, 0
; M32I-NEXT:    movl r2, 65535
; M32I-NEXT:    and r2, r1, r2
; M32I-NEXT:    cmpi.eq r2, 0
; M32I-NEXT:    bt .LBB4_2
; M32I-NEXT:    bra .LBB4_1
; M32I-NEXT:  .LBB4_1: ; %cond.false
; M32I-NEXT:    addi r0, r1, -1
; M32I-NEXT:    xori r1, r1, -1
; M32I-NEXT:    and r0, r1, r0
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:  .LBB4_2: ; %cond.end
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i16 @llvm.cttz.i16(i16 %a, i1 false)
  ret i16 %tmp
}

define i32 @test_cttz_i32(i32 %a) nounwind {
; M32I-LABEL: test_cttz_i32:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, 0
; M32I-NEXT:    movi r0, 32
; M32I-NEXT:    cmpi.eq r1, 0
; M32I-NEXT:    bt .LBB5_2
; M32I-NEXT:    bra .LBB5_1
; M32I-NEXT:  .LBB5_1: ; %cond.false
; M32I-NEXT:    addi r0, r1, -1
; M32I-NEXT:    xori r1, r1, -1
; M32I-NEXT:    and r0, r1, r0
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:  .LBB5_2: ; %cond.end
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i32 @llvm.cttz.i32(i32 %a, i1 false)
  ret i32 %tmp
}

define i32 @test_ctlz_i32(i32 %a) nounwind {
; M32I-LABEL: test_ctlz_i32:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, 0
; M32I-NEXT:    movi r0, 32
; M32I-NEXT:    cmpi.eq r1, 0
; M32I-NEXT:    bt .LBB6_2
; M32I-NEXT:    bra .LBB6_1
; M32I-NEXT:  .LBB6_1: ; %cond.false
; M32I-NEXT:    xori r0, r1, -1
; M32I-NEXT:    clo r0, r0
; M32I-NEXT:  .LBB6_2: ; %cond.end
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i32 @llvm.ctlz.i32(i32 %a, i1 false)
  ret i32 %tmp
}

define i64 @test_cttz_i64(i64 %a) nounwind {
; M32I-LABEL: test_cttz_i64:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r2, r1, -1
; M32I-NEXT:    xori r1, r1, -1
; M32I-NEXT:    and r1, r1, r2
; M32I-NEXT:    addi r2, r0, -1
; M32I-NEXT:    xori r3, r0, -1
; M32I-NEXT:    and r2, r3, r2
; M32I-NEXT:    popcnt r3, r2
; M32I-NEXT:    popcnt r1, r1
; M32I-NEXT:    addi r2, r1, 32
; M32I-NEXT:    cmpi.eq r0, 0
; M32I-NEXT:    mf r2, r3
; M32I-NEXT:    movi r1, 0
; M32I-NEXT:    addi r0, r2, 0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i64 @llvm.cttz.i64(i64 %a, i1 false)
  ret i64 %tmp
}

define i8 @test_cttz_i8_zero_undef(i8 %a) nounwind {
; M32I-LABEL: test_cttz_i8_zero_undef:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, -1
; M32I-NEXT:    xori r0, r0, -1
; M32I-NEXT:    and r0, r0, r1
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i8 @llvm.cttz.i8(i8 %a, i1 true)
  ret i8 %tmp
}

define i16 @test_cttz_i16_zero_undef(i16 %a) nounwind {
; M32I-LABEL: test_cttz_i16_zero_undef:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, -1
; M32I-NEXT:    xori r0, r0, -1
; M32I-NEXT:    and r0, r0, r1
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i16 @llvm.cttz.i16(i16 %a, i1 true)
  ret i16 %tmp
}

define i32 @test_cttz_i32_zero_undef(i32 %a) nounwind {
; M32I-LABEL: test_cttz_i32_zero_undef:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r1, r0, -1
; M32I-NEXT:    xori r0, r0, -1
; M32I-NEXT:    and r0, r0, r1
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i32 @llvm.cttz.i32(i32 %a, i1 true)
  ret i32 %tmp
}

define i64 @test_cttz_i64_zero_undef(i64 %a) nounwind {
; M32I-LABEL: test_cttz_i64_zero_undef:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    addi r2, r1, -1
; M32I-NEXT:    xori r1, r1, -1
; M32I-NEXT:    and r1, r1, r2
; M32I-NEXT:    addi r2, r0, -1
; M32I-NEXT:    xori r3, r0, -1
; M32I-NEXT:    and r2, r3, r2
; M32I-NEXT:    popcnt r3, r2
; M32I-NEXT:    popcnt r1, r1
; M32I-NEXT:    addi r2, r1, 32
; M32I-NEXT:    cmpi.eq r0, 0
; M32I-NEXT:    mf r2, r3
; M32I-NEXT:    movi r1, 0
; M32I-NEXT:    addi r0, r2, 0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i64 @llvm.cttz.i64(i64 %a, i1 true)
  ret i64 %tmp
}

define i32 @test_ctlz_i32_zero_undef(i32 %a) nounwind {
; M32I-LABEL: test_ctlz_i32_zero_undef:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    xori r0, r0, -1
; M32I-NEXT:    clo r0, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %tmp = call i32 @llvm.ctlz.i32(i32 %a, i1 true)
  ret i32 %tmp
}

define i32 @test_ctpop_i32(i32 %a) nounwind {
; M32I-LABEL: test_ctpop_i32:
; M32I:       ; %bb.0:
; M32I-NEXT:    addi sp, sp, -4
; M32I-NEXT:    st r11, [sp, 0]
; M32I-NEXT:    addi r11, sp, 4
; M32I-NEXT:    popcnt r0, r0
; M32I-NEXT:    ld r11, [sp, 0]
; M32I-NEXT:    addi sp, sp, 4
; M32I-NEXT:    ret
  %1 = call i32 @llvm.ctpop.i32(i32 %a)
  ret i32 %1
}
