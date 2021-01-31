//===-- MINA32TargetInfo.h - MINA32 Target Implementation -------*- C++ -*-===//
//
// Part of the LLVM fork for the MINA32 project, under the Apache License v2.0
// with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
// information.
// SPDX-License-Identifier: Apache-2.0 with LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINA32_TARGET_INFO_H
#define LLVM_TARGET_MINA32_TARGET_INFO_H

namespace llvm {

class Target;

Target &getTheMINA32Target();

} // end namespace llvm

#endif // LLVM_TARGET_MINA32_TARGET_INFO_H