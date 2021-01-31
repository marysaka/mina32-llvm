set(LLVM_TARGET_DEFINITIONS MINA32.td)

tablegen(LLVM MINA32GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM MINA32GenAsmWriter.inc -gen-asm-writer)
# tablegen(LLVM MINA32GenCallingConv.inc -gen-callingconv)
tablegen(LLVM MINA32GenDAGISel.inc -gen-dag-isel)
# tablegen(LLVM MINA32GenDisassemblerTables.inc -gen-disassembler)
# tablegen(LLVM MINA32GenFastISel.inc -gen-fast-isel)
# tablegen(LLVM MINA32GenGlobalISel.inc -gen-global-isel)
tablegen(LLVM MINA32GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MINA32GenMCCodeEmitter.inc -gen-emitter)
# tablegen(LLVM MINA32GenMCPseudoLowering.inc -gen-pseudo-lowering)
# tablegen(LLVM MINA32GenRegisterBank.inc -gen-register-bank)
tablegen(LLVM MINA32GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MINA32GenSubtargetInfo.inc -gen-subtarget)
# tablegen(LLVM MINA32GenExegesis.inc -gen-exegesis)

add_public_tablegen_target(MINA32CommonTableGen)

add_llvm_target(MINA32CodeGen
  MINA32TargetMachine.cpp
  MINA32Subtarget.cpp
  MINA32RegisterInfo.cpp
  MINA32InstrInfo.cpp
  MINA32FrameLowering.cpp
  MINA32ISelLowering.cpp
  MINA32ISelDAGToDAG.cpp
 )

add_subdirectory(AsmParser)
# add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
