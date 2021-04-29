//===-- AVISysV_MINA32.h ----------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SOURCE_PLUGINS_ABI_MINA32_ABISYSV_MINA32_H
#define LLDB_SOURCE_PLUGINS_ABI_MINA32_ABISYSV_MINA32_H

#include "lldb/Target/ABI.h"
#include "lldb/lldb-private.h"

class ABISysV_MINA32 : public lldb_private::RegInfoBasedABI {
public:
  ~ABISysV_MINA32() override = default;

  size_t GetRedZoneSize() const override;

  bool PrepareTrivialCall(lldb_private::Thread &thread, lldb::addr_t sp,
                          lldb::addr_t functionAddress,
                          lldb::addr_t returnAddress,
                          llvm::ArrayRef<lldb::addr_t> args) const override;

  bool GetArgumentValues(lldb_private::Thread &thread,
                         lldb_private::ValueList &values) const override;

  lldb_private::Status
  SetReturnValueObject(lldb::StackFrameSP &frame_sp,
                       lldb::ValueObjectSP &new_value) override;

  lldb::ValueObjectSP
  GetReturnValueObjectImpl(lldb_private::Thread &thread,
                           lldb_private::CompilerType &type) const override;

  bool
  CreateFunctionEntryUnwindPlan(lldb_private::UnwindPlan &unwind_plan) override;

  bool CreateDefaultUnwindPlan(lldb_private::UnwindPlan &unwind_plan) override;

  bool RegisterIsVolatile(const lldb_private::RegisterInfo *reg_info) override;

  bool CallFrameAddressIsValid(lldb::addr_t cfa) override {
    // Stack call frame address must be 4 byte aligned.
    return (cfa & 0x3ull) == 0;
  }

  bool CodeAddressIsValid(lldb::addr_t pc) override {
    // Code address must be 4 byte aligned.
    return (pc & 3ull) == 0;
  }

  const lldb_private::RegisterInfo *
  GetRegisterInfoArray(uint32_t &count) override;

  //------------------------------------------------------------------
  // Static Functions
  //------------------------------------------------------------------

  static void Initialize();

  static void Terminate();

  static lldb::ABISP CreateInstance(lldb::ProcessSP process_sp,
                                    const lldb_private::ArchSpec &arch);

  static lldb_private::ConstString GetPluginNameStatic();

  //------------------------------------------------------------------
  // PluginInterface protocol
  //------------------------------------------------------------------

  lldb_private::ConstString GetPluginName() override;

  uint32_t GetPluginVersion() override;

protected:
  lldb::ValueObjectSP
  GetReturnValueObjectSimple(lldb_private::Thread &thread,
                             lldb_private::CompilerType &ast_type) const;

private:
  using lldb_private::RegInfoBasedABI::RegInfoBasedABI; // Call CreateInstance instead.
};

#endif // LLDB_SOURCE_PLUGINS_ABI_MINA32_ABISYSV_MINA32_H
