//===-- ABISysV_MINA32.cpp ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ABISysV_MINA32.h"

#include "llvm/ADT/Triple.h"
#include "llvm/IR/DerivedTypes.h"

#include "lldb/Core/Module.h"
#include "lldb/Core/PluginManager.h"
#include "lldb/Core/Value.h"
#include "lldb/Core/ValueObjectConstResult.h"
#include "lldb/Core/ValueObjectMemory.h"
#include "lldb/Core/ValueObjectRegister.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/RegisterContext.h"
#include "lldb/Target/StackFrame.h"
#include "lldb/Target/Target.h"
#include "lldb/Target/Thread.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/RegisterValue.h"
#include "lldb/Utility/Status.h"

using namespace lldb;
using namespace lldb_private;

LLDB_PLUGIN_DEFINE_ADV(ABISysV_MINA32, ABIMINA32)

static RegisterInfo g_register_infos[] =
{
    { "r0"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  0,  0, LLDB_INVALID_REGNUM,     0,  0 }, NULL, NULL, NULL, 0 },
    { "r1"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  1,  1, LLDB_INVALID_REGNUM,     1,  1 }, NULL, NULL, NULL, 0 },
    { "r2"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  2,  2, LLDB_INVALID_REGNUM,     2,  2 }, NULL, NULL, NULL, 0 },
    { "r3"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  3,  3, LLDB_INVALID_REGNUM,     3,  3 }, NULL, NULL, NULL, 0 },
    { "r4"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  4,  4, LLDB_INVALID_REGNUM,     4,  4 }, NULL, NULL, NULL, 0 },
    { "r5"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  5,  5, LLDB_INVALID_REGNUM,     5,  5 }, NULL, NULL, NULL, 0 },
    { "r6"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  6,  6, LLDB_INVALID_REGNUM,     6,  6 }, NULL, NULL, NULL, 0 },
    { "r7"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  7,  7, LLDB_INVALID_REGNUM,     7,  7 }, NULL, NULL, NULL, 0 },
    { "r8"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  8,  8, LLDB_INVALID_REGNUM,     8,  8 }, NULL, NULL, NULL, 0 },
    { "r9"  , NULL, 4, 0, eEncodingUint, eFormatHex, {  9,  9, LLDB_INVALID_REGNUM,     9,  9 }, NULL, NULL, NULL, 0 },
    { "r10" , NULL, 4, 0, eEncodingUint, eFormatHex, { 10, 10, LLDB_INVALID_REGNUM,    10, 10 }, NULL, NULL, NULL, 0 },
    { "r11" , NULL, 4, 0, eEncodingUint, eFormatHex, { 11, 11, LLDB_INVALID_REGNUM,    11, 11 }, NULL, NULL, NULL, 0 },
    { "r12" , NULL, 4, 0, eEncodingUint, eFormatHex, { 12, 12, LLDB_INVALID_REGNUM,    12, 12 }, NULL, NULL, NULL, 0 },
    { "r13" , NULL, 4, 0, eEncodingUint, eFormatHex, { 13, 13, LLDB_INVALID_REGNUM,    13, 13 }, NULL, NULL, NULL, 0 },
    { "r14" , "fp", 4, 0, eEncodingUint, eFormatHex, { 14, 14, LLDB_REGNUM_GENERIC_FP, 14, 14 }, NULL, NULL, NULL, 0 },
    { "r15" , "sp", 4, 0, eEncodingUint, eFormatHex, { 15, 15, LLDB_REGNUM_GENERIC_SP, 15, 15 }, NULL, NULL, NULL, 0 },
    { "mcr" , NULL, 4, 0, eEncodingUint, eFormatHex, { 16, 16, LLDB_INVALID_REGNUM,    16, 16 }, NULL, NULL, NULL, 0 },
    { "fret", NULL, 4, 0, eEncodingUint, eFormatHex, { 17, 17, LLDB_INVALID_REGNUM,    17, 17 }, NULL, NULL, NULL, 0 },
    { "pc"  , NULL, 4, 0, eEncodingUint, eFormatHex, { 18, 18, LLDB_REGNUM_GENERIC_PC, 18, 18 }, NULL, NULL, NULL, 0 }
};

static const uint32_t k_num_register_infos =
  sizeof(g_register_infos)/sizeof(RegisterInfo);
static bool g_register_info_names_constified = false;

const lldb_private::RegisterInfo *
ABISysV_MINA32::GetRegisterInfoArray ( uint32_t &count )
{
  // Make the C-string names and alt_names for the register infos into const
  // C-string values by having the ConstString unique the names in the global
  // constant C-string pool.
  if (!g_register_info_names_constified) {
    g_register_info_names_constified = true;
    for (uint32_t i = 0; i < k_num_register_infos; ++i) {
      RegisterInfo &reg = g_register_infos[i];
      if (reg.name) reg.name = ConstString(reg.name).GetCString();
      if (reg.alt_name) reg.alt_name = ConstString(reg.alt_name).GetCString();
    }
  }
  count = k_num_register_infos;
  return g_register_infos;
}

/*
    http://en.wikipedia.org/wiki/Red_zone_%28computing%29

    In computing, a red zone is a fixed size area in memory beyond the stack
   pointer that has not been
    "allocated". This region of memory is not to be modified by
   interrupt/exception/signal handlers.
    This allows the space to be used for temporary data without the extra
   overhead of modifying the
    stack pointer. The x86-64 ABI mandates a 128 byte red zone.[1] The OpenRISC
   toolchain assumes a
    128 byte red zone though it is not documented.
*/
size_t ABISysV_MINA32::GetRedZoneSize() const { return 0; }

//------------------------------------------------------------------
// Static Functions
//------------------------------------------------------------------

ABISP
ABISysV_MINA32::CreateInstance(lldb::ProcessSP process_sp, const ArchSpec &arch) {
  if (arch.GetTriple().getArch() == llvm::Triple::mina32) {
    return ABISP(
        new ABISysV_MINA32(std::move(process_sp), MakeMCRegisterInfo(arch)));
  }
  return ABISP();
}

bool ABISysV_MINA32::PrepareTrivialCall(Thread &thread, lldb::addr_t sp,
                                         lldb::addr_t pc, lldb::addr_t ra,
                                         llvm::ArrayRef<addr_t> args) const {
  return false;
}

bool ABISysV_MINA32::GetArgumentValues(Thread &thread,
                                       ValueList &values) const {
  return false;
}

Status
ABISysV_MINA32::SetReturnValueObject(lldb::StackFrameSP &frame_sp,
                                     lldb::ValueObjectSP &new_value_sp) {
  Status error;
  return error;
}

ValueObjectSP ABISysV_MINA32::GetReturnValueObjectSimple(
    Thread &thread, CompilerType &return_compiler_type) const {
  ValueObjectSP return_valobj_sp;
  return return_valobj_sp;
}

ValueObjectSP ABISysV_MINA32::GetReturnValueObjectImpl(
    Thread &thread, CompilerType &return_compiler_type) const {
  ValueObjectSP return_valobj_sp;
  return return_valobj_sp;
}

// called when we are on the first instruction of a new function
bool ABISysV_MINA32::CreateFunctionEntryUnwindPlan(UnwindPlan &unwind_plan) {
  unwind_plan.Clear();
  unwind_plan.SetRegisterKind(eRegisterKindGeneric);

  uint32_t pc_reg_num = LLDB_REGNUM_GENERIC_PC;

  UnwindPlan::RowSP row(new UnwindPlan::Row);

  // Our Call Frame Address is the stack pointer value
  row->GetCFAValue().SetIsRegisterPlusOffset(LLDB_REGNUM_GENERIC_SP, 0);
  row->SetRegisterLocationToAtCFAPlusOffset(pc_reg_num, 0, true);
  unwind_plan.AppendRow(row);

  unwind_plan.SetSourceName("mina32 at-func-entry default");
  unwind_plan.SetSourcedFromCompiler(eLazyBoolNo);
  return true;
}

bool ABISysV_MINA32::CreateDefaultUnwindPlan(UnwindPlan &unwind_plan) {
  unwind_plan.Clear();
  unwind_plan.SetRegisterKind(eRegisterKindGeneric);

  uint32_t fp_reg_num = LLDB_REGNUM_GENERIC_FP;
  uint32_t pc_reg_num = LLDB_REGNUM_GENERIC_PC;

  UnwindPlan::RowSP row(new UnwindPlan::Row);

  row->GetCFAValue().SetIsRegisterPlusOffset(LLDB_REGNUM_GENERIC_FP, 0);
  row->SetRegisterLocationToAtCFAPlusOffset(fp_reg_num, -4, true);
  row->SetRegisterLocationToAtCFAPlusOffset(pc_reg_num,  0, true);
  unwind_plan.AppendRow(row);

  unwind_plan.SetSourceName("mina32 default unwind plan");
  unwind_plan.SetSourcedFromCompiler(eLazyBoolNo);
  unwind_plan.SetUnwindPlanValidAtAllInstructions(eLazyBoolNo);
  unwind_plan.SetUnwindPlanForSignalTrap(eLazyBoolNo);
  return true;
}

bool ABISysV_MINA32::RegisterIsVolatile(const RegisterInfo *reg_info) {
  return (reg_info->byte_offset) / 4 < 4;
}

void ABISysV_MINA32::Initialize() {
  PluginManager::RegisterPlugin(GetPluginNameStatic(),
                                "System V ABI for MINA32 targets",
                                CreateInstance);
}

void ABISysV_MINA32::Terminate() {
  PluginManager::UnregisterPlugin(CreateInstance);
}

lldb_private::ConstString ABISysV_MINA32::GetPluginNameStatic() {
  static ConstString g_name("sysv-mina32");
  return g_name;
}

//------------------------------------------------------------------
// PluginInterface protocol
//------------------------------------------------------------------
lldb_private::ConstString ABISysV_MINA32::GetPluginName() {
  return GetPluginNameStatic();
}

uint32_t ABISysV_MINA32::GetPluginVersion() { return 1; }
