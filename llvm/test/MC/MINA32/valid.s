; RUN: llvm-mc %s -triple=mina32 -show-encoding \
; RUN:     | FileCheck -check-prefixes=CHECK,CHECK-INST %s
; RUN: llvm-mc -filetype=obj -triple mina32 < %s \
; RUN:     | llvm-objdump -d - | FileCheck -check-prefix=CHECK-INST %s

; Arithmetic instruction tests

; CHECK-INST: addi sp, r12, 25
; CHECK: encoding: [0x19,0xd0,0xc0,0x00]
addi r13, r12, 5*5
; CHECK-INST: addi sp, r12, 3
; CHECK: encoding: [0x03,0xd0,0xc0,0x00]
addi r13, r12, 0b11
; CHECK-INST: addi sp, r12, -5
; CHECK: encoding: [0xfb,0xdf,0xc0,0x00]
addi r13, r12, -5
; CHECK-INST: addi sp, r12, 4080
; CHECK: encoding: [0xf8,0xd7,0xc1,0x00]
addi r13, r12, 0xff0
; CHECK-INST: addi sp, r12, -4080
; CHECK: encoding: [0x08,0xd8,0xc1,0x00]
addi r13, r12, -0xff0
; CHECK-INST: multi sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x01]
multi r13, r12, 5
; CHECK-INST: divi sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x02]
divi r13, r12, 5
; CHECK-INST: remi sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x03]
remi r13, r12, 5
; CHECK-INST: slti sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x04]
slti r13, r12, 5
; CHECK-INST: sltiu sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x05]
sltiu r13, r12, 5

; CHECK-INST: mult sp, r12, r5
; CHECK: encoding: [0x00,0xd0,0xc5,0x09]
mult r13, r12, r5
; CHECK-INST: div sp, r12, r5
; CHECK: encoding: [0x00,0xd0,0xc5,0x0a]
div r13, r12, r5
; CHECK-INST: rem sp, r12, r5
; CHECK: encoding: [0x00,0xd0,0xc5,0x0b]
rem r13, r12, r5
; CHECK-INST: slt sp, r12, r5
; CHECK: encoding: [0x00,0xd0,0xc5,0x0c]
slt r13, r12, r5
; CHECK-INST: sltu sp, r12, r5
; CHECK: encoding: [0x00,0xd0,0xc5,0x0d]
sltu r13, r12, r5

; Logical instruction tests

; CHECK-INST: andi sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x10]
andi r13, r12, 5
; CHECK-INST: ori sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x11]
ori r13, r12, 5
; CHECK-INST: xori sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x12]
xori r13, r12, 5
; CHECK-INST: nandi sp, r12, 5
; CHECK: encoding: [0x05,0xd0,0xc0,0x13]
nandi r13, r12, 5

; CHECK-INST: and r0, r5, sp
; CHECK: encoding: [0x00,0x00,0x5d,0x18]
and r0, r5, sp
; CHECK-INST: or r0, r5, sp
; CHECK: encoding: [0x00,0x00,0x5d,0x19]
or r0, r5, sp
; CHECK-INST: xor r0, r5, sp
; CHECK: encoding: [0x00,0x00,0x5d,0x1a]
xor r0, r5, sp
; CHECK-INST: nand r0, r5, sp
; CHECK: encoding: [0x00,0x00,0x5d,0x1b]
nand r0, r5, sp
; CHECK-INST: popcnt r1, r1
; CHECK: encoding: [0x00,0x10,0x11,0x1c]
popcnt r1, r1
; CHECK-INST: clo r0, r5
; CHECK: encoding: [0x00,0x00,0x50,0x1d]
clo r0, r5
;plo r0, r5

; Comparison instruction tests

; CHECK-INST: cmpi.eq r1, 5
; CHECK: encoding: [0x05,0x00,0x10,0x20]
cmpi.eq r1, 5
; CHECK-INST: cmpi.lo r1, 5
; CHECK: encoding: [0x05,0x00,0x10,0x21]
cmpi.lo r1, 5
; CHECK-INST: cmpi.ls r1, 5
; CHECK: encoding: [0x05,0x00,0x10,0x22]
cmpi.ls r1, 5
; CHECK-INST: cmpi.lt r1, 5
; CHECK: encoding: [0x05,0x00,0x10,0x23]
cmpi.lt r1, 5
; CHECK-INST: cmpi.le r1, 5
; CHECK: encoding: [0x05,0x00,0x10,0x24]
cmpi.le r1, 5

; CHECK-INST: cmp.eq r1, r5
; CHECK: encoding: [0x00,0x00,0x15,0x28]
cmp.eq r1, r5
; CHECK-INST: cmp.lo r1, r5
; CHECK: encoding: [0x00,0x00,0x15,0x29]
cmp.lo r1, r5
; CHECK-INST: cmp.ls r1, r5
; CHECK: encoding: [0x00,0x00,0x15,0x2a]
cmp.ls r1, r5
; CHECK-INST: cmp.lt r1, r5
; CHECK: encoding: [0x00,0x00,0x15,0x2b]
cmp.lt r1, r5
; CHECK-INST: cmp.le r1, r5
; CHECK: encoding: [0x00,0x00,0x15,0x2c]
cmp.le r1, r5

; Register branch instruction tests

;rbra r4, 0x20
;rcall r4, 0x20
; CHECK-INST: ret
; CHECK: encoding: [0x00,0x00,0x00,0x32]
ret

;robra r1, r5
;rocall r1, r5

; Memory access instruction tests

; CHECK-INST: ld r8, [r2, 4]
; CHECK: encoding: [0x01,0x80,0x20,0x40]
ld r8, [r2, 4]
; CHECK-INST: ldh r8, [r2, 4]
; CHECK: encoding: [0x02,0x80,0x20,0x41]
ldh r8, [r2, 4]
; CHECK-INST: ldb r8, [r2, 4]
; CHECK: encoding: [0x04,0x80,0x20,0x42]
ldb r8, [r2, 4]
; CHECK-INST: st r8, [r2, 4]
; CHECK: encoding: [0x01,0x80,0x20,0x43]
st r8, [r2, 4]
; CHECK-INST: sth r8, [r2, 4]
; CHECK: encoding: [0x02,0x80,0x20,0x44]
sth r8, [r2, 4]
; CHECK-INST: stb r8, [r2, 4]
; CHECK: encoding: [0x04,0x80,0x20,0x45]
stb r8, [r2, 4]
;ldc [r2, 4]
;stc [r2, 4]

; CHECK-INST: rld r8, [r2, r4]
; CHECK: encoding: [0x00,0x80,0x24,0x48]
rld r8, [r2, r4]
; CHECK-INST: rldh r8, [r2, r4]
; CHECK: encoding: [0x00,0x80,0x24,0x49]
rldh r8, [r2, r4]
; CHECK-INST: rldb r8, [r2, r4]
; CHECK: encoding: [0x00,0x80,0x24,0x4a]
rldb r8, [r2, r4]
; CHECK-INST: rst r8, [r2, r4]
; CHECK: encoding: [0x00,0x80,0x24,0x4b]
rst r8, [r2, r4]
; CHECK-INST: rsth r8, [r2, r4]
; CHECK: encoding: [0x00,0x80,0x24,0x4c]
rsth r8, [r2, r4]
; CHECK-INST: rstb r8, [r2, r4]
; CHECK: encoding: [0x00,0x80,0x24,0x4d]
rstb r8, [r2, r4]
;pop r8
;push r8

; Move instruction tests

; CHECK-INST: movi r1, 5
; CHECK: encoding: [0x05,0x10,0x00,0x50]
movi r1, 5
; CHECK-INST: mti r1, 5
; CHECK: encoding: [0x05,0x10,0x00,0x51]
mti r1, 5
; CHECK-INST: mfi r1, 5
; CHECK: encoding: [0x05,0x10,0x00,0x52]
mfi r1, 5

; CHECK-INST: movl r1, 65535
; CHECK: encoding: [0xff,0x1f,0x0f,0x53]
movl r1, 0xffff
; CHECK-INST: movu r1, 65535
; CHECK: encoding: [0xff,0x1f,0x0f,0x54]
movu r1, 0xffff

;mov r0, r5
; CHECK-INST: mt r4, r2
; CHECK: encoding: [0x00,0x40,0x20,0x51]
mt r4, r2
;mfrc sp

; Shift instruction tests

; CHECK-INST: lsl r8, r3, 4
; CHECK: encoding: [0x04,0x80,0x30,0x60]
lsl r8, r3, 4
; CHECK-INST: lsr r8, r3, 4
; CHECK: encoding: [0x04,0x80,0x30,0x61]
lsr r8, r3, 4
; CHECK-INST: asr r8, r3, 4
; CHECK: encoding: [0x04,0x80,0x30,0x62]
asr r8, r3, 4
; CHECK-INST: ror r8, r3, 4
; CHECK: encoding: [0x04,0x80,0x30,0x63]
ror r8, r3, 4

; CHECK-INST: rlsl r8, r3, r4
; CHECK: encoding: [0x00,0x80,0x34,0x68]
rlsl r8, r3, r4
; CHECK-INST: rlsr r8, r3, r4
; CHECK: encoding: [0x00,0x80,0x34,0x69]
rlsr r8, r3, r4
; CHECK-INST: rasr r8, r3, r4
; CHECK: encoding: [0x00,0x80,0x34,0x6a]
rasr r8, r3, r4
; CHECK-INST: rror r8, r3, r4
; CHECK: encoding: [0x00,0x80,0x34,0x6b]
rror r8, r3, r4

;flsl r8,r3,r4,5
;flsr r8,r3,r4,5

; Control instruction tests

;stop
;wfi
;sett
;clrt
;switch

;svcall r1
;fault r1, r2
;mtof r1
;mfrf r1

; Relative branch instruction tests

; CHECK-INST: bra 32768
; CHECK: encoding: [0x00,0x80,0x00,0x80]
bra 0x8000
; CHECK-INST: bra -32768
; CHECK: encoding: [0x00,0x80,0xff,0x80]
bra -0x8000
; CHECK-INST: bt 32768
; CHECK: encoding: [0x00,0x80,0x00,0x81]
bt 0x8000
; CHECK-INST: bf 32768
; CHECK: encoding: [0x00,0x80,0x00,0x82]
bf 0x8000

;call 0x8000
;ct 0x8000
;cf 0x8000
