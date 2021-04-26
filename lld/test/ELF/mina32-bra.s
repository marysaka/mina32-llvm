# REQUIRES: mina32

# RUN: llvm-mc -filetype=obj -triple=mina32-elf -mattr=-relax %s -o %t.o

# RUN: ld.lld %t.o --defsym foo=_start+4 --defsym bar=_start -o %t
# RUN: llvm-objdump -d %t | FileCheck %s
# CHECK: 01 00 00 81     bt    4
# CHECK: ff ff ff 82     bf    -4
#
# RUN: ld.lld %t.o --defsym foo=_start+0x1fffffc --defsym bar=_start+4-0x2000000 -o %t.limits
# RUN: llvm-objdump -d %t.limits | FileCheck --check-prefix=LIMITS %s
# LIMITS:      ff ff 7f 81     bt 33554428
# LIMITS-NEXT: 00 00 80 82     bf -33554432

# RUN: not ld.lld %t.o --defsym foo=_start+0x2000000 --defsym bar=_start+4-0x2000004 -o /dev/null 2>&1 | FileCheck --check-prefix=ERROR-RANGE %s
# ERROR-RANGE: relocation R_MINA32_BRA out of range: 33554432 is not in [-33554432, 33554431]; references foo
# ERROR-RANGE: relocation R_MINA32_BRA out of range: -33554436 is not in [-33554432, 33554431]; references bar

# RUN: not ld.lld %t.o --defsym foo=_start+2 --defsym bar=_start-2 -o /dev/null 2>&1 | FileCheck --check-prefix=ERROR-ALIGN %s
# ERROR-ALIGN: improper alignment for relocation R_MINA32_BRA: 0x2 is not aligned to 4 bytes

.global _start
_start:
     bt foo
     bf bar
